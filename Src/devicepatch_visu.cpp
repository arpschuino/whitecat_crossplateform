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
#include "gdtf_share.h"
#include "wc_zip.h"
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
    std::string path;   // fichier local (vide si online-only)
    int         rid;    // revision GDTF Share (0 = local)
    std::vector<wcgdtf::ModeInfo> modes;
};
static std::vector<DevFix>      g_local;    // fixtures des fichiers locaux (fixtures/)
static std::vector<DevFix>      g_lib;      // liste fusionnee (local + index en ligne) affichee
static std::vector<std::string> g_manufs;   // fabricants distincts (colonne de gauche)
static bool g_scanned = false;
static char g_search[64] = "";
static bool g_filter_local = false;   // filtre All (false) / Local (true : seulement les fixtures avec fichier local)
// compte GDTF Share + etat
static char g_user[64] = "";
static char g_pass[64] = "";
static char g_status[96] = "";
static int  g_sel_manuf = 0;    // index dans g_manufs
static int  g_sel_fix   = -1;   // index dans g_lib
static int  g_sel_mode  = 0;
// defilement des 3 colonnes + molette
static int  g_man_scroll = 0, g_fix_scroll = 0, g_mode_scroll = 0;
static int  g_man_count = 0, g_fix_count = 0, g_mode_count = 0;   // tailles (pour clamp molette)
static int  g_wheel_col  = 0;   // colonne survolee pour la molette (1=manuf 2=fix 3=mode ; 0=aucune)

// champs de patch (chaines editables via wc_inline_edit)
static char g_circuit_s[8] = "1";
static char g_address_s[8] = "1";
static char g_qty_s[8]     = "1";
static char g_offset_s[8]  = "0";

// --- layout ---
static const int ACCT_Y   = 46;    // ligne compte GDTF Share
static const int SRCH_Y   = 62;    // boite de recherche (remontee de 10 px)
static const int HDR_Y    = 98;    // en-tetes de colonnes
static const int LIST_Y   = 118;   // 1re ligne des listes
static const int ROWH     = 18;
static const int NVIS     = 10;
static const int COL_MAN_X = 20,  COL_MAN_W = 130;
static const int COL_FIX_X = 160, COL_FIX_W = 200;
static const int COL_MOD_X = 370, COL_MOD_W = 170;
// compte GDTF Share
static const int USER_X = 70,  USER_W = 100;
static const int PASS_X = 180, PASS_W = 100;
static const int UPD_X  = 290, UPD_W  = 70, UPD_H = 22;
static const int SRCH_X = 70,  SRCH_W = 250;

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
    DevFix d; d.manuf=manuf; d.model=name; d.path=path; d.rid=0; d.modes=modes;
    g_local.push_back(d);
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

// comparaison insensible a la casse (ordre alphabetique normal, pas MAJUSCULES puis minuscules)
static int ci_cmp(const std::string& a, const std::string& b)
{
    size_t n = a.size()<b.size()?a.size():b.size();
    for(size_t i=0;i<n;++i){ int ca=tolower((unsigned char)a[i]), cb=tolower((unsigned char)b[i]); if(ca!=cb) return ca-cb; }
    return (int)a.size()-(int)b.size();
}
static bool devfix_less(const DevFix& a, const DevFix& b)
{
    bool ga=(ci_cmp(a.manuf,"Generic")==0), gb=(ci_cmp(b.manuf,"Generic")==0);
    if(ga!=gb) return ga;                     // "Generic" en tete (les fabricants restent contigus = pas de doublon)
    int c=ci_cmp(a.manuf,b.manuf); if(c) return c<0;
    return ci_cmp(a.model,b.model)<0;
}

// reconstruit la colonne fabricants (distincts) en respectant le filtre All/Local
static void rebuild_manufs()
{
    g_manufs.clear();
    for(size_t i=0;i<g_lib.size();++i){
        if(g_filter_local && g_lib[i].path.empty()) continue;    // filtre Local
        if(g_manufs.empty() || g_manufs.back()!=g_lib[i].manuf)  // g_lib trie par manuf -> distincts contigus
            g_manufs.push_back(g_lib[i].manuf);
    }
    if(g_sel_manuf>=(int)g_manufs.size()) g_sel_manuf=0;
}

// reconstruit g_lib = fixtures locales + index GDTF Share en ligne, puis la colonne fabricants
static void rebuild_lib()
{
    g_lib = g_local;
    const std::vector<gdtfshare::Fix>& online = gdtfshare::index();
    g_lib.reserve(g_lib.size()+online.size());
    for(size_t i=0;i<online.size();++i)
    {
        DevFix d; d.manuf=online[i].manuf; d.model=online[i].fixture; d.rid=online[i].rid;
        for(size_t m=0;m<online[i].modes.size();++m)
        {
            wcgdtf::ModeInfo mi; mi.name=online[i].modes[m].name;
            mi.nb_channels=0; mi.footprint=online[i].modes[m].footprint;
            d.modes.push_back(mi);
        }
        g_lib.push_back(d);
    }
    std::sort(g_lib.begin(), g_lib.end(), devfix_less);
    rebuild_manufs();
}

// compte GDTF Share persiste (user/gdtf_account.txt : 1 ligne user, 1 ligne pass ; en clair, comme BlenderDMX)
static void account_path(char* out, size_t n){ snprintf(out,n,"%s" WC_DIRSEP "user" WC_DIRSEP "gdtf_account.txt", mondirectory); }
static void account_load()
{
    char p[600]; account_path(p,sizeof(p));
    FILE* fp=fopen(p,"rt"); if(!fp) return;
    if(fgets(g_user,sizeof(g_user),fp)){ size_t l=strlen(g_user); while(l&&(g_user[l-1]=='\n'||g_user[l-1]=='\r'))g_user[--l]=0; }
    if(fgets(g_pass,sizeof(g_pass),fp)){ size_t l=strlen(g_pass); while(l&&(g_pass[l-1]=='\n'||g_pass[l-1]=='\r'))g_pass[--l]=0; }
    fclose(fp);
}
static void account_save()
{
    char p[600]; account_path(p,sizeof(p));
    FILE* fp=fopen(p,"wt"); if(!fp) return;
    fprintf(fp,"%s\n%s\n", g_user, g_pass);
    fclose(fp);
}

// --- panneau compte GDTF Share (reutilisable : popup fenetre Patch + ecran Config) ---
static void draw_field(int x, int y, int w, char* buf, bool mask);   // defini plus bas
static bool g_show_account = false;   // popup ouverte dans la fenetre Patch
static bool g_pass_show    = false;   // oeil : afficher le mot de passe en clair

// applique login + getList + save + rebuild (partage par la popup et Config)
static void gdtf_do_update()
{
    if(wc_inline_active()) wc_inline_commit();
    std::string err; int count=0;
    if(g_user[0]==0){ snprintf(g_status,sizeof(g_status),"enter user/password"); sprintf(string_Last_Order,">> GDTF Share: enter user/password"); return; }
    sprintf(string_Last_Order,">> GDTF Share: connecting as %.30s ...", g_user);
    if(gdtfshare::login(g_user, g_pass, err)!=0){ snprintf(g_status,sizeof(g_status),"login failed: %.55s", err.c_str()); sprintf(string_Last_Order,">> GDTF Share login FAILED: %.60s", err.c_str()); return; }
    account_save();
    if(gdtfshare::update_index(err, count)!=0){ snprintf(g_status,sizeof(g_status),"index failed: %.55s", err.c_str()); sprintf(string_Last_Order,">> GDTF Share index FAILED: %.60s", err.c_str()); return; }
    rebuild_lib();
    snprintf(g_status,sizeof(g_status),"index updated: %d fixtures", count);
    sprintf(string_Last_Order,">> GDTF Share index updated: %d fixtures", count);
}

// dessine le panneau a partir de (x,y). Geometrie partagee avec gdtf_account_logical.
void gdtf_account_draw(int x, int y)
{
    neuromoyen.Print("GDTF Share account", x, y);
    petitchiffre.Print("User:", x, y+34);   draw_field(x+50, y+22, 150, g_user, false);
    petitchiffre.Print("Pass:", x, y+58);   draw_field(x+50, y+46, 150, g_pass, !g_pass_show);
    // oeil afficher/masquer le mot de passe (amande lissee : demi-ellipse par segments + pupille)
    {
        int ex=x+50+150+14, ey=y+55;
        const int W=10, H=5, N=8;
        int pxu=ex-W, pyu=ey, pxl=ex-W, pyl=ey;
        for(int i=1;i<=N;i++){
            double t=-1.0+2.0*i/N;
            int px=ex+(int)(W*t);
            int dy=(int)(H*(1.0 - t*t));   // demi-ellipse (parabole) : paupiere arrondie
            if(dy<0) dy=0;
            Line(Vec2D(pxu,pyu),Vec2D(px,ey-dy)).Draw(CouleurLigne); pxu=px; pyu=ey-dy;
            Line(Vec2D(pxl,pyl),Vec2D(px,ey+dy)).Draw(CouleurLigne); pxl=px; pyl=ey+dy;
        }
        Circle pup(Vec2D(ex,ey),2); pup.Draw(CouleurLigne);
        if(!g_pass_show) Line(Vec2D(ex-10,ey+7),Vec2D(ex+10,ey-7)).Draw(CouleurLigne);   // barre = masque
    }
    Rect Upd(Vec2D(x, y+72), Vec2D(90, 22)); Upd.SetRoundness(5);
    if(gdtfshare::logged_in()) Upd.Draw(CouleurConfig);
    Upd.DrawOutline(CouleurLigne);
    petitchiffre.Print("Update", x+18, y+87);
    if(g_status[0]) petitpetitchiffre.Print(g_status, x, y+112);
}

void gdtf_account_logical(int x, int y)
{
    if(mouse_released==0 && mouse_x>x+50 && mouse_x<x+200){
        if(mouse_y>y+22 && mouse_y<y+40){ wc_inline_begin(g_user,sizeof(g_user),140,0); mouse_released=1; }
        else if(mouse_y>y+46 && mouse_y<y+64){ wc_inline_begin(g_pass,sizeof(g_pass),140,0); mouse_released=1; }
    }
    // oeil (afficher/masquer mot de passe)
    if(mouse_released==0 && mouse_x>x+50+150+6 && mouse_x<x+50+150+22 && mouse_y>y+48 && mouse_y<y+62){
        g_pass_show=!g_pass_show; mouse_released=1;
    }
    if(mouse_released==0 && mouse_x>x && mouse_x<x+90 && mouse_y>y+72 && mouse_y<y+94){
        gdtf_do_update(); mouse_released=1;
    }
}

static void devlib_scan()
{
    account_load();
    g_local.clear();
    char dir[600]; sprintf(dir, "%s" WC_DIRSEP "fixtures", mondirectory);
    devlib_scan_dir(dir);
    gdtfshare::load_cache();   // index en ligne mis en cache (offline / demarrage)
    rebuild_lib();
    int n=(int)gdtfshare::index().size();
    if(n>0) snprintf(g_status,sizeof(g_status),"%d online + %d local (cached)", n, (int)g_local.size());
    else    snprintf(g_status,sizeof(g_status),"%d local  -  login + Update to get online library", (int)g_local.size());
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
        if(g_filter_local && g_lib[i].path.empty()) continue;    // filtre Local : seulement les fixtures a fichier local
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

// nom de fichier sain (alnum -> garde, reste -> '_')
static std::string sanitize(const std::string& s)
{
    std::string o; o.reserve(s.size());
    for(size_t i=0;i<s.size();++i){ char c=s[i]; o += (isalnum((unsigned char)c)? c : '_'); }
    return o;
}

// s'assure que la fixture est disponible en LOCAL : si online (rid, pas de path), telecharge le .gdtf,
// extrait description.xml, met en cache (fixtures/cache/) et renseigne fx.path. Retour true si patchable.
static bool ensure_local(DevFix& fx)
{
    if(!fx.path.empty()) return true;
    if(fx.rid<=0){ snprintf(g_status,sizeof(g_status),"no file for this fixture"); return false; }
    if(!gdtfshare::logged_in()){ snprintf(g_status,sizeof(g_status),"login first (Update)"); sprintf(string_Last_Order,">> GDTF Share: login first (Update) to download online fixtures"); return false; }

    std::string bytes, err;
    if(gdtfshare::download(fx.rid, bytes, err)!=0){ snprintf(g_status,sizeof(g_status),"download failed: %.48s", err.c_str()); sprintf(string_Last_Order,">> GDTF Share download FAILED: %.50s", err.c_str()); return false; }
    std::string xml;
    if(!wczip::extract(bytes.c_str(), bytes.size(), "description.xml", xml)){ snprintf(g_status,sizeof(g_status),"unzip failed"); sprintf(string_Last_Order,">> GDTF Share: unzip failed"); return false; }

    char dir[600]; snprintf(dir,sizeof(dir),"%s" WC_DIRSEP "fixtures" WC_DIRSEP "cache", mondirectory);
    wc_mkdir(dir);
    std::string fn = sanitize(fx.manuf) + "@" + sanitize(fx.model) + "@" + std::to_string(fx.rid) + ".xml";
    char path[900]; snprintf(path,sizeof(path),"%s" WC_DIRSEP "%s", dir, fn.c_str());
    FILE* fp=fopen(path,"wb");
    if(!fp){ snprintf(g_status,sizeof(g_status),"cache write failed"); return false; }
    fwrite(xml.data(),1,xml.size(),fp); fclose(fp);
    fx.path = path;
    return true;
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

// Tronque `full` pour tenir dans maxw pixels (police petitchiffre) et ajoute "…". Renvoie true si tronque.
static bool fit_ellipsis(const char* full, int maxw, std::string& out)
{
    out = full ? full : "";
    if(petitchiffre.TextWidth(out.c_str()) <= maxw) return false;
    const char* ell = "\xE2\x80\xA6";   // … (UTF-8)
    while(!out.empty()){
        out.pop_back();
        std::string cand = out + ell;
        if(petitchiffre.TextWidth(cand.c_str()) <= maxw){ out = cand; return true; }
    }
    out = ell; return true;
}

// nom complet de la ligne survolee (si tronquee) -> affiche en tooltip apres les colonnes
static std::string g_tip; static int g_tip_x=0, g_tip_y=0;

// dessine une colonne-liste simple ; tronque au pixel avec "…" et memorise le tooltip au survol.
static void draw_list_row(int x, int y, int w, const char* text, bool selected)
{
    if(selected){ Rect hl(Vec2D(x-2, y-2), Vec2D(w, ROWH)); hl.SetRoundness(3); hl.Draw(CouleurConfig); }
    std::string shown;
    bool trunc = fit_ellipsis(text, w - 14, shown);   // -14 : padding + place de l'ascenseur
    petitchiffre.Print(shown.c_str(), x+2, y+12);
    if(trunc && window_focus_id==W_DEVICEPATCH &&
       mouse_x>x-2 && mouse_x<x-2+w && mouse_y>y-2 && mouse_y<y-2+ROWH){
        g_tip = text; g_tip_x = x; g_tip_y = y;
    }
}

static const int SBW = 10;   // largeur de l'ascenseur

static int clamp_scroll(int s, int count){ int m=count-NVIS; if(m<0)m=0; if(s>m)s=m; if(s<0)s=0; return s; }

// ascenseur graphique (piste + curseur) colle au bord droit <right> d'une colonne. Drag gere dans do_logical.
static void draw_scrollbar(int right, int y, int count, int scroll)
{
    if(count<=NVIS) return;
    int x = right - SBW;
    int trackh = NVIS*ROWH;
    int thumbh = trackh*NVIS/count; if(thumbh<16) thumbh=16;
    int maxs = count-NVIS;
    int ty = y + (maxs>0 ? (trackh-thumbh)*scroll/maxs : 0);
    Rect track(Vec2D(x, y), Vec2D(SBW, trackh)); track.SetRoundness(3); track.Draw(CouleurGrisAnthracite);
    Rect thumb(Vec2D(x, ty), Vec2D(SBW, thumbh)); thumb.SetRoundness(3); thumb.Draw(CouleurGrisMoyen);
}
// position/hauteur du curseur pour le drag (do_logical)
static void scrollbar_thumb_geom(int y, int count, int scroll, int& ty, int& th)
{
    int trackh=NVIS*ROWH; th=trackh*NVIS/count; if(th<12)th=12; int maxs=count-NVIS;
    ty = y + (maxs>0 ? (trackh-th)*scroll/maxs : 0);
}
static int scroll_from_thumb(int y, int count, int mouse_y_in)
{
    int trackh=NVIS*ROWH; int th=trackh*NVIS/count; if(th<12)th=12; int maxs=count-NVIS;
    int rel = mouse_y_in - y - th/2;
    if(trackh-th<=0) return 0;
    int s = rel*maxs/(trackh-th);
    return clamp_scroll(s,count);
}

// molette sur les colonnes (appelee depuis DoMouseLevel via win_under==W_DEVICEPATCH)
int devicepatch_wheel_col(){ return g_wheel_col; }
void devicepatch_wheel(int steps)
{
    if(g_wheel_col==1)      g_man_scroll  = clamp_scroll(g_man_scroll+steps,  g_man_count);
    else if(g_wheel_col==2) g_fix_scroll  = clamp_scroll(g_fix_scroll+steps,  g_fix_count);
    else if(g_wheel_col==3) g_mode_scroll = clamp_scroll(g_mode_scroll+steps, g_mode_count);
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

// champ texte editable (masque optionnel pour le mot de passe)
static void draw_field(int x, int y, int w, char* buf, bool mask)
{
    Rect box(Vec2D(x, y), Vec2D(w, 18));
    box.SetRoundness(4);
    box.Draw(CouleurConfig);
    box.DrawOutline(CouleurLigne);
    if(wc_inline_editing(buf)) { wc_inline_render(x+4, y+13, y+2, y+16); return; }
    if(mask){ std::string s(strlen(buf), '*'); petitchiffre.Print(s.c_str(), x+4, y+13); }
    else      petitchiffre.Print(buf, x+4, y+13);
}

// geometrie des champs de patch (relative a la fenetre)
static const int FLD_Y = 312;
static const int CIRC_X = 68,  CIRC_W = 42;
static const int ADDR_X = 178, ADDR_W = 46;
static const int QTY_X  = 268, QTY_W  = 30;
static const int OFF_X  = 356, OFF_W  = 40;
static const int PATCH_X = 412, PATCH_Y = 308, PATCH_W = 120, PATCH_H = 24;

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

    // --- bouton GDTF Share (ouvre la popup compte) ---
    {
        Rect Btn(Vec2D(xd+devicepatch_window_w-135, yd+18), Vec2D(120, 22));
        Btn.SetRoundness(5);
        if(g_show_account || gdtfshare::logged_in()) Btn.Draw(CouleurConfig);
        Btn.DrawOutline(CouleurLigne);
        petitchiffre.Print("GDTF Share", xd+devicepatch_window_w-125, yd+33);
    }

    // --- recherche + filtre All/Local ---
    petitchiffre.Print("Search:", xd+20, yd+SRCH_Y+13);
    draw_field(xd+SRCH_X, yd+SRCH_Y, SRCH_W, g_search, false);
    {
        Rect Flt(Vec2D(xd+SRCH_X+SRCH_W+10, yd+SRCH_Y), Vec2D(70,18));
        Flt.SetRoundness(5);
        if(g_filter_local) Flt.Draw(CouleurConfig);
        Flt.DrawOutline(CouleurLigne);
        petitchiffre.Print(g_filter_local ? "Local" : "All", xd+SRCH_X+SRCH_W+24, yd+SRCH_Y+13);
    }

    // --- en-tetes ---
    neuromoyen.Print("Manufacturer:", xd+COL_MAN_X, yd+HDR_Y);
    neuromoyen.Print("Fixture:",      xd+COL_FIX_X, yd+HDR_Y);
    neuromoyen.Print("Mode:",         xd+COL_MOD_X, yd+HDR_Y);

    // --- colonnes defilables ---
    std::vector<int> fixlist; build_fixlist(fixlist);
    const DevFix* seld = (g_sel_fix>=0 && g_sel_fix<(int)g_lib.size()) ? &g_lib[g_sel_fix] : 0;
    g_man_count=(int)g_manufs.size();
    g_fix_count=(int)fixlist.size();
    g_mode_count=seld? (int)seld->modes.size() : 0;
    g_man_scroll=clamp_scroll(g_man_scroll,g_man_count);
    g_fix_scroll=clamp_scroll(g_fix_scroll,g_fix_count);
    g_mode_scroll=clamp_scroll(g_mode_scroll,g_mode_count);

    // colonne survolee (pour la molette, consommee dans DoMouseLevel)
    g_wheel_col=0;
    { int ly0=yd+LIST_Y, ly1=yd+LIST_Y+NVIS*ROWH;
      if(window_focus_id==W_DEVICEPATCH && mouse_y>ly0 && mouse_y<ly1){
        if(mouse_x>xd+COL_MAN_X && mouse_x<xd+COL_MAN_X+COL_MAN_W) g_wheel_col=1;
        else if(mouse_x>xd+COL_FIX_X && mouse_x<xd+COL_FIX_X+COL_FIX_W) g_wheel_col=2;
        else if(mouse_x>xd+COL_MOD_X && mouse_x<xd+COL_MOD_X+COL_MOD_W) g_wheel_col=3; } }

    g_tip.clear();   // tooltip recalcule a chaque frame selon le survol

    // Manufacturer
    for(int i=0;i<NVIS && i+g_man_scroll<g_man_count; ++i){
        int gi=i+g_man_scroll;
        draw_list_row(xd+COL_MAN_X, yd+LIST_Y+i*ROWH, COL_MAN_W, g_manufs[gi].c_str(), gi==g_sel_manuf && g_search[0]==0);
    }
    draw_scrollbar(xd+COL_MAN_X+COL_MAN_W, yd+LIST_Y, g_man_count, g_man_scroll);

    // Fixture
    for(int i=0;i<NVIS && i+g_fix_scroll<g_fix_count; ++i){
        int gi=fixlist[i+g_fix_scroll];
        draw_list_row(xd+COL_FIX_X, yd+LIST_Y+i*ROWH, COL_FIX_W, g_lib[gi].model.c_str(), gi==g_sel_fix);
    }
    draw_scrollbar(xd+COL_FIX_X+COL_FIX_W, yd+LIST_Y, g_fix_count, g_fix_scroll);

    // Mode
    if(seld){
        for(int i=0;i<NVIS && i+g_mode_scroll<g_mode_count; ++i){
            int m=i+g_mode_scroll;
            char mr[128]; snprintf(mr, sizeof(mr), "%s (%d)", seld->modes[m].name.c_str(), seld->modes[m].footprint);
            draw_list_row(xd+COL_MOD_X, yd+LIST_Y+i*ROWH, COL_MOD_W, mr, m==g_sel_mode);
        }
        draw_scrollbar(xd+COL_MOD_X+COL_MOD_W, yd+LIST_Y, g_mode_count, g_mode_scroll);
    }

    // tooltip : nom complet de la ligne survolee si elle est tronquee (par-dessus les colonnes)
    if(!g_tip.empty()){
        int tw = petitchiffre.TextWidth(g_tip.c_str()) + 10;
        int tx = g_tip_x, ty = g_tip_y;
        if(tx+tw > xd+devicepatch_window_w-8) tx = xd+devicepatch_window_w-8-tw;
        if(tx < xd+4) tx = xd+4;
        Rect Tip(Vec2D(tx, ty-2), Vec2D(tw, ROWH)); Tip.SetRoundness(3);
        Tip.Draw(CouleurGrisAnthracite); Tip.DrawOutline(CouleurFader);
        petitchiffre.Print(g_tip.c_str(), tx+4, ty+12);
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

    // --- popup compte GDTF Share (option 2) ---
    if(g_show_account)
    {
        int px=xd+150, py=yd+70;
        Rect Pop(Vec2D(px,py), Vec2D(250,150)); Pop.SetRoundness(8);
        Pop.Draw(CouleurFond); Pop.DrawOutline(CouleurFader);
        Line(Vec2D(px+250-20,py+8),Vec2D(px+250-10,py+18)).Draw(CouleurLigne);   // croix de fermeture
        Line(Vec2D(px+250-10,py+8),Vec2D(px+250-20,py+18)).Draw(CouleurLigne);
        gdtf_account_draw(px+15, py+18);
    }

    return(0);
}

int do_logical_devicepatch(int xd, int yd)
{
    if(!g_scanned) devlib_scan();

    // bouton GDTF Share -> ouvre/ferme la popup compte
    if(mouse_released==0 && mouse_x>xd+devicepatch_window_w-135 && mouse_x<xd+devicepatch_window_w-15 && mouse_y>yd+18 && mouse_y<yd+40)
    {
        g_show_account = !g_show_account; mouse_released=1;
    }
    // popup ouverte -> croix de fermeture, sinon router les clics vers le panneau
    if(g_show_account)
    {
        int px=xd+150, py=yd+70;
        if(mouse_released==0 && mouse_x>px+250-24 && mouse_x<px+250-6 && mouse_y>py+4 && mouse_y<py+22){ g_show_account=false; mouse_released=1; }
        else gdtf_account_logical(px+15, py+18);
    }

    // clic dans la boite de recherche -> edition inline
    if(mouse_released==0 && mouse_x>xd+SRCH_X && mouse_x<xd+SRCH_X+SRCH_W && mouse_y>yd+SRCH_Y && mouse_y<yd+SRCH_Y+18)
    {
        wc_inline_begin(g_search, sizeof(g_search), SRCH_W-10, devlib_on_search_commit);
        mouse_released=1;
    }
    // toggle All / Local
    if(mouse_released==0 && mouse_x>xd+SRCH_X+SRCH_W+10 && mouse_x<xd+SRCH_X+SRCH_W+80 && mouse_y>yd+SRCH_Y && mouse_y<yd+SRCH_Y+18)
    {
        g_filter_local=!g_filter_local; g_sel_manuf=0; g_man_scroll=0; g_fix_scroll=0; rebuild_manufs(); mouse_released=1;
    }

    // clic sur un fabricant (offset scroll ; les 8 px de droite = ascenseur)
    for(int i=0;i<NVIS && i+g_man_scroll<(int)g_manufs.size(); ++i)
    {
        int ry = yd+LIST_Y + i*ROWH;
        if(mouse_released==0 && mouse_x>xd+COL_MAN_X-2 && mouse_x<xd+COL_MAN_X+COL_MAN_W-SBW-2 && mouse_y>ry-2 && mouse_y<ry+ROWH-2)
        {
            g_sel_manuf=i+g_man_scroll; g_search[0]=0; g_sel_fix=-1; g_sel_mode=0; g_fix_scroll=0; mouse_released=1;
        }
    }

    // clic sur une fixture (offset scroll)
    std::vector<int> fixlist; build_fixlist(fixlist);
    for(int i=0;i<NVIS && i+g_fix_scroll<(int)fixlist.size(); ++i)
    {
        int ry = yd+LIST_Y + i*ROWH;
        if(mouse_released==0 && mouse_x>xd+COL_FIX_X-2 && mouse_x<xd+COL_FIX_X+COL_FIX_W-SBW-2 && mouse_y>ry-2 && mouse_y<ry+ROWH-2)
        {
            g_sel_fix=fixlist[i+g_fix_scroll]; g_sel_mode=0; g_mode_scroll=0; autofill_fields(); mouse_released=1;
        }
    }

    // clic sur un mode (offset scroll)
    if(g_sel_fix>=0 && g_sel_fix<(int)g_lib.size())
    {
        const DevFix& d = g_lib[g_sel_fix];
        for(int m=0;m<NVIS && m+g_mode_scroll<(int)d.modes.size(); ++m)
        {
            int ry = yd+LIST_Y + m*ROWH;
            if(mouse_released==0 && mouse_x>xd+COL_MOD_X-2 && mouse_x<xd+COL_MOD_X+COL_MOD_W-SBW-2 && mouse_y>ry-2 && mouse_y<ry+ROWH-2)
            {
                g_sel_mode=m+g_mode_scroll; autofill_fields(); mouse_released=1;
            }
        }
    }

    // drag des curseurs d'ascenseur (strip droit de chaque colonne, bouton maintenu)
    {
        int ly=yd+LIST_Y, lh=NVIS*ROWH;
        if(mouse_button==1 && mouse_y>ly && mouse_y<ly+lh)
        {
            if(g_man_count>NVIS && mouse_x>xd+COL_MAN_X+COL_MAN_W-SBW-2 && mouse_x<xd+COL_MAN_X+COL_MAN_W+2) g_man_scroll=scroll_from_thumb(ly,g_man_count,mouse_y);
            if(g_fix_count>NVIS && mouse_x>xd+COL_FIX_X+COL_FIX_W-SBW-2 && mouse_x<xd+COL_FIX_X+COL_FIX_W+2) g_fix_scroll=scroll_from_thumb(ly,g_fix_count,mouse_y);
            if(g_mode_count>NVIS && mouse_x>xd+COL_MOD_X+COL_MOD_W-SBW-2 && mouse_x<xd+COL_MOD_X+COL_MOD_W+2) g_mode_scroll=scroll_from_thumb(ly,g_mode_count,mouse_y);
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
        // fixture en ligne : telecharger + extraire + mettre en cache avant de patcher (transparent)
        if(!ensure_local(g_lib[g_sel_fix])){ mouse_released=1; return(0); }
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
