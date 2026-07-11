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

/**

* \file Call_everybody_5.cpp
* \brief {no idea}
* \author Christoph Guillermet
* \version {0.8.6.3}
* \date {12/02/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Aucune idée à quoi sert ce fichier
*
*   No idea what it does
*
 **/

#include "wc_tus.h"
#include "gui_boutons_rebuild1.h"
#include "channels_core.h"

int reset_other_index_cfg()
{
  index_config_dmx=0;
  index_config_midi=0;
  index_config_arduino=0;
  index_setup_gfx=0;
  index_do_affect_net_to_dock=0;
  index_config_network=0;
  receive_from_trichro=0;
  index_show_config_window=0;
  return(0);
}


int do_logical_FunctionBoxChannel(int fx,int fy, int flarg, int fhaut, int space)
{
for (int df=0;df<5;df++)
{
if (mouse_x>=fx && mouse_x<=fx+flarg && mouse_y>=(fy + (df*fhaut)+ (space*df)) && mouse_y<=(fy+fhaut + (df*fhaut)+ (space*df)) && mouse_released==0 )
{

switch (df)
{
case 0: //select all
if( Midi_Faders_Affectation_Type!=0)//config midi
{
switch(miditable[0][682])
  {
  case 0: sprintf(thetypinfo,"Note");break;
  case 1: sprintf(thetypinfo,"Key On");break;
  case 2: sprintf(thetypinfo,"Key Off");break;
  case 4: sprintf(thetypinfo,"Ctrl Change");break;
  }
sprintf(string_last_midi_id,"NumPad ALL is Ch: %d Pitch: %d Typ: %s" ,miditable[1][682],miditable[2][682],thetypinfo);
attribute_midi_solo_affectation(682,Midi_Faders_Affectation_Mode);
}
else
{
simulate_keypress(KEY_Y<<8);
}
break;
case 1://inverse sel
if( Midi_Faders_Affectation_Type!=0)//config midi
{
switch(miditable[0][683])
  {
  case 0: sprintf(thetypinfo,"Note");break;
  case 1: sprintf(thetypinfo,"Key On");break;
  case 2: sprintf(thetypinfo,"Key Off");break;
  case 4: sprintf(thetypinfo,"Ctrl Change");break;
  }
sprintf(string_last_midi_id,"NumPad INV is Ch: %d Pitch: %d Typ: %s" ,miditable[1][683],miditable[2][683],thetypinfo);
attribute_midi_solo_affectation(683,Midi_Faders_Affectation_Mode);
}
else
{
simulate_keypress(KEY_U<<8);
}
break;

case 2: //to channel funct
if(Midi_Faders_Affectation_Type!=0)//config midi
{
switch(miditable[0][684])
  {
  case 0: sprintf(thetypinfo,"Note");break;
  case 1: sprintf(thetypinfo,"Key On");break;
  case 2: sprintf(thetypinfo,"Key Off");break;
  case 4: sprintf(thetypinfo,"Ctrl Change");break;
  }
sprintf(string_last_midi_id,"NumPad Thru is Ch: %d Pitch: %d Typ: %s" ,miditable[1][684],miditable[2][684],thetypinfo);
attribute_midi_solo_affectation(684,Midi_Faders_Affectation_Mode);
}
else
{
simulate_keypress(KEY_TAB<<8);
}
break;
case 3:
channel_copy();
break;
case 4:
channel_paste();
break;
}
mouse_released=1;
}
}
return(0);
}


int FunctionBoxChannel(int fx,int fy, int flarg, int fhaut, int space)
{
for (int df=0;df<5;df++)
{
Rect FunctionChThr(Vec2D(fx, fy + (df*fhaut)+ (space*df)), Vec2D ( flarg,fhaut));
FunctionChThr.SetRoundness(10);
FunctionChThr.SetLineWidth(epaisseur_ligne_fader);
FunctionChThr.DrawOutline(CouleurLigne);
if( Midi_Faders_Affectation_Type!=0)//config midi
{
 if(mouse_x>fx && mouse_x<fx+flarg && mouse_y>fy + (df*fhaut)+ (space*df) && mouse_y<fy + (df*fhaut)+ (space*df)+fhaut)
  {
  FunctionChThr.DrawOutline(CouleurBlind);
  }
}
switch(df)
{
case 0:
petitchiffre.Print("  ALL",fx+5, fy+15 + (df*fhaut)+ (space*df));
petitpetitchiffre.Print(" Y",fx+40, fy+25 + (df*fhaut)+ (space*df));
break;
case 1:
petitchiffre.Print("  INV",fx+5, fy+15 + (df*fhaut)+ (space*df));
petitpetitchiffre.Print(" U",fx+40, fy+25 + (df*fhaut)+ (space*df));
break;
case 2:
petitchiffre.Print(" THRU",fx+5, fy+15 + (df*fhaut)+ (space*df));
petitpetitchiffre.Print("Tab",fx+40, fy+25 + (df*fhaut)+ (space*df));
break;
case 3:
petitchiffre.Print("  COPY",fx+5, fy+15 + (df*fhaut)+ (space*df));
petitpetitchiffre.Print(" Ctrl-C",fx+20, fy+25 + (df*fhaut)+ (space*df));
break;
case 4:
petitchiffre.Print("  PASTE",fx+5, fy+15 + (df*fhaut)+ (space*df));
petitpetitchiffre.Print(" Ctrl-V",fx+20, fy+25 + (df*fhaut)+ (space*df));
break;


}

}
return(0);
}





// Table partagée des 21 entrées de la fenêtre MENUS (4 colonnes de 4 + 1 de 5).
// idcmd = action (toggle fenêtre), fixe par entrée ; la position x/y est purement visuelle.
struct MenuItem { const char* name; const char* sc; int idmidi; int idcmd; };
// Ordre demandé : 4 colonnes de 4 + 1 colonne de 5 (remplissage colonne par colonne).
static const MenuItem g_menu_items[22] = {
  // colonne 1
  {"CUELIST","F9",751,0},{"TIME","F6",748,1},{"WIZARD","",1329,3},{"SAVE","",1330,4},
  // colonne 2
  {"PATCH","Shift-P",1331,10},{"LIST","",1332,11},{"GRID PL.","",1340,24},{"DRAW","",1659,35},
  // colonne 3
  {"FADERS","F10",752,20},{"MiniFaders","Shift-F10",1336,21},{"CHASERS","Shift-C",1337,23},{"Track.Vid.","F8",750,30},
  // colonne 4
  {"BANGER","F11",753,2},{"N-Pad","P",1338,41},{"AUDIO PL.","Ctrl-A",756,33},{"ECHO","",1662,32},
  // colonne 5
  {"CFG-Menu","Shift-F11",755,40},{"NAME","F5",747,34},{"Trichromy","F7",749,22},{"LIGHT PLOT","",1594,12},{"QUIT","Ctrl-F12",757,43},
  {"CTRL FIXT.","",1341,26}
};
static const int g_menu_col_x[5] = {10,80,150,220,290};
static const int g_menu_col_n[5] = {4,4,4,4,6};
static inline bool g_menu_state(int i){
  switch(i){
    // colonne 1
    case 0:return index_window_sequentiel; case 1:return index_time; case 2:return index_show_wizard_window;
    case 3:return index_menu_save;
    // colonne 2
    case 4:return index_patch_window; case 5:return index_list_projecteurs; case 6:return index_grider_window;
    case 7:return index_draw_window;
    // colonne 3
    case 8:return index_show_faders; case 9:return index_show_minifaders; case 10:return index_window_chasers;
    case 11:return index_video_window;
    // colonne 4
    case 12:return index_show_banger_window; case 13:return index_visual_pad; case 14:return index_show_audio_window;
    case 15:return index_show_echo_window;
    // colonne 5
    case 16:return index_show_config_window; case 17:return index_type; case 18:return index_trichro_window;
    case 19:return index_plot_window; case 20:return index_do_quit_with_save;
    case 21:return index_window_fixturectl;
  }
  return false;
}

int do_logical_Menus( int xmenu, int ymenu)
{
if(window_focus_id==W_MAINMENU)
{
//AUTOCLOSE
if(mouse_x> xmenu+330 && mouse_x< xmenu+360 && mouse_y>ymenu+8  && mouse_y<ymenu+23 && mouse_button==1 && mouse_released==0)
{
index_text_auto_close=toggle(index_text_auto_close);
mouse_released=1;
}


int midx=0;
for(int c=0;c<5;c++)
{
for(int r=0;r<g_menu_col_n[c];r++)
{
command_button_logical(xmenu+g_menu_col_x[c],ymenu+50+(r*25),g_menu_state(midx),g_menu_items[midx].name,g_menu_items[midx].sc,g_menu_items[midx].idmidi,g_menu_items[midx].idcmd);
midx++;
}
}
//fin condition focus + souris
}
 return(0);
}


int Menus( int xmenu, int ymenu)
{
Rect MyMainMenu(Vec2D(xmenu,ymenu),Vec2D(size_x_mainmenu,size_y_mainmenu));
MyMainMenu.SetRoundness(15);
MyMainMenu.Draw(CouleurFond);
MyMainMenu.SetLineWidth(epaisseur_bordure_fenetre);
if(window_focus_id==W_MAINMENU)
{
MyMainMenu.DrawOutline(CouleurFader);
}
else {MyMainMenu.DrawOutline(CouleurLigne); }
neuro.Print("MENUS",xmenu+100,ymenu+30);


// TEXT autoclose
Rect Autoclose( Vec2D( xmenu+330,ymenu+8), Vec2D ( 30,15));
Autoclose.SetRoundness(5);
Autoclose.Draw(CouleurBlind.WithAlpha(index_text_auto_close));
Autoclose.DrawOutline(CouleurLigne);
petitchiffre.Print("Text",xmenu+295, ymenu+20);
petitchiffre.Print("AutoClose",xmenu+295, ymenu+34);
petitchiffre.Print("XX",xmenu+337, ymenu+20);




int midx=0;
for(int c=0;c<5;c++)
{
for(int r=0;r<g_menu_col_n[c];r++)
{
command_button_view(xmenu+g_menu_col_x[c],ymenu+50+(r*25),g_menu_state(midx),g_menu_items[midx].name,g_menu_items[midx].sc,g_menu_items[midx].idmidi);
midx++;
}
}

return(0);
}


