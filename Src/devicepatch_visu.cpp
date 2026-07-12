/*-------------------------------------------------------------------------------------------------------------
 White Cat - Patch a device window (devices)
 Copyright (C) 2009-2016 Christoph Guillermet - Maintenance 2026 Jacques Bouault (arpschuino.fr)
 GNU General Public License v2 or later.
---------------------------------------------------------------------------------------------------------------*/

/**
* \file devicepatch_visu.cpp
* \brief Fenetre "Patch a device" : bibliotheque locale (scan fixtures/), navigation 3 colonnes
*        Manufacturer -> Fixture -> Mode (regroupement par fabricant du XML) + recherche.
*
* Phase 1 (offline). Les champs circuit/adresse/quantite/offset + PATCH arrivent a l'etape suivante.
 **/

#include "wc_tus.h"
#include "gui_boutons_rebuild1.h"
#include "devicepatch_visu.h"
#include "gdtf_import.h"
#include "wc_inline_edit.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <cctype>
#ifndef _WIN32
#include <dirent.h>
#include <strings.h>
#endif

// ---------------------------------------------------------------------------
// Bibliotheque locale (dossier <exe>/fixtures/, recursif ; description GDTF *.xml).
// ---------------------------------------------------------------------------
struct DevFix {
    std::string manuf;
    std::string model;
    std::string path;
    std::vector<wcgdtf::ModeInfo> modes;
};
static std::vector<DevFix>     g_lib;
static std::vector<std::string> g_manufs;   // fabricants distincts (colonne de gauche)
static bool g_scanned = false;
static char g_search[64] = "";
static int  g_sel_manuf = 0;    // index dans g_manufs
static int  g_sel_fix   = -1;   // index dans g_lib
static int  g_sel_mode  = 0;

// champs de patch (chaines editables via wc_inline_edit)
static char g_circuit_s[8] = "1";
static char g_address_s[8] = "1";
static char g_qty_s[8]     = "1";
static char g_offset_s[8]  = "0";

// --- layout ---
static const int HDR_Y    = 74;    // en-tetes de colonnes
static const int LIST_Y   = 94;    // 1re ligne des listes
static const int ROWH     = 18;
static const int NVIS     = 8;
static const int COL_MAN_X = 20,  COL_MAN_W = 130;
static const int COL_FIX_X = 160, COL_FIX_W = 200;
static const int COL_MOD_X = 370, COL_MOD_W = 170;

// nom de fichier se terminant par ".xml" (insensible a la casse) ?
static bool ends_xml(const char* nm)
{
    size_t L = strlen(nm);
    if(L<4) return false;
    const char* e = nm+L-4;
    return e[0]=='.' && tolower((unsigned char)e[1])=='x'
        && tolower((unsigned char)e[2])=='m' && tolower((unsigned char)e[3])=='l';
}

static void devlib_add(const char* path)
{
    std::vector<wcgdtf::ModeInfo> modes;
    std::string name, manuf;
    if(wcgdtf::list_modes(path, modes, name, manuf)!=0) return;
    if(modes.empty()) return;
    if(manuf.empty()) manuf = "Unknown";
    DevFix d; d.manuf=manuf; d.model=name; d.path=path; d.modes=modes;
    g_lib.push_back(d);
}

// scan recursif d'un dossier (fixtures/*.xml + sous-dossiers)
static void devlib_scan_dir(const char* dir)
{
#ifdef _WIN32
    char search[760]; sprintf(search, "%s\\*", dir);
    WIN32_FIND_DATA f; HANDLE h=FindFirstFile(search,&f);
    if(h==INVALID_HANDLE_VALUE) return;
    do {
        if(f.cFileName[0]=='.') continue;
        char path[900]; sprintf(path,"%s\\%s",dir,f.cFileName);
        if(f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){ devlib_scan_dir(path); continue; }
        if(ends_xml(f.cFileName)) devlib_add(path);
    } while(FindNextFile(h,&f));
    FindClose(h);
#else
    DIR* d=opendir(dir);
    if(!d) return;
    struct dirent* e;
    while((e=readdir(d))!=NULL){
        if(e->d_name[0]=='.') continue;
        char path[900]; snprintf(path,sizeof(path),"%s/%s",dir,e->d_name);
        if(e->d_type==DT_DIR){ devlib_scan_dir(path); continue; }
        if(ends_xml(e->d_name)) devlib_add(path);
    }
    closedir(d);
#endif
}

static bool devfix_less(const DevFix& a, const DevFix& b)
{
    if(a.manuf!=b.manuf) return a.manuf<b.manuf;
    return a.model<b.model;
}

static void devlib_scan()
{
    g_lib.clear(); g_manufs.clear();
    char dir[600]; sprintf(dir, "%s" WC_DIRSEP "fixtures", mondirectory);
    devlib_scan_dir(dir);
    std::sort(g_lib.begin(), g_lib.end(), devfix_less);
    for(size_t i=0;i<g_lib.size();++i)
        if(g_manufs.empty() || g_manufs.back()!=g_lib[i].manuf)   // g_lib trie par manuf -> distincts contigus
            if(std::find(g_manufs.begin(),g_manufs.end(),g_lib[i].manuf)==g_manufs.end())
                g_manufs.push_back(g_lib[i].manuf);
    g_scanned=true;
}

// sous-chaine insensible a la casse
static bool ci_contains(const std::string& hay, const char* needle)
{
    if(!needle || !*needle) return true;
    std::string H=hay, N=needle;
    for(size_t i=0;i<H.size();++i) H[i]=(char)tolower((unsigned char)H[i]);
    for(size_t i=0;i<N.size();++i) N[i]=(char)tolower((unsigned char)N[i]);
    return H.find(N)!=std::string::npos;
}

// indices de g_lib a montrer dans la colonne Fixture : resultats de recherche (si recherche active),
// sinon fixtures du fabricant selectionne.
static void build_fixlist(std::vector<int>& out)
{
    out.clear();
    bool searching = (g_search[0]!=0);
    for(size_t i=0;i<g_lib.size();++i)
    {
        if(searching){
            std::string key = g_lib[i].manuf + " " + g_lib[i].model;
            if(ci_contains(key, g_search)) out.push_back((int)i);
        } else if(g_sel_manuf>=0 && g_sel_manuf<(int)g_manufs.size() && g_lib[i].manuf==g_manufs[g_sel_manuf]){
            out.push_back((int)i);
        }
    }
}

static void devlib_on_search_commit(){ g_sel_fix=-1; g_sel_mode=0; }

// footprint (nb d'adresses) du mode selectionne, 0 si pas de selection valide
static int sel_footprint()
{
    if(g_sel_fix<0 || g_sel_fix>=(int)g_lib.size()) return 0;
    const DevFix& d = g_lib[g_sel_fix];
    if(g_sel_mode<0 || g_sel_mode>=(int)d.modes.size()) return 0;
    return d.modes[g_sel_mode].footprint;
}

// prochain circuit libre (non utilise par le patch courant)
static int next_free_circuit()
{
    bool used[514]; for(int i=0;i<514;i++) used[i]=false;
    for(int o=1;o<514;o++){ int c=Patch[o]; if(c>0 && c<514) used[c]=true; }
    for(int c=1;c<513;c++) if(!used[c]) return c;
    return 1;
}

// prochaine adresse ou un bloc de <footprint> outputs consecutifs est entierement libre
static int next_free_address(int footprint)
{
    if(footprint<1) footprint=1;
    bool occ[514]; for(int i=0;i<514;i++) occ[i]=false;
    for(int o=1;o<513;o++) if(Patch[o]>0 || is_fine[o]) occ[o]=true;
    for(int a=1; a+footprint-1<=512; a++)
    {
        bool ok=true;
        for(int k=0;k<footprint;k++) if(occ[a+k]){ ok=false; break; }
        if(ok) return a;
    }
    return 1;
}

// pre-remplit circuit/adresse (prochain libre), qty=1, offset=0 pour la selection courante
static void autofill_fields()
{
    int fp = sel_footprint();
    if(fp<=0) return;
    sprintf(g_circuit_s, "%d", next_free_circuit());
    sprintf(g_address_s, "%d", next_free_address(fp));
    strcpy(g_qty_s, "1");
    strcpy(g_offset_s, "0");
}

// dessine une colonne-liste simple ; renvoie l'index (dans items) clique en mode logique via helper.
static void draw_list_row(int x, int y, int w, const char* text, bool selected)
{
    if(selected){ Rect hl(Vec2D(x-2, y-2), Vec2D(w, ROWH)); hl.SetRoundness(3); hl.Draw(CouleurConfig); }
    petitchiffre.Print(text, x+2, y+12);
}

// champ numerique editable (boite bleue) : saisie inline sur clic (gere dans do_logical)
static void draw_num_field(int x, int y, int w, char* buf)
{
    Rect box(Vec2D(x, y), Vec2D(w, 18));
    box.SetRoundness(4);
    box.Draw(CouleurConfig);
    box.DrawOutline(CouleurLigne);
    if(wc_inline_editing(buf)) wc_inline_render(x+4, y+13, y+2, y+16);
    else                       petitchiffre.Print(buf, x+4, y+13);
}

// geometrie des champs (relative a la fenetre)
static const int FLD_Y = 250;
static const int CIRC_X = 68,  CIRC_W = 42;
static const int ADDR_X = 178, ADDR_W = 46;
static const int QTY_X  = 268, QTY_W  = 30;
static const int OFF_X  = 356, OFF_W  = 40;
static const int PATCH_X = 412, PATCH_Y = 248, PATCH_W = 120, PATCH_H = 24;

// ---------------------------------------------------------------------------
int devicepatch_window(int xd, int yd)
{
    Rect Panel(Vec2D(xd, yd), Vec2D(devicepatch_window_w, devicepatch_window_h));
    Panel.SetRoundness(15);
    Panel.SetLineWidth(epaisseur_bordure_fenetre);
    Panel.Draw(CouleurFond);
    if (window_focus_id == W_DEVICEPATCH) { Panel.DrawOutline(CouleurFader); }
    else                                  { Panel.DrawOutline(CouleurLigne); }

    neuro.Print("PATCH A DEVICE", xd + 100, yd + 30);

    if(!g_scanned) devlib_scan();

    // --- recherche ---
    petitchiffre.Print("Search:", xd + 20, yd + 54);
    Rect SearchBox(Vec2D(xd+70, yd+42), Vec2D(250, 20));
    SearchBox.SetRoundness(5);
    SearchBox.Draw(CouleurConfig);
    SearchBox.DrawOutline(CouleurLigne);
    if(wc_inline_editing(g_search)) wc_inline_render(xd+75, yd+56, yd+44, yd+60);
    else petitchiffre.Print(g_search[0] ? g_search : "(click, type, Enter)", xd+75, yd+56);

    // --- en-tetes ---
    neuromoyen.Print("Manufacturer:", xd+COL_MAN_X, yd+HDR_Y);
    neuromoyen.Print("Fixture:",      xd+COL_FIX_X, yd+HDR_Y);
    neuromoyen.Print("Mode:",         xd+COL_MOD_X, yd+HDR_Y);

    // --- colonne Manufacturer ---
    for(int i=0;i<NVIS && i<(int)g_manufs.size(); ++i)
        draw_list_row(xd+COL_MAN_X, yd+LIST_Y + i*ROWH, COL_MAN_W, g_manufs[i].c_str(), i==g_sel_manuf && g_search[0]==0);

    // --- colonne Fixture ---
    std::vector<int> fixlist; build_fixlist(fixlist);
    for(int i=0;i<NVIS && i<(int)fixlist.size(); ++i)
    {
        int gi = fixlist[i];
        char row[64]; sprintf(row, "%.30s", g_lib[gi].model.c_str());
        draw_list_row(xd+COL_FIX_X, yd+LIST_Y + i*ROWH, COL_FIX_W, row, gi==g_sel_fix);
    }

    // --- colonne Mode ---
    if(g_sel_fix>=0 && g_sel_fix<(int)g_lib.size())
    {
        const DevFix& d = g_lib[g_sel_fix];
        for(int m=0;m<NVIS && m<(int)d.modes.size(); ++m)
        {
            char mr[48]; sprintf(mr, "%.16s (%d)", d.modes[m].name.c_str(), d.modes[m].footprint);
            draw_list_row(xd+COL_MOD_X, yd+LIST_Y + m*ROWH, COL_MOD_W, mr, m==g_sel_mode);
        }
    }

    // --- champs de patch + bouton PATCH ---
    petitchiffre.Print("Circuit:", xd+20,      yd+FLD_Y+13);
    draw_num_field(xd+CIRC_X, yd+FLD_Y, CIRC_W, g_circuit_s);
    petitchiffre.Print("Address:", xd+120,     yd+FLD_Y+13);
    draw_num_field(xd+ADDR_X, yd+FLD_Y, ADDR_W, g_address_s);
    petitchiffre.Print("Qty:", xd+238,         yd+FLD_Y+13);
    draw_num_field(xd+QTY_X, yd+FLD_Y, QTY_W, g_qty_s);
    petitchiffre.Print("Offset:", xd+308,      yd+FLD_Y+13);
    draw_num_field(xd+OFF_X, yd+FLD_Y, OFF_W, g_offset_s);

    Rect PatchBtn(Vec2D(xd+PATCH_X, yd+PATCH_Y), Vec2D(PATCH_W, PATCH_H));
    PatchBtn.SetRoundness(6);
    bool can_patch = (g_sel_fix>=0 && sel_footprint()>0);
    if(can_patch) PatchBtn.Draw(CouleurFader);
    PatchBtn.DrawOutline(CouleurLigne);
    neuromoyen.Print("PATCH", xd+PATCH_X+30, yd+PATCH_Y+17);

    // --- recap ---
    if(can_patch)
    {
        int fp=sel_footprint();
        int circ=atoi(g_circuit_s), addr=atoi(g_address_s), qty=atoi(g_qty_s), off=atoi(g_offset_s);
        if(qty<1) qty=1;
        int step=fp+off;
        char recap[128];
        sprintf(recap, "%d x %.20s [%.14s] : circuits %d-%d, addr %d-%d  (fp %d)",
                qty, g_lib[g_sel_fix].model.c_str(),
                g_lib[g_sel_fix].modes[g_sel_mode].name.c_str(),
                circ, circ+qty-1, addr, addr+qty*step-1, fp);
        petitchiffre.Print(recap, xd+20, yd+FLD_Y+42);
    }

    return(0);
}

int do_logical_devicepatch(int xd, int yd)
{
    if(!g_scanned) devlib_scan();

    // clic dans la boite de recherche -> edition inline
    if(mouse_released==0 && mouse_x>xd+70 && mouse_x<xd+320 && mouse_y>yd+42 && mouse_y<yd+62)
    {
        wc_inline_begin(g_search, sizeof(g_search), 240, devlib_on_search_commit);
        mouse_released=1;
    }

    // clic sur un fabricant -> le selectionne (et sort du mode recherche)
    for(int i=0;i<NVIS && i<(int)g_manufs.size(); ++i)
    {
        int ry = yd+LIST_Y + i*ROWH;
        if(mouse_released==0 && mouse_x>xd+COL_MAN_X-2 && mouse_x<xd+COL_MAN_X-2+COL_MAN_W && mouse_y>ry-2 && mouse_y<ry+ROWH-2)
        {
            g_sel_manuf=i; g_search[0]=0; g_sel_fix=-1; g_sel_mode=0; mouse_released=1;
        }
    }

    // clic sur une fixture
    std::vector<int> fixlist; build_fixlist(fixlist);
    for(int i=0;i<NVIS && i<(int)fixlist.size(); ++i)
    {
        int ry = yd+LIST_Y + i*ROWH;
        if(mouse_released==0 && mouse_x>xd+COL_FIX_X-2 && mouse_x<xd+COL_FIX_X-2+COL_FIX_W && mouse_y>ry-2 && mouse_y<ry+ROWH-2)
        {
            g_sel_fix=fixlist[i]; g_sel_mode=0; autofill_fields(); mouse_released=1;
        }
    }

    // clic sur un mode
    if(g_sel_fix>=0 && g_sel_fix<(int)g_lib.size())
    {
        const DevFix& d = g_lib[g_sel_fix];
        for(int m=0;m<NVIS && m<(int)d.modes.size(); ++m)
        {
            int ry = yd+LIST_Y + m*ROWH;
            if(mouse_released==0 && mouse_x>xd+COL_MOD_X-2 && mouse_x<xd+COL_MOD_X-2+COL_MOD_W && mouse_y>ry-2 && mouse_y<ry+ROWH-2)
            {
                g_sel_mode=m; autofill_fields(); mouse_released=1;
            }
        }
    }

    // clics sur les champs numeriques -> edition inline
    int fy = yd+FLD_Y;
    if(mouse_released==0 && mouse_y>fy && mouse_y<fy+18)
    {
        if(mouse_x>xd+CIRC_X && mouse_x<xd+CIRC_X+CIRC_W){ wc_inline_begin(g_circuit_s,sizeof(g_circuit_s),CIRC_W-6,0); mouse_released=1; }
        else if(mouse_x>xd+ADDR_X && mouse_x<xd+ADDR_X+ADDR_W){ wc_inline_begin(g_address_s,sizeof(g_address_s),ADDR_W-6,0); mouse_released=1; }
        else if(mouse_x>xd+QTY_X  && mouse_x<xd+QTY_X+QTY_W){ wc_inline_begin(g_qty_s,sizeof(g_qty_s),QTY_W-6,0); mouse_released=1; }
        else if(mouse_x>xd+OFF_X  && mouse_x<xd+OFF_X+OFF_W){ wc_inline_begin(g_offset_s,sizeof(g_offset_s),OFF_W-6,0); mouse_released=1; }
    }

    // bouton PATCH
    if(mouse_released==0 && g_sel_fix>=0 && g_sel_fix<(int)g_lib.size()
       && mouse_x>xd+PATCH_X && mouse_x<xd+PATCH_X+PATCH_W && mouse_y>yd+PATCH_Y && mouse_y<yd+PATCH_Y+PATCH_H)
    {
        if(wc_inline_active()) wc_inline_commit();   // valide un champ en cours d'edition
        int fp   = sel_footprint();
        int circ = atoi(g_circuit_s);
        int addr = atoi(g_address_s);
        int qty  = atoi(g_qty_s);
        int off  = atoi(g_offset_s);
        if(fp>0)
        {
            if(qty<1) qty=1; if(qty>512) qty=512;
            if(off<0) off=0;
            int step = fp+off;
            int done=0;
            for(int i=0;i<qty;i++)
            {
                int a = addr + i*step;
                int c = circ + i;
                if(a<1 || a+fp-1>512 || c<1 || c>512) break;
                if(create_device_from_gdtf_at(g_lib[g_sel_fix].path.c_str(), g_sel_mode, a, c)==0) done++;
            }
            // avancer les champs pour un prochain lot
            sprintf(g_circuit_s, "%d", circ+done);
            sprintf(g_address_s, "%d", addr+done*step);
            sprintf(string_Last_Order, ">> patched %d x %.24s @%d ch%d", done, g_lib[g_sel_fix].model.c_str(), addr, circ);
        }
        mouse_released=1;
    }

    return(0);
}
