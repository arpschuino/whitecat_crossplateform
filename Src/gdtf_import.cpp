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
#include <map>

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

// [devices] AttributeDefinitions > Attributes : indice molette/bouton par nom d'attribut GDTF.
//   PhysicalUnit != None (Angle, ColorComponent...) -> continu (molette seule)
//   None + Feature "Control.*"                       -> mode (molette + selecteur de crans, ex. PositionMSpeed)
//   sinon                                            -> neutre (selecteur ajoute si beaucoup de crans nommes)
// [devices] GDTF PhysicalUnit (chaine) -> code wc::PhysUnit (affichage physique/level + suffixe).
static uint8_t phys_unit_from_gdtf(const char* pu)
{
    if(!pu || !*pu)                  return wc::PU_NONE;
    if(!strcmp(pu,"None"))           return wc::PU_NONE;
    if(!strcmp(pu,"Percent"))        return wc::PU_PERCENT;
    if(!strcmp(pu,"ColorComponent")) return wc::PU_COLORCOMPONENT;
    if(!strcmp(pu,"Angle"))          return wc::PU_ANGLE;
    if(!strcmp(pu,"AngularSpeed"))   return wc::PU_ANGULARSPEED;
    if(!strcmp(pu,"Frequency"))      return wc::PU_FREQUENCY;
    if(!strcmp(pu,"Time"))           return wc::PU_TIME;
    if(!strcmp(pu,"Length"))         return wc::PU_LENGTH;
    if(!strcmp(pu,"Temperature"))    return wc::PU_TEMPERATURE;
    if(!strcmp(pu,"Speed"))          return wc::PU_SPEED;
    return wc::PU_OTHER;             // autre unite dimensionnelle (Power, Voltage...) -> physique, sans suffixe connu
}

static void build_phys_hints(const wcxml::Node& root, std::map<std::string,uint8_t>& out,
                             std::map<std::string,uint8_t>& units_out)
{
    out.clear(); units_out.clear();
    const wcxml::Node* gdtf = root.child("GDTF");                 if(!gdtf)  return;
    const wcxml::Node* ft   = gdtf->child("FixtureType");         if(!ft)    return;
    const wcxml::Node* adef = ft->child("AttributeDefinitions");  if(!adef)  return;
    const wcxml::Node* attrs= adef->child("Attributes");          if(!attrs) return;
    for(size_t i=0;i<attrs->children.size();++i){
        const wcxml::Node& a = attrs->children[i];
        if(a.name != "Attribute") continue;
        const char* nm = a.attr("Name"); if(!nm||!*nm) continue;
        const char* pu = a.attr("PhysicalUnit");
        const char* fe = a.attr("Feature");
        uint8_t hint;
        if(pu && *pu && strcmp(pu,"None")!=0)     hint = wc::PHYS_CONTINUOUS;
        else if(fe && strncmp(fe,"Control",7)==0) hint = wc::PHYS_MODE;
        else                                      hint = wc::PHYS_PLAIN;
        out[nm] = hint;
        units_out[nm] = phys_unit_from_gdtf(pu);
    }
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

    std::map<std::string,uint8_t> phys_hints, phys_units;
    build_phys_hints(root, phys_hints, phys_units);   // nom d'attribut GDTF -> molette/mode + unite physique

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
        if(gname){ std::map<std::string,uint8_t>::const_iterator it=phys_hints.find(gname);      // molette/mode (GDTF PhysicalUnit+Feature)
                   ch.phys_hint = (it!=phys_hints.end()) ? it->second : (uint8_t)wc::PHYS_PLAIN;
                   std::map<std::string,uint8_t>::const_iterator itu=phys_units.find(gname);     // unite physique (GDTF PhysicalUnit)
                   ch.phys_unit = (itu!=phys_units.end()) ? itu->second : (uint8_t)wc::PU_NONE; }

        // [devices] slots nommes : LogicalChannel > ChannelFunction > ChannelSet (Name + DMXFrom)
        // + borne physique : PhysicalFrom/To de la 1re ChannelFunction (modele "un canal = une plage physique").
        {
            const wcxml::Node* lc2 = dc.child("LogicalChannel");
            if(lc2){
                bool got_phys=false; int cfCount=0;
                struct CFInfo { uint16_t dmx_from; float rf, rt; int named; std::string name; };
                std::vector<CFInfo> cfinfo;
                for(size_t k=0;k<lc2->children.size();++k){
                    const wcxml::Node& cf = lc2->children[k];
                    if(cf.name != "ChannelFunction") continue;
                    cfCount++;
                    const char* pf=cf.attr("PhysicalFrom"); const char* pt=cf.attr("PhysicalTo");
                    float rf = pf ? (float)atof(pf) : 0.0f;              // defauts GDTF : From=0, To=1
                    float rt = pt ? (float)atof(pt) : 1.0f;
                    if(!got_phys){ ch.phys_from=rf; ch.phys_to=rt; got_phys=true; }   // 1re CF : plage physique du canal (legacy)

                    int cfNamed=0;                                       // ChannelSet nommes de CETTE ChannelFunction
                    for(size_t s=0;s<cf.children.size();++s){
                        const wcxml::Node& cs = cf.children[s];
                        if(cs.name != "ChannelSet") continue;
                        const char* snm = cs.attr("Name");
                        if(!snm || !*snm) continue;                       // ignore les sets sans nom
                        cfNamed++;
                        wc::ChannelSlot slot;
                        slot.from16 = (uint16_t)parse_dmxvalue(cs.attr("DMXFrom"));
                        slot.name   = snm;
                        ch.slots.push_back(slot);
                    }
                    CFInfo ci; ci.dmx_from=(uint16_t)parse_dmxvalue(cf.attr("DMXFrom"));
                    ci.rf=rf; ci.rt=rt; ci.named=cfNamed;
                    const char* cfn=cf.attr("Name"); ci.name=(cfn&&*cfn)?cfn:(gname?gname:"");
                    cfinfo.push_back(ci);
                }
                std::sort(ch.slots.begin(), ch.slots.end(),
                          [](const wc::ChannelSlot& a, const wc::ChannelSlot& b){ return a.from16 < b.from16; });
                std::sort(cfinfo.begin(), cfinfo.end(),
                          [](const CFInfo& a, const CFInfo& b){ return a.dmx_from < b.dmx_from; });

                // [devices] tranches (modele "par tranches") : proportional = vrai balayage, sinon step.
                //  - valeur physique fixe (from==to)          -> step (mode/gobo/macro)
                //  - vraie plage non normalisee (deg/K/Hz...) -> proportional
                //  - plage 0..1 avec BEAUCOUP de reperes      -> step (liste discrete : gobos, filtres)
                //  - plage 0..1 sinon : LARGE = balayage (iris/dimmer/%), ETROITE = commande (reset/mode) -> span
                const unsigned SPAN_MIN = 6144;   // ~24 valeurs 8 bit : seuil balayage vs commande
                const unsigned GAP_MIN  = 2560;   // ~10 valeurs 8 bit : ecart moyen mini entre reperes pour une rampe
                for(size_t i=0;i<cfinfo.size();++i){
                    const CFInfo& ci=cfinfo[i];
                    unsigned span=(i+1<cfinfo.size()?cfinfo[i+1].dmx_from:65536u)-ci.dmx_from;
                    wc::ChannelRange rg;
                    rg.dmx_from=ci.dmx_from; rg.phys_from=ci.rf; rg.phys_to=ci.rt; rg.unit=ch.phys_unit; rg.name=ci.name;
                    float lo=ci.rf<ci.rt?ci.rf:ci.rt, hi=ci.rf<ci.rt?ci.rt:ci.rf;
                    bool norm01  = (lo>-0.001f&&lo<0.001f&&hi>0.999f&&hi<1.001f);
                    bool norm100 = (lo>-0.1f  &&lo<0.1f  &&hi>99.9f &&hi<100.1f);
                    bool realrange = (ci.rf!=ci.rt) && !norm01 && !norm100;
                    // reperes SERRES (ecart moyen faible) = liste discrete ; ESPACES = rampe a reperes.
                    bool dense = (ci.named>=2) && (span/(unsigned)ci.named < GAP_MIN);
                    // Ordre = inference geometrique (aucun champ GDTF ne donne prop/step) :
                    if(ci.rf==ci.rt)                   rg.proportional=false;   // 0) valeur physique fixe -> mode/step
                    else if(realrange)                 rg.proportional=true;    // 1) vraie plage physique (CTO/CTC/CRI) : les reperes sont des etiquettes
                    else if(dense)                     rg.proportional=false;   // 2) reperes serres -> liste discrete (gobos, modes)
                    else if(ci.named==0)               rg.proportional=false;   // 3) AUCUN repere + normalise -> zone reservee/passe-plat (LED freq reservee)
                    else if(ch.phys_unit!=wc::PU_NONE) rg.proportional=true;    // 4) unite dimensionnelle + repere -> balayage (Angle-pos, RGB)
                    else if(cfCount==1)                rg.proportional=true;    // 5) canal a fonction UNIQUE -> parametre continu (Tint)
                    else                               rg.proportional=(span>=SPAN_MIN); // 6) None multi-CF, avec repere : large=balayage, etroit=commande
                    ch.ranges.push_back(rg);
                }

                // [devices] Une SEULE ChannelFunction = fonction continue = PROPORTIONNEL -> molette, meme si
                // PhysicalUnit=None et crans nommes (ex. CTO 8000..2700K, CRI 80..90).
                //  - PLAIN (None neutre)          : promu toujours.
                //  - MODE  (Control.*)            : promu seulement si VRAIE plage physique non normalisee, pour
                //    distinguer CRIMode (80..90 -> molette) d'un vrai selecteur (PositionMSpeed 0..1 -> boutons).
                if(cfCount == 1){
                    if(ch.phys_hint == wc::PHYS_PLAIN) ch.phys_hint = wc::PHYS_CONTINUOUS;
                    else if(ch.phys_hint == wc::PHYS_MODE){
                        float lo=ch.phys_from, hi=ch.phys_to; if(lo>hi){ float t=lo; lo=hi; hi=t; }
                        bool real = (hi>lo) && !(lo>-0.001f&&lo<0.001f&&hi>0.999f&&hi<1.001f)
                                             && !(lo>-0.1f&&lo<0.1f&&hi>99.9f&&hi<100.1f);
                        if(real) ch.phys_hint = wc::PHYS_CONTINUOUS;
                    }
                }
            }
        }
        fx.channels.push_back(ch);
    }
    return fx.channels.empty() ? 2 : 0;
}

} // namespace wcgdtf
