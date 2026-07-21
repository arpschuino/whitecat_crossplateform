/*-------------------------------------------------------------------------------------------------------------
 White Cat - GDTF personality import (devices)
 Copyright (C) 2009-2016 Christoph Guillermet - Maintenance 2026 Jacques Bouault (arpschuino.fr)
 GNU General Public License v2 or later.
---------------------------------------------------------------------------------------------------------------*/

/**
* \file gdtf_import.cpp
* \brief Import GDTF (description.xml) -> wc::Fixture. Cf. gdtf_import.h.
 **/

#include "gdtf_import.h"
#include "wc_xml.h"
#include "channel.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace wcgdtf {

// --- lecture du fichier entier en memoire (cap de securite) ---
static bool read_file(const char* path, std::string& out)
{
    FILE* f = fopen(path, "rb");
    if(!f) return false;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if(n < 0 || n > 8*1024*1024){ fclose(f); return false; }   // > 8 Mo : ce n'est pas un description.xml (ex. .gdtf brut = zip)
    out.resize((size_t)n);
    size_t rd = n>0 ? fread(&out[0], 1, (size_t)n, f) : 0;
    fclose(f);
    out.resize(rd);
    return true;
}

// Le contenu ressemble-t-il a un description.xml GDTF (et pas a un binaire, ex. .gdtf = zip "PK..") ?
static bool looks_like_gdtf(const std::string& b)
{
    if(b.size()>=2 && b[0]=='P' && b[1]=='K') return false;   // archive zip (.gdtf brut, pas extrait)
    return b.find("<GDTF")!=std::string::npos || b.find("<FixtureType")!=std::string::npos;
}

// --- nom d'attribut GDTF -> wc::AttrId (0 = ATTR_NONE = non reconnu) ---
static uint8_t attr_from_gdtf(const char* name)
{
    if(!name) return wc::ATTR_NONE;
    if(!strcmp(name,"Dimmer"))     return wc::ATTR_DIMMER;
    if(!strcmp(name,"Pan"))        return wc::ATTR_PAN;
    if(!strcmp(name,"Tilt"))       return wc::ATTR_TILT;
    if(!strcmp(name,"ColorAdd_R")) return wc::ATTR_COLORADD_R;
    if(!strcmp(name,"ColorAdd_G")) return wc::ATTR_COLORADD_G;
    if(!strcmp(name,"ColorAdd_B")) return wc::ATTR_COLORADD_B;
    if(!strcmp(name,"ColorAdd_W")) return wc::ATTR_COLORADD_W;
    if(!strcmp(name,"Zoom"))       return wc::ATTR_ZOOM;
    if(!strcmp(name,"Shutter1"))   return wc::ATTR_SHUTTER1;
    return wc::ATTR_NONE;
}

// --- "4,5" -> coarse=4, fine=5 ; "2" -> coarse=2, fine=0 ; "None"/"" -> coarse=0 ---
static void parse_offset(const char* s, int& coarse, int& fine)
{
    coarse = 0; fine = 0;
    if(!s || !*s) return;
    if(!strcmp(s,"None")) return;
    coarse = atoi(s);
    const char* comma = strchr(s, ',');
    if(comma) fine = atoi(comma+1);
}

// DMXValue GDTF "X/n" (X sur n octets) -> valeur 16 bit (0..65535). "22/1"->22*257 ; "32768/2"->32768.
static int parse_dmxvalue(const char* s)
{
    if(!s || !*s) return 0;
    long v = atol(s);
    const char* slash = strchr(s,'/');
    int bytes = slash ? atoi(slash+1) : 1;
    if(bytes<=1) return (int)(v*257);                       // 8 bit -> 16 bit (x257)
    while(bytes>2){ v>>=8; bytes--; }                       // >2 octets : garder les 16 bits de poids fort
    if(v<0) v=0; if(v>65535) v=65535;
    return (int)v;
}

// Valeur par defaut d'un DMXChannel (16 bit) = Default de sa 1re ChannelFunction (fonction "home").
static int channel_default(const wcxml::Node& dmxch)
{
    const wcxml::Node* lc = dmxch.child("LogicalChannel");
    if(!lc) return 0;
    const wcxml::Node* cf = lc->child("ChannelFunction");
    if(!cf) return 0;
    return parse_dmxvalue(cf->attr("Default"));
}

// Attribut d'un DMXChannel : LogicalChannel[Attribute], sinon 1er ChannelFunction[Attribute].
static const char* channel_attribute(const wcxml::Node& dmxch)
{
    const wcxml::Node* lc = dmxch.child("LogicalChannel");
    if(lc){
        const char* a = lc->attr("Attribute");
        if(a) return a;
        const wcxml::Node* cf = lc->child("ChannelFunction");
        if(cf){ const char* a2 = cf->attr("Attribute"); if(a2) return a2; }
    }
    return 0;
}

// Descend GDTF > FixtureType > DMXModes ; renvoie NULL si structure absente.
static const wcxml::Node* find_dmxmodes(const wcxml::Node& root, std::string& fixture_name, std::string& manufacturer)
{
    const wcxml::Node* gdtf = root.child("GDTF");
    if(!gdtf) return 0;
    const wcxml::Node* ft = gdtf->child("FixtureType");
    if(!ft) return 0;
    const char* nm = ft->attr("Name");         if(nm) fixture_name = nm;
    const char* mf = ft->attr("Manufacturer");  if(mf) manufacturer = mf;
    return ft->child("DMXModes");
}

int list_modes(const char* xmlpath, std::vector<ModeInfo>& modes, std::string& fixture_name, std::string& manufacturer)
{
    modes.clear(); fixture_name.clear(); manufacturer.clear();
    std::string buf;
    if(!read_file(xmlpath, buf)) return 1;
    if(!looks_like_gdtf(buf)) return 2;   // pas un description.xml (ex. .gdtf brut = zip, ou binaire)
    wcxml::Node root = wcxml::parse(buf.c_str(), buf.size());

    const wcxml::Node* dmodes = find_dmxmodes(root, fixture_name, manufacturer);
    if(!dmodes) return 2;

    for(size_t i=0;i<dmodes->children.size();++i)
    {
        const wcxml::Node& m = dmodes->children[i];
        if(m.name != "DMXMode") continue;
        ModeInfo mi; mi.nb_channels=0; mi.footprint=0;
        const char* nm = m.attr("Name"); mi.name = nm ? nm : "";
        const wcxml::Node* chans = m.child("DMXChannels");
        if(chans){
            for(size_t c=0;c<chans->children.size();++c)
            {
                const wcxml::Node& dc = chans->children[c];
                if(dc.name != "DMXChannel") continue;
                int co=0,fi=0; parse_offset(dc.attr("Offset"), co, fi);
                if(co<=0) continue;                 // canal virtuel (pas d'adresse)
                mi.nb_channels++;
                if(co>mi.footprint) mi.footprint=co;
                if(fi>mi.footprint) mi.footprint=fi;
            }
        }
        modes.push_back(mi);
    }
    return modes.empty() ? 2 : 0;
}

int build_fixture(const char* xmlpath, int mode_index, int base, int circuit,
                  wc::Fixture& fx, std::string& mode_name, int& footprint)
{
    fx.channels.clear(); mode_name.clear(); footprint=0;
    std::string buf;
    if(!read_file(xmlpath, buf)) return 1;
    if(!looks_like_gdtf(buf)) return 2;   // pas un description.xml (ex. .gdtf brut = zip, ou binaire)
    wcxml::Node root = wcxml::parse(buf.c_str(), buf.size());

    std::string fixture_name, manufacturer;
    const wcxml::Node* dmodes = find_dmxmodes(root, fixture_name, manufacturer);
    if(!dmodes) return 2;
    if(!manufacturer.empty()) fixture_name = manufacturer + " " + fixture_name;

    // localiser le mode_index-ieme DMXMode
    const wcxml::Node* mode = 0; int mi=0;
    for(size_t i=0;i<dmodes->children.size();++i)
    {
        if(dmodes->children[i].name != "DMXMode") continue;
        if(mi==mode_index){ mode = &dmodes->children[i]; break; }
        ++mi;
    }
    if(!mode) return 3;
    const char* mnm = mode->attr("Name"); mode_name = mnm ? mnm : "";
    fx.name = fixture_name.empty() ? mode_name : (fixture_name + " (" + mode_name + ")");

    const wcxml::Node* chans = mode->child("DMXChannels");
    if(!chans) return 2;

    for(size_t c=0;c<chans->children.size();++c)
    {
        const wcxml::Node& dc = chans->children[c];
        if(dc.name != "DMXChannel") continue;

        int co=0,fi=0; parse_offset(dc.attr("Offset"), co, fi);
        if(co<=0) continue;                             // virtuel : pas d'adresse

        if(co>footprint) footprint=co;
        if(fi>footprint) footprint=fi;

        const char* gname = channel_attribute(dc);      // nom d'attribut GDTF (ex. "Pan", "Gobo1", "Prism1")
        uint8_t attr = attr_from_gdtf(gname);
        if(attr==wc::ATTR_NONE) attr = wc::ATTR_RAW;    // attribut non mappe : pilotable en generique via son nom

        int coarse_addr = base + co - 1;
        int fine_addr   = fi>0 ? base + fi - 1 : 0;
        if(coarse_addr<1 || coarse_addr>512) continue;
        if(fine_addr>512) fine_addr=0;

        wc::Channel ch;
        ch.attribute   = attr;
        ch.combine     = (attr==wc::ATTR_DIMMER) ? wc::COMBINE_HTP : wc::COMBINE_LTP;
        ch.resolution  = (fine_addr!=0) ? wc::RES_16BIT : wc::RES_8BIT;
        ch.curve       = 0;
        ch.universe    = 0;
        ch.coarse_addr = (uint16_t)coarse_addr;
        ch.fine_addr   = (uint16_t)fine_addr;
        ch.circuit     = (uint16_t)circuit;
        ch.home        = (uint16_t)channel_default(dc);   // valeur par defaut GDTF (16 bit) -> persistee dans le patch
        if(gname){ strncpy(ch.name, gname, sizeof(ch.name)-1); ch.name[sizeof(ch.name)-1]=0; }  // nom GDTF -> pilotage generique

        // [devices] slots nommes : LogicalChannel > ChannelFunction > ChannelSet (Name + DMXFrom)
        {
            const wcxml::Node* lc2 = dc.child("LogicalChannel");
            if(lc2){
                for(size_t k=0;k<lc2->children.size();++k){
                    const wcxml::Node& cf = lc2->children[k];
                    if(cf.name != "ChannelFunction") continue;
                    for(size_t s=0;s<cf.children.size();++s){
                        const wcxml::Node& cs = cf.children[s];
                        if(cs.name != "ChannelSet") continue;
                        const char* snm = cs.attr("Name");
                        if(!snm || !*snm) continue;                       // ignore les sets sans nom
                        wc::ChannelSlot slot;
                        slot.from16 = (uint16_t)parse_dmxvalue(cs.attr("DMXFrom"));
                        slot.name   = snm;
                        ch.slots.push_back(slot);
                    }
                }
                std::sort(ch.slots.begin(), ch.slots.end(),
                          [](const wc::ChannelSlot& a, const wc::ChannelSlot& b){ return a.from16 < b.from16; });
            }
        }
        fx.channels.push_back(ch);
    }
    return fx.channels.empty() ? 2 : 0;
}

} // namespace wcgdtf
