/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
       WWWWWWWWWWWWWWW           |
     WWWWWWWWWWWWWWWWWWW         | This file is part of White Cat.
    WWWWWWWWWWWWWWWWWCWWWW       |
   WWWWWWWWWWWWWWWWW tWWWWW      | White Cat is free software: you can redistribute it and/or modify
  WWWW   WWWWWWWWWW  tWWWWWW     | it under the terms of the GNU General Public License as published by
 WWWWWt              tWWWWWWa    | the Free Software Foundation, either version 2 of the License, or
 WWWWWW               WWWWWWW    | (at your option) any later version.
WWWWWWWW              WWWWWWW    |
WWWWWWWW               WWWWWWW   | White Cat is distributed in the hope that it will be useful,
WWWWWWW               WWWWWWWW   | but WITHOUT ANY WARRANTY; without even the implied warranty of
WWWWWWW      CWWW    W WWWWWWW   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
WWWWWWW            aW  WWWWWWW   | GNU General Public License for more details.
WWWWWWWW           C  WWWWWWWW   |
 WWWWWWWW            CWWWWWWW    | You should have received a copy of the GNU General Public License
 WWWWWWWWW          WWWWWWWWW    | along with White Cat.  If not, see <http://www.gnu.org/licenses/>.
  WWWWWWWWWWC    CWWWWWWWWWW     |
   WWWWWWWWWWWWWWWWWWWWWWWW      |
    WWWWWWWWWWWWWWWWWWWWWW       |
      WWWWWWWWWWWWWWWWWWa        |
        WWWWWWWWWWWWWWW          |
           WWWWWWWWt             |
                                 |
---------------------------------------------------------------------------------------------------------------*/

#include "wc_tus.h"

/**

* \file saves_menu_8.cpp
* \brief {GUI window of save and load menu}
* \author Christoph Guillermet
* \version {0.8.6.3}
* \date {12/02/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   GUI fonctions pour la fenêtre save menu
*
*   GUI window of save and load menu
*
**/

int scan_savesfolder()
{
    int nbre_de_shows=0;
#ifdef _WIN32
    WIN32_FIND_DATA f;
    HANDLE hFind;
    char search_saves[512];
    sprintf(search_saves,"%s\\saves\\*.*",mondirectory);
    hFind = FindFirstFile(search_saves, &f);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            int f_name_len = strlen(f.cFileName);
            bool isDir=true;
            for(unsigned int a=0;a<(unsigned int)f_name_len;a++)
            {
                if(f.cFileName[a]=='.') { isDir=false; break; }
            }
            if(isDir)
            {
                sprintf(list_save_files[nbre_de_shows],f.cFileName);
                nbre_de_shows++;
            }
        }
        while(FindNextFile(hFind, &f));
        FindClose(hFind);
    }
#else
    char search_saves[512];
    snprintf(search_saves, sizeof(search_saves), "%s/saves", mondirectory);
    DIR* dir = opendir(search_saves);
    if(dir)
    {
        struct dirent* entry;
        while((entry = readdir(dir)) != NULL)
        {
            if(entry->d_name[0] == '.') continue;
            if(entry->d_type != DT_DIR) continue;
            snprintf(list_save_files[nbre_de_shows], sizeof(list_save_files[0]), "%s", entry->d_name);
            nbre_de_shows++;
        }
        closedir(dir);
    }
#endif
    nbre_save_files = nbre_de_shows;
    sprintf(rep,"%s" WC_DIRSEP, mondirectory);
    chdir(rep);
    return(0);
}

//sab 02/03/2014 int scan_importfolder(char *subdir)
void scan_importfolder(const char* subdir)
{
    for(int i=0;i<127;i++) strcpy(list_import_files[i],"");
    int nrbe_de_fichiers=0;
#ifdef _WIN32
    WIN32_FIND_DATA f;
    HANDLE hFind;
    char search_import[512];
    sprintf(search_import,"%s\\import_export\\%s*.*",mondirectory,subdir);
    hFind = FindFirstFile(search_import, &f);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(nrbe_de_fichiers<127)
            {
                sprintf(list_import_files[nrbe_de_fichiers],f.cFileName);
                nrbe_de_fichiers++;
            }
        }
        while(FindNextFile(hFind, &f));
        FindClose(hFind);
    }
#else
    char search_import[512];
    snprintf(search_import, sizeof(search_import), "%s/import_export", mondirectory);
    DIR* dir = opendir(search_import);
    if(dir)
    {
        struct dirent* entry;
        while((entry = readdir(dir)) != NULL && nrbe_de_fichiers < 127)
        {
            if(entry->d_name[0] == '.') continue;
            if(subdir && subdir[0] && strncmp(entry->d_name, subdir, strlen(subdir)) != 0) continue;
            snprintf(list_import_files[nrbe_de_fichiers], sizeof(list_import_files[0]), "%s", entry->d_name);
            nrbe_de_fichiers++;
        }
        closedir(dir);
    }
#endif
    nbre_import_files = nrbe_de_fichiers;
    sprintf(rep,"%s" WC_DIRSEP, mondirectory);
    chdir(rep);
}

//////////////////ASCENSEUR FENETRE SAVE (remplace ronds +/-)/////////////////////
// Rendu d'un ascenseur vertical : fond + flèche haut + flèche bas + thumb.
static void save_scrollbar_draw(int bx, int by, int bar_h, int line_pos, int total, int visible, bool dragging)
{
    if (total <= visible) return; // rien à défiler → pas d'ascenseur
    const int bar_w = 14;
    int max_scroll = total - visible; if (max_scroll < 0) max_scroll = 0;
    Rect Bar(Vec2D(bx, by), Vec2D(bar_w, bar_h));
    Bar.SetRoundness(3);
    Bar.Draw(Rgba(0, 0, 0));
    // flèche haut
    Rect ArrowUp(Vec2D(bx + 1, by), Vec2D(bar_w - 2, bar_w));
    ArrowUp.SetRoundness(2);
    ArrowUp.Draw(line_pos > 0 ? CouleurGrisAnthracite : CouleurFond);
    petitpetitchiffre.Print("^", bx + 4, by + 10);
    // flèche bas
    Rect ArrowDown(Vec2D(bx + 1, by + bar_h - bar_w), Vec2D(bar_w - 2, bar_w));
    ArrowDown.SetRoundness(2);
    ArrowDown.Draw(line_pos < max_scroll ? CouleurGrisAnthracite : CouleurFond);
    petitpetitchiffre.Print("v", bx + 4, by + bar_h - bar_w + 10);
    // thumb
    int track_h = bar_h - 2 * bar_w;
    int thumb_h = (total > 0) ? (track_h * visible / total) : track_h;
    if (thumb_h < 10) thumb_h = 10;
    if (thumb_h > track_h) thumb_h = track_h;
    int thumb_y = by + bar_w + (max_scroll > 0 ? line_pos * (track_h - thumb_h) / max_scroll : 0);
    if (thumb_y < by + bar_w) thumb_y = by + bar_w;
    if (thumb_y + thumb_h > by + bar_h - bar_w) thumb_y = by + bar_h - bar_w - thumb_h;
    Rect Thumb(Vec2D(bx + 2, thumb_y), Vec2D(bar_w - 4, thumb_h));
    Thumb.SetRoundness(3);
    Thumb.Draw(dragging ? CouleurGrisAnthracite : CouleurGrisAnthracite.WithAlpha(0.7));
}

// Logique clic/drag d'un ascenseur. line_pos/dragging/start_* passés par pointeur.
static void save_scrollbar_logic(int bx, int by, int bar_h, int* line_pos, int total, int visible,
                                 bool* dragging, int* drag_start_y, int* drag_start_scroll)
{
    if (total <= visible) { *line_pos = 0; return; } // rien à défiler
    const int bar_w = 14;
    int max_scroll = total - visible; if (max_scroll < 0) max_scroll = 0;
    int track_h = bar_h - 2 * bar_w;
    int thumb_h = (total > 0) ? (track_h * visible / total) : track_h;
    if (thumb_h < 10) thumb_h = 10;
    if (thumb_h > track_h) thumb_h = track_h;

    if (*dragging) {
        int delta = mouse_y - *drag_start_y;
        int travel = track_h - thumb_h;
        if (travel > 0) {
            int new_scroll = *drag_start_scroll + delta * max_scroll / travel;
            if (new_scroll < 0) new_scroll = 0;
            if (new_scroll > max_scroll) new_scroll = max_scroll;
            *line_pos = new_scroll;
        }
    } else if (window_focus_id == W_SAVE && mouse_button == 1 && mouse_released == 0) {
        int thumb_y = by + bar_w + (max_scroll > 0 ? *line_pos * (track_h - thumb_h) / max_scroll : 0);
        if (thumb_y < by + bar_w) thumb_y = by + bar_w;
        if (thumb_y + thumb_h > by + bar_h - bar_w) thumb_y = by + bar_h - bar_w - thumb_h;
        // flèche haut
        if (mouse_x > bx && mouse_x < bx + bar_w && mouse_y > by && mouse_y < by + bar_w) {
            if (*line_pos > 0) (*line_pos)--;
            mouse_released = 1;
        }
        // flèche bas
        else if (mouse_x > bx && mouse_x < bx + bar_w && mouse_y > by + bar_h - bar_w && mouse_y < by + bar_h) {
            if (*line_pos < max_scroll) (*line_pos)++;
            mouse_released = 1;
        }
        // début drag thumb
        else if (mouse_x > bx && mouse_x < bx + bar_w && mouse_y > thumb_y && mouse_y < thumb_y + thumb_h) {
            *dragging = 1;
            *drag_start_y = mouse_y;
            *drag_start_scroll = *line_pos;
        }
    }
}

// Molette souris sur la zone liste : fait défiler line_pos. last_z = position molette précédente.
static void save_scrollbar_wheel(int area_x, int area_y, int area_w, int area_h,
                                 int* line_pos, int total, int visible, int* last_z)
{
    int max_scroll = total - visible; if (max_scroll < 0) max_scroll = 0;
    if (window_focus_id == W_SAVE &&
        mouse_x >= area_x && mouse_x <= area_x + area_w &&
        mouse_y >= area_y && mouse_y <= area_y + area_h)
    {
        int wdelta = mouse_z - *last_z;
        if (wdelta != 0) {
            *line_pos -= wdelta;
            if (*line_pos < 0) *line_pos = 0;
            if (*line_pos > max_scroll) *line_pos = max_scroll;
        }
    }
    *last_z = mouse_z;
}
static int save_import_last_scroll_z = 0;
static int save_binary_last_scroll_z = 0;


int choose_personnal_preset_binary_save_load(int xs,int ys)
{
petitchiffre.Print("Personnal Presets:",xs-15,ys-15);
for(int i=0;i<4;i++)
{
Rect Carre(Vec2D(xs+(20*i),ys),Vec2D(10,10));
switch(i)
{
case 0:
Carre.Draw(CouleurBlind);
break;
case 1:
Carre.Draw(CouleurBleuProcedure);
break;
case 2:
Carre.Draw(CouleurGreen);
break;
case 3:
Carre.Draw(CouleurYellowFgroup);
break;
}
Carre.DrawOutline(CouleurLigne.WithAlpha(0.5));

}
 return(0);
}


int do_logical_choose_personnal_preset_binary_save_load(int xs,int ys)
{
for(int i=0;i<4;i++)
{
if(window_focus_id==W_SAVE && mouse_button==1 && mouse_released==0 && mouse_x>=xs+(20*i) && mouse_x<=xs+(20*i)+10 && mouse_y>=ys && mouse_y<=ys+10)
{
mouse_released=1;

if(index_do_dock==1)
{
for(int p=0;p<80;p++)
{
preset_specify_who_to_save_load[p][i]=specify_who_to_save_load[p];
}
index_do_dock=0;
sprintf(string_Last_Order,"Recorded Personnal Save-Load Preset %d",i+1);
}

else if(index_main_clear==1)
{
for(int p=0;p<80;p++)
{
preset_specify_who_to_save_load[p][i]=0;
}
index_main_clear=0;
sprintf(string_Last_Order,"Cleared Personnal Save-Load Preset %d",i+1);
}

else
{
for(int p=0;p<80;p++)
{
specify_who_to_save_load[p]=preset_specify_who_to_save_load[p][i];
}
index_save_global_is=0;
sprintf(string_Last_Order,"Called Personnal Save-Load Preset %d",i+1);
}

}
}
 return(0);
}


int check_import_type()
{

if(index_is_saving==0)
{
if(strcmp(importfile_name,"ascii")==0){scan_importfolder("ascii");}
else if(strcmp(importfile_name,"pdf")==0){scan_importfolder("pdf");}
else if(strcmp(importfile_name,"plot")==0  ){scan_importfolder("plot");}
else if(strcmp(importfile_name,"schwz")==0){scan_importfolder("schwz");index_export_choice=1;isSchwz=1;}//christoph 16/06/14 debug perte fonctionnalité import whitecat
else if(strcmp(importfile_name,"..")==0){scan_importfolder("");line_import=0;}
else
{
//christoph 19/12/14
int f_name_len = strlen(importfile_name);
            // check if it's a dir or a file
          for(unsigned int a=0;a<f_name_len;a++)
          {
           if( importfile_name[a]=='.')
           {
                if((importfile_name[a+1]=='a' &&  importfile_name[a+2]=='s' &&  importfile_name[a+3]=='c')
                || (importfile_name[a+1]=='A' &&  importfile_name[a+2]=='S' &&  importfile_name[a+3]=='C')
                || (importfile_name[a+1]=='t' &&  importfile_name[a+2]=='x' &&  importfile_name[a+3]=='t')
                || (importfile_name[a+1]=='T' &&  importfile_name[a+2]=='X' &&  importfile_name[a+3]=='T')
                )
                {
                    isSchwz=0; isPdf=0; isAlq=0;
                    isASCII=1;index_export_choice=0;
                    sprintf(string_typeexport_view,">> ascii file");
                    scan_importfolder("ascii");
                    break;
                }

                if((importfile_name[a+1]=='a' &&  importfile_name[a+2]=='l' &&  importfile_name[a+3]=='q')
                || (importfile_name[a+1]=='A' &&  importfile_name[a+2]=='L' &&  importfile_name[a+3]=='Q'))
                {
                    isSchwz=0;  isPdf=0;
                    isASCII=0; isAlq=1;index_export_choice=2;
                    sprintf(string_typeexport_view,">> ascii alq file");
                    scan_importfolder("ascii");
                    break;
                }

                if((importfile_name[a+1]=='p' &&  importfile_name[a+2]=='d' &&  importfile_name[a+3]=='f')
                ||(importfile_name[a+1]=='P' &&  importfile_name[a+2]=='D' &&  importfile_name[a+3]=='F'))
                {   isSchwz=0;  isASCII=0; isAlq=0;
                    isPdf=1;index_export_choice=3;
                    sprintf(string_typeexport_view,">> PDF file");
                    scan_importfolder("pdf");
                    break;
                }


                else
                {
                isASCII=0; isPdf=0; isAlq=0; isSchwz=1;index_export_choice=1;
                sprintf(string_typeexport_view,">> schwartzpeter");
                scan_importfolder("schwz");
                }

           }
    }
}
}


return(0);
}



//sab 02/03/2014 int deroule_repertoire_export_import(int xrep, int yrep, char name_of_rep[25])
void deroule_repertoire_export_import(int xrep, int yrep, const std::string label)
{
//christoph 19/12/14 substr(0,25)>substr(0,24);
//const std::string name_of_rep = label.substr(0,24);
//////////////////////LISTE DOSSIERS ETC///////////////////////////////////////
petitchiffre.Print("Content of import_export folder:",xrep+10,yrep+170);
Rect BackDeroule(Vec2D(xrep,yrep+155),Vec2D(245,185));
BackDeroule.SetRoundness(15);
BackDeroule.Draw(CouleurConfig.WithAlpha(0.7));

for (int y=0;y<8;y++)
{
Rect OverFile(Vec2D(xrep+5,(yrep+185+(y*20)-10)),Vec2D(200,20));
OverFile.SetRoundness(7.5);

//affichage qui est selectionné
if(strcmp(importfile_name,list_import_files[importfile_selected])==0 && (importfile_selected==(y+line_import)))
{OverFile.Draw(CouleurFond.WithAlpha(0.5));}


if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+155 && mouse_y>(yrep+175+(y*20)) && mouse_y<(yrep+190+(y*20)))
{
OverFile.DrawOutline(CouleurLigne);
}
petitpetitchiffre.Print(list_import_files[line_import+y],xrep+10,yrep+188+(y*20));

 //fin des 8 lignes
}



//nom fichier save laod
Rect FrameSelected(Vec2D(xrep+5,yrep+347),Vec2D(240,30));
FrameSelected.SetRoundness(7.5);
FrameSelected.Draw(CouleurConfig);



//selection depuis chaine de caracteres pour export plot


if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+5+200 && mouse_y>yrep+347 && mouse_y<yrep+367)
{
FrameSelected.DrawOutline(CouleurLigne);
}




FrameSelected.SetLineWidth(epaisseur_ligne_fader);
FrameSelected.DrawOutline(CouleurLigne.WithAlpha(alpha_blinker));
petitchiffre.Print(importfile_name,xrep+10,yrep+365);
petitpetitchiffre.Print(string_typeexport_view,xrep+150,yrep+375);

petitpetitchiffre.Print("Name must have extension: ",xrep+250,yrep+170 );
petitchiffre.Print("Ascii: .asc .txt or .alq",xrep+250,yrep+185 );
petitchiffre.Print("PDF: .pdf",xrep+250,yrep+200 );
petitpetitchiffre.Print("Schwarzpeter: no extension",xrep+250,yrep+215 );

//////////////////ASCENSEUR IMPORT/////////////////////
save_scrollbar_draw(xrep+228, yrep+176, 162, line_import, nbre_import_files, 8, save_import_scroll_dragging);
save_scrollbar_wheel(xrep, yrep+155, 245, 185, &line_import, nbre_import_files, 8, &save_import_last_scroll_z);


/////////////////////////////SAVE LOAD && CONDITIONS////////////////////////////////
switch (index_export_choice)
{
case 0://ascii
enable_export=1;
enable_import=1;
break;
case 1://schwz
enable_export=0;
enable_import=1;
break;
case 2://alq
enable_export=0;
enable_import=1;
break;
case 3://pdf
enable_export=1;
enable_import=0;
break;

}
////////////////////////////////////////////////////////////////////////////////
if(enable_export==1)
{
Rect SaveExport(Vec2D(xrep+40,yrep+390),Vec2D(70,30));
SaveExport.SetRoundness(7.5);
if(window_focus_id==W_SAVE && mouse_x>xrep+40 && mouse_x<xrep+110 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
SaveExport.Draw(CouleurSurvol);
}
petitchiffre.Print("SAVE",xrep+55,yrep+410);
SaveExport.DrawOutline(CouleurLigne);
}
////////////////////////////////////////////////////////////////////////////////
if(enable_import==1)
{
Rect LoadExport(Vec2D(xrep+140,yrep+390),Vec2D(70,30));
LoadExport.SetRoundness(7.5);
if(window_focus_id==W_SAVE && mouse_x>xrep+140 && mouse_x<xrep+210 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
LoadExport.Draw(CouleurSurvol);
}
petitchiffre.Print("LOAD",xrep+155,yrep+410);
LoadExport.DrawOutline(CouleurLigne);
}
//sab 02/03/2014 return(0);
}


//sab 02/03/2014 -> void -> std::string
void do_logical_deroule_repertoire_export_import(int xrep, int yrep, const std::string label)
{
//christoph 19/12/14 substr(0,25)>substr(0,24);
const std::string name_of_rep 		= label.substr (0,24);
//////////////////////LISTE DOSSIERS ETC///////////////////////////////////////

for (int y=0;y<8;y++)
{


if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+155 && mouse_y>(yrep+175+(y*20)) && mouse_y<(yrep+190+(y*20)))
{

if(mouse_button==1 && mouse_released==0)
{
importfile_selected=(y+line_import);
if(y+line_import<127)
{sprintf(importfile_name,list_import_files[importfile_selected]);  }
check_import_type();
mouse_released=1;
}
}

 //fin des 8 lignes
}



//selection depuis chaine de caracteres pour export plot


if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+205 && mouse_y>yrep+347 && mouse_y<yrep+367)
{

if(mouse_button==1 && mouse_released==0 && index_type==1 )
{
for (int tt=0;tt<24;tt++)
{
importfile_name[tt]=numeric[tt];
}
reset_numeric_entry();
index_type=0;
check_import_type();
mouse_released=1;
}
}




//////////////////ASCENSEUR IMPORT/////////////////////
save_scrollbar_logic(xrep+228, yrep+176, 162, &line_import, nbre_import_files, 8,
                     &save_import_scroll_dragging, &save_import_scroll_drag_start_y, &save_import_scroll_drag_start_scroll);



/////////////////////////////SAVE LOAD && CONDITIONS////////////////////////////////
/*switch (index_export_choice)
{
case 0://ascii
enable_export=1;
enable_import=1;
break;
case 1://schwz
enable_export=0;
enable_import=1;
break;
case 2://alq
enable_export=0;
enable_import=1;
break;
case 3://pdf
enable_export=1;
enable_import=0;
break;

}*/ //mis dans  affichage
////////////////////////////////////////////////////////////////////////////////
if(enable_export==1)
{
if(window_focus_id==W_SAVE && mouse_x>xrep+40 && mouse_x<xrep+110 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
if(mouse_button==1 && mouse_released==0)
{
index_do_export=1;
index_ask_confirm=1;
mouse_released=1;
}
}
}
////////////////////////////////////////////////////////////////////////////////
if(enable_import==1)
{
if(window_focus_id==W_SAVE && mouse_x>xrep+140 && mouse_x<xrep+210 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
if(mouse_button==1 && mouse_released==0)
{
index_do_import=1;
index_ask_confirm=1;
mouse_released=1;
}
}
}
//sab 02/03/2014 return(0);
}
/*

int deroule_repertoire_export_import(int xrep, int yrep, char name_of_rep[25])
{

//////////////////////LISTE DOSSIERS ETC///////////////////////////////////////
petitchiffre.Print("Content of import_export folder:",xrep+10,yrep+170);
Rect BackDeroule(Vec2D(xrep,yrep+155),Vec2D(245,185));
BackDeroule.SetRoundness(15);
BackDeroule.Draw(CouleurConfig.WithAlpha(0.7));

for (int y=0;y<8;y++)
{
Rect OverFile(Vec2D(xrep+5,(yrep+185+(y*20)-10)),Vec2D(200,20));
OverFile.SetRoundness(7.5);

//affichage qui est selectionné
if(strcmp(importfile_name,list_import_files[importfile_selected])==0 && (importfile_selected==(y+line_import)))
{OverFile.Draw(CouleurFond.WithAlpha(0.5));}


if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+155 && mouse_y>(yrep+175+(y*20)) && mouse_y<(yrep+190+(y*20)))
{
OverFile.DrawOutline(CouleurLigne);
}
petitpetitchiffre.Print(list_import_files[line_import+y],xrep+10,yrep+188+(y*20));

 //fin des 8 lignes
}



//nom fichier save laod
Rect FrameSelected(Vec2D(xrep+5,yrep+347),Vec2D(240,30));
FrameSelected.SetRoundness(7.5);
FrameSelected.Draw(CouleurConfig);



//selection depuis chaine de caracteres pour export plot


if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+5+200 && mouse_y>yrep+347 && mouse_y<yrep+367)
{
FrameSelected.DrawOutline(CouleurLigne);
}




FrameSelected.SetLineWidth(epaisseur_ligne_fader);
FrameSelected.DrawOutline(CouleurLigne.WithAlpha(alpha_blinker));
petitchiffre.Print(importfile_name,xrep+10,yrep+365);
petitpetitchiffre.Print(string_typeexport_view,xrep+150,yrep+375);

petitpetitchiffre.Print("Name must have extension: ",xrep+250,yrep+170 );
petitchiffre.Print("Ascii: .asc .txt or .alq",xrep+250,yrep+185 );
petitchiffre.Print("PDF: .pdf",xrep+250,yrep+200 );
petitpetitchiffre.Print("Schwarzpeter: no extension",xrep+250,yrep+215 );

//////////////////ASCENSEUR IMPORT/////////////////////
save_scrollbar_draw(xrep+228, yrep+176, 162, line_import, nbre_import_files, 8, save_import_scroll_dragging);
save_scrollbar_wheel(xrep, yrep+155, 245, 185, &line_import, nbre_import_files, 8, &save_import_last_scroll_z);


/////////////////////////////SAVE LOAD && CONDITIONS////////////////////////////////
switch (index_export_choice)
{
case 0://ascii
enable_export=1;
enable_import=1;
break;
case 1://schwz
enable_export=0;
enable_import=1;
break;
case 2://alq
enable_export=0;
enable_import=1;
break;
case 3://pdf
enable_export=1;
enable_import=0;
break;

}
////////////////////////////////////////////////////////////////////////////////
if(enable_export==1)
{
Rect SaveExport(Vec2D(xrep+40,yrep+390),Vec2D(70,30));
SaveExport.SetRoundness(7.5);
if(window_focus_id==W_SAVE && mouse_x>xrep+40 && mouse_x<xrep+110 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
SaveExport.Draw(CouleurSurvol);
}
petitchiffre.Print("SAVE",xrep+55,yrep+410);
SaveExport.DrawOutline(CouleurLigne);
}
////////////////////////////////////////////////////////////////////////////////
if(enable_import==1)
{
Rect LoadExport(Vec2D(xrep+140,yrep+390),Vec2D(70,30));
LoadExport.SetRoundness(7.5);
if(window_focus_id==W_SAVE && mouse_x>xrep+140 && mouse_x<xrep+210 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
LoadExport.Draw(CouleurSurvol);
}
petitchiffre.Print("LOAD",xrep+155,yrep+410);
LoadExport.DrawOutline(CouleurLigne);
}
return(0);
}


int do_logical_deroule_repertoire_export_import(int xrep, int yrep, char name_of_rep[25])
{

//////////////////////LISTE DOSSIERS ETC///////////////////////////////////////

for (int y=0;y<8;y++)
{


if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+155 && mouse_y>(yrep+175+(y*20)) && mouse_y<(yrep+190+(y*20)))
{

if(mouse_button==1 && mouse_released==0)
{
importfile_selected=(y+line_import);
if(y+line_import<127)
{sprintf(importfile_name,list_import_files[importfile_selected]);  }
check_import_type();
mouse_released=1;
}
}

 //fin des 8 lignes
}



//selection depuis chaine de caracteres pour export plot


if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+205 && mouse_y>yrep+347 && mouse_y<yrep+367)
{

if(mouse_button==1 && mouse_released==0 && index_type==1 )
{
for (int tt=0;tt<24;tt++)
{
importfile_name[tt]=numeric[tt];
}
reset_numeric_entry();
index_type=0;
check_import_type();
mouse_released=1;
}
}




//////////////////ASCENSEUR IMPORT/////////////////////
save_scrollbar_logic(xrep+228, yrep+176, 162, &line_import, nbre_import_files, 8,
                     &save_import_scroll_dragging, &save_import_scroll_drag_start_y, &save_import_scroll_drag_start_scroll);

////////////////////////////////////////////////////////////////////////////////
if(enable_export==1)
{
if(window_focus_id==W_SAVE && mouse_x>xrep+40 && mouse_x<xrep+110 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
if(mouse_button==1 && mouse_released==0)
{
index_do_export=1;
index_ask_confirm=1;
mouse_released=1;
}
}
}
////////////////////////////////////////////////////////////////////////////////
if(enable_import==1)
{
if(window_focus_id==W_SAVE && mouse_x>xrep+140 && mouse_x<xrep+210 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
if(mouse_button==1 && mouse_released==0)
{
index_do_import=1;
index_ask_confirm=1;
mouse_released=1;
}
}
}
return(0);
}

*/

////////////////////////////////////////////////////////////////////////////////
////////////////////////BINARY SAVING///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//sab 02/03/2014 int deroule_repertoire_classical_save(int xrep, int yrep, char name_of_rep[25])
void deroule_repertoire_classical_save(int xrep, int yrep, const std::string label)
{
//christoph 19/12/14 substr(0,25)>substr(0,24);
const std::string name_of_rep = label.substr(0,24);
//////////////////////LISTE DOSSIERS ETC///////////////////////////////////////
petitchiffre.Print("Content of SAVES folder:",xrep+10,yrep+170);
Rect BackDeroule(Vec2D(xrep,yrep+155),Vec2D(245,185));
BackDeroule.SetRoundness(15);
BackDeroule.Draw(CouleurConfig.WithAlpha(0.7));

for (int y=0;y<8;y++)
{
Rect OverFile(Vec2D(xrep+5,(yrep+185+(y*20)-10)),Vec2D(200,20));
OverFile.SetRoundness(7.5);
//affichage quiest selectionné
if(strcmp(savefile_name,list_save_files[savefile_selected])==0 && (savefile_selected==(y+line_save)))
{OverFile.Draw(CouleurFond.WithAlpha(0.5));}

if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+155 && mouse_y>(yrep+175+(y*20)) && mouse_y<(yrep+190+(y*20)))
{
OverFile.DrawOutline(CouleurLigne);
}

petitpetitchiffre.Print(list_save_files[line_save+y],xrep+10,yrep+188+(y*20));
}
//nom du spectacle
Rect FrameSelected(Vec2D(xrep+5,yrep+347),Vec2D(240,30));
FrameSelected.SetRoundness(7.5);
FrameSelected.Draw(CouleurConfig);
if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+245 && mouse_y>yrep+347 && mouse_y<yrep+377)
{
FrameSelected.DrawOutline(CouleurLigne);
}
FrameSelected.SetLineWidth(epaisseur_ligne_fader);
FrameSelected.DrawOutline(CouleurLigne.WithAlpha(alpha_blinker));
petitchiffre.Print(savefile_name,xrep+10,yrep+365);
//////////////////ASCENSEUR SAVE/////////////////////
save_scrollbar_draw(xrep+228, yrep+176, 162, line_save, nbre_save_files, 8, save_binary_scroll_dragging);
save_scrollbar_wheel(xrep, yrep+155, 245, 185, &line_save, nbre_save_files, 8, &save_binary_last_scroll_z);
///////////////////////////////////////////////////////////////////////////////
Rect SaveB(Vec2D(xrep+40,yrep+390),Vec2D(70,30));
SaveB.SetRoundness(7.5);
if(window_focus_id==W_SAVE && mouse_x>xrep+40 && mouse_x<xrep+110 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
SaveB.Draw(CouleurSurvol);
}
petitchiffre.Print("SAVE",xrep+55,yrep+410);
SaveB.DrawOutline(CouleurLigne);

////////////////////////////////////////////////////////////////////////////////

Rect LoadB(Vec2D(xrep+140,yrep+390),Vec2D(70,30));
LoadB.SetRoundness(7.5);
if(window_focus_id==W_SAVE && mouse_x>xrep+140 && mouse_x<xrep+210 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
LoadB.Draw(CouleurSurvol);
}
petitchiffre.Print("LOAD",xrep+155,yrep+410);
LoadB.DrawOutline(CouleurLigne);

////////////////////////////////////////////////////////////////////////////////

Rect ResetB(Vec2D(xrep+280,yrep+390),Vec2D(70,30));
ResetB.SetRoundness(7.5);

if(window_focus_id==W_SAVE && mouse_x>xrep+280 && mouse_x<xrep+350 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
ResetB.Draw(CouleurSurvol);
}
petitchiffre.Print("RESET",xrep+295,yrep+410);
ResetB.DrawOutline(CouleurLigne);


//sab 02/03/2014 return(0);
}


//sab 02/03/2014 int do_logical_deroule_repertoire_classical_save(int xrep, int yrep, char name_of_rep[25])
void do_logical_deroule_repertoire_classical_save(int xrep, int yrep, const std::string label)
{
//christoph 19/12/14 substr(0,25)>substr(0,24);
const std::string name_of_rep = label.substr(0,24);

for (int y=0;y<8;y++)
{


if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+155 && mouse_y>(yrep+175+(y*20)) && mouse_y<(yrep+190+(y*20)))
{

if(mouse_button==1 && mouse_released==0)
{
savefile_selected=(y+line_save);
sprintf(savefile_name,list_save_files[savefile_selected]);
mouse_released=1;
}
}

}
//nom du spectacle

if(window_focus_id==W_SAVE && mouse_x>xrep+5 && mouse_x<xrep+245 && mouse_y>yrep+347 && mouse_y<yrep+377)
{

if(mouse_button==1 && mouse_released==0 && index_type==1)
{

for (int tt=0;tt<24;tt++)
{
savefile_name[tt]=numeric[tt];
}
reset_numeric_entry();
index_type=0;
mouse_released=1;
}
}

//////////////////ASCENSEUR SAVE/////////////////////
save_scrollbar_logic(xrep+228, yrep+176, 162, &line_save, nbre_save_files, 8,
                     &save_binary_scroll_dragging, &save_binary_scroll_drag_start_y, &save_binary_scroll_drag_start_scroll);
///////////////////////////////////////////////////////////////////////////////
//save button
if(window_focus_id==W_SAVE && mouse_x>xrep+40 && mouse_x<xrep+110 && mouse_y>yrep+390 && mouse_y<yrep+420)
{

if(mouse_button==1 && mouse_released==0)
{

if(strlen(savefile_name)==0){sprintf(savefile_name,"unnamed");}
index_do_saveshow=1;
index_ask_confirm=1;
mouse_released=1;
}
}

////////////////////////////////////////////////////////////////////////////////
//load button
if(window_focus_id==W_SAVE && mouse_x>xrep+140 && mouse_x<xrep+210 && mouse_y>yrep+390 && mouse_y<yrep+420)
{
if(mouse_button==1 && mouse_released==0)
{

if(strcmp(savefile_name,"")!=0)
{
index_do_loadshow=1;
index_ask_confirm=1;
}
mouse_released=1;
}
}

////////////////////////////////////////////////////////////////////////////////
//reset button

if(window_focus_id==W_SAVE && mouse_x>xrep+280 && mouse_x<xrep+350 && mouse_y>yrep+390 && mouse_y<yrep+420)
{

if(mouse_button==1 && mouse_released==0)
{
index_do_resetshow=1;
index_ask_confirm=1;
mouse_released=1;
}
}
//sab 02/03/2014 return(0);
}


// Liste des éléments sauvegardables (label + index réel dans le tableau de flags).
struct WcSaveItem { const char* label; int idx; };

// PDF : 16 éléments réels (MOVER, iCAT PRESETS et slots "-" retirés).
static const WcSaveItem g_pdf_col1[] = {
  {"MEMORIES",0},{"CHANNEL VIEW",1},{"CHANNELS from Plot and List",2},{"PATCH: per CHANNELS view",3},
  {"PATCH: per DIMMERS view",4},{"BANGER",5},{"AUDIO LIST OF FILES",6},{"FADERS",7},
  {"CHASERS",8},{"TRICHROMY",9},{"VIDEOTRACKING",10},{"GRIDPLAYERS",11}
};
static const WcSaveItem g_pdf_col2[] = {
  {"ECHO",13},{"DRAW",14},{"ARDUINO",21},{"MIDI AFFECTATIONS",22}
};
static const int g_pdf_col1_n = 12;
static const int g_pdf_col2_n = 4;

int selecteur_PDF_save_solo_global(int xrep, int yrep)
{
// colonne 1
for(int u=0;u<g_pdf_col1_n;u++)
{
Rect SavePin(Vec2D(xrep,yrep+(u*15)),Vec2D(10,10));
if(specify_who_to_save_PDF[g_pdf_col1[u].idx]==1){SavePin.Draw(CouleurFader); }
petitpetitchiffre.Print(g_pdf_col1[u].label,xrep+20,yrep+(u*15)+10);
SavePin.DrawOutline(CouleurLigne);
}
// colonne 2
for(int u=0;u<g_pdf_col2_n;u++)
{
Rect SavePin(Vec2D(xrep+190,yrep+(u*15)),Vec2D(10,10));
if(specify_who_to_save_PDF[g_pdf_col2[u].idx]==1){SavePin.Draw(CouleurFader); }
petitpetitchiffre.Print(g_pdf_col2[u].label,xrep+210,yrep+(u*15)+10);
SavePin.DrawOutline(CouleurLigne);
}

 return(0);
}



int do_logical_selecteur_PDF_save_solo_global(int xrep, int yrep)
{
// colonne 1
for(int u=0;u<g_pdf_col1_n;u++)
{
if(window_focus_id==W_SAVE && mouse_x>xrep && mouse_x<xrep+180 && mouse_y>yrep+(u*15) && mouse_y<yrep+(u*15)+15)
{
if(mouse_button==1 && mouse_released==0)
{
specify_who_to_save_PDF[g_pdf_col1[u].idx]=toggle(specify_who_to_save_PDF[g_pdf_col1[u].idx]);
mouse_released=1;
}
}
}
// colonne 2
for(int u=0;u<g_pdf_col2_n;u++)
{
if(window_focus_id==W_SAVE && mouse_x>xrep+190 && mouse_x<xrep+340 && mouse_y>yrep+(u*15) && mouse_y<yrep+(u*15)+15)
{
if(mouse_button==1 && mouse_released==0)
{
specify_who_to_save_PDF[g_pdf_col2[u].idx]=toggle(specify_who_to_save_PDF[g_pdf_col2[u].idx]);
mouse_released=1;
}
}
}

 return(0);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// BINARY : 33 éléments en 3 colonnes de 11 (iCAT/MOVER/slot vide retirés, sans trou).
static const WcSaveItem g_bin_col1[] = {
  {"MEMORIES",0},{"Mem.Times",1},{"Mem.Links",2},{"Mem.Bangers",3},{"Mem.Man.Ratio",4},
  {"Ch.List&Views",5},{"Ch.DirectCH",6},{"Ch.Freeze/Excl.",7},{"Patch.Circuits",8},
  {"Patch.LTP/HTP",9},{"Patch.Curves",10}
};
static const WcSaveItem g_bin_col2[] = {
  {"BANGER",11},{"FADERS.Content",12},{"Faders.LFO/Times",13},{"Faders.Curves",14},
  {"MINIF.Prsts/Lcks",15},{"CHASERS",16},{"MIDI.Affectation",17},{"Midi.Cheat",18},
  {"TRICHROMY",19},{"VIDEO-TRACKING",20},{"ARDUINO",21}
};
static const WcSaveItem g_bin_col3[] = {
  {"AUDIO.Conf",22},{"Windows",24},{"ColorProfile",25},{"Keyboard.conf",26},{"Indexes",27},
  {"GRID",29},{"GridPl1.CueList",30},{"Light Plot",31},{"DRAW",32},{"ECHO",33},{"Save Presets",35}
};
static const int g_bin_col1_n = 11;
static const int g_bin_col2_n = 11;
static const int g_bin_col3_n = 11;

int selecteur_binary_save_solo_global(int xrep, int yrep)
{
Rect SwitchGlobal(Vec2D(xrep+170,yrep-35),Vec2D(70,20));
SwitchGlobal.SetRoundness(7.5);


if(window_focus_id==W_SAVE && mouse_x>xrep+170 && mouse_x<xrep+240 && mouse_y>yrep-35 && mouse_y<yrep-15)
{
SwitchGlobal.Draw(CouleurSurvol);
}
if (!index_save_global_is)
{
SwitchGlobal.Draw(CouleurBlind.WithAlpha(alpha_blinker));
petitchiffre.Print("CHOICE",xrep+180,yrep-20);
}
else
{
SwitchGlobal.Draw(CouleurFader);
petitchiffre.Print("GLOBAL",xrep+180,yrep-20);
}
SwitchGlobal.DrawOutline(CouleurLigne);


// colonne 1
for(int u=0;u<g_bin_col1_n;u++)
{
Rect SavePin(Vec2D(xrep,yrep+(u*15)),Vec2D(10,10));
if(specify_who_to_save_load[g_bin_col1[u].idx]==1){SavePin.Draw(CouleurFader); }
petitpetitchiffre.Print(g_bin_col1[u].label,xrep+20,yrep+(u*15)+10);
SavePin.DrawOutline(CouleurLigne);
}
// colonne 2
for(int u=0;u<g_bin_col2_n;u++)
{
Rect SavePinSnd(Vec2D(xrep+110,yrep+(u*15)),Vec2D(10,10));
if(specify_who_to_save_load[g_bin_col2[u].idx]==1){SavePinSnd.Draw(CouleurFader); }
petitpetitchiffre.Print(g_bin_col2[u].label,xrep+130,yrep+(u*15)+10);
SavePinSnd.DrawOutline(CouleurLigne);
}
// colonne 3
for(int u=0;u<g_bin_col3_n;u++)
{
Rect SavePinThrd(Vec2D(xrep+240,yrep+(u*15)),Vec2D(10,10));
if(specify_who_to_save_load[g_bin_col3[u].idx]==1){SavePinThrd.Draw(CouleurFader); }
petitpetitchiffre.Print(g_bin_col3[u].label,xrep+260,yrep+(u*15)+10);
SavePinThrd.DrawOutline(CouleurLigne);
}

 return(0);
}

int do_logical_selecteur_binary_save_solo_global(int xrep, int yrep)
{

if(window_focus_id==W_SAVE && mouse_x>xrep+170 && mouse_x<xrep+240 && mouse_y>yrep-35 && mouse_y<yrep-15)
{

if(mouse_button==1 && mouse_released==0)
{

index_save_global_is=toggle(index_save_global_is);
if(index_save_global_is==1)
{
set_all_saves_indexes_at(1);
}
else // si mode choice, on efface tous les indexes
{
set_all_saves_indexes_at(0);
}
mouse_released=1;
}
}


// colonne 1
for(int u=0;u<g_bin_col1_n;u++)
{
if(window_focus_id==W_SAVE && mouse_x>xrep && mouse_x<xrep+50 && mouse_y>yrep+(u*15) && mouse_y<yrep+(u*15)+15)
{
if(mouse_button==1 && mouse_released==0)
{
index_save_global_is=0;
specify_who_to_save_load[g_bin_col1[u].idx]=toggle(specify_who_to_save_load[g_bin_col1[u].idx]);
mouse_released=1;
}
}
}
// colonne 2
for(int u=0;u<g_bin_col2_n;u++)
{
if(window_focus_id==W_SAVE && mouse_x>xrep+110 && mouse_x<xrep+160 && mouse_y>yrep+(u*15) && mouse_y<yrep+(u*15)+15)
{
if(mouse_button==1 && mouse_released==0)
{
index_save_global_is=0;
specify_who_to_save_load[g_bin_col2[u].idx]=toggle(specify_who_to_save_load[g_bin_col2[u].idx]);
mouse_released=1;
}
}
}
// colonne 3
for(int u=0;u<g_bin_col3_n;u++)
{
if(window_focus_id==W_SAVE && mouse_x>xrep+240 && mouse_x<xrep+290 && mouse_y>yrep+(u*15) && mouse_y<yrep+(u*15)+15)
{
if(mouse_button==1 && mouse_released==0)
{
index_save_global_is=0;
specify_who_to_save_load[g_bin_col3[u].idx]=toggle(specify_who_to_save_load[g_bin_col3[u].idx]);
mouse_released=1;
}
}
}

 return(0);
}

int Save_Menu(int xsave, int ysave)
{
Rect backSave(Vec2D(xsave,ysave),Vec2D(420,520));
backSave.SetRoundness(15);
backSave.SetLineWidth(epaisseur_bordure_fenetre);
backSave.Draw(CouleurFond);
backSave.DrawOutline(CouleurLigne);

if(window_focus_id==W_SAVE){backSave.DrawOutline(CouleurFader);}

neuro.Print("SAVE",xsave+110, ysave+30);


Rect TogBSav(Vec2D(xsave+310,ysave+15),Vec2D(100,20));
TogBSav.SetRoundness(5);
TogBSav.SetLineWidth(epaisseur_ligne_fader);



if (!index_save_mode_export_or_binary)
{
petitchiffre.Print("  BINARY",xsave+325,ysave+30);
selecteur_binary_save_solo_global(xsave+30, ysave+50);
deroule_repertoire_classical_save(xsave+20, ysave+85, "saves");
choose_personnal_preset_binary_save_load(xsave+300, ysave+270);
}
else
{
TogBSav.Draw(CouleurSurvol);
petitchiffre.Print("IMP-EXPORT",xsave+325,ysave+30);
if(isPdf==1) {selecteur_PDF_save_solo_global(xsave+30, ysave+50);   }
deroule_repertoire_export_import(xsave+20, ysave+85, "import_export");

}
TogBSav.DrawOutline(CouleurLigne);

Rect ShowReport(Vec2D(xsave+300,ysave+430),Vec2D(70,25));
ShowReport.SetRoundness(5);
ShowReport.Draw(CouleurSurvol.WithAlpha(index_show_save_load_report));
ShowReport.DrawOutline(CouleurLigne);
petitchiffre.Print("Show",xsave+305,ysave+440);
petitchiffre.Print("Log",xsave+315,ysave+450);


return(0);
}


int do_logical_Save_Menu(int xsave, int ysave)
{

if(window_focus_id==W_SAVE && mouse_x>xsave+310 && mouse_x<xsave+410 && mouse_y>ysave+15 && mouse_y<ysave+35)
{
if(mouse_button==1 && mouse_released==0)
{
index_save_mode_export_or_binary=toggle(index_save_mode_export_or_binary);
mouse_released=1;
}
}



if (!index_save_mode_export_or_binary)
{
do_logical_selecteur_binary_save_solo_global(xsave+30, ysave+50);
do_logical_deroule_repertoire_classical_save(xsave+20, ysave+85, "saves");
do_logical_choose_personnal_preset_binary_save_load(xsave+300, ysave+270);
}
else
{
if(isPdf==1) {do_logical_selecteur_PDF_save_solo_global(xsave+30, ysave+50);   }
do_logical_deroule_repertoire_export_import(xsave+20, ysave+85, "import_export");

}
if(window_focus_id==W_SAVE && mouse_x>xsave+300 && mouse_x<xsave+370 && mouse_y>ysave+430 && mouse_y<ysave+455)
{
if(mouse_button==1 && mouse_released==0)
{
index_show_save_load_report=toggle(index_show_save_load_report);
mouse_released=1;
there_is_change_on_show_save_state=1;
}

}

return(0);
}

