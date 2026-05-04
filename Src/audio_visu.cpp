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

* \file audio_visu4.cpp
* \brief {Audio GUI fonctions}
* \author Christoph Guillermet
* \version {0.8.6.3}
* \date {12/02/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Gestion des fonctions de la fenetre Audio
*
*   Audio GUI fonctions
*
 **/

#include "graphics_backend.h"
#include "audio_backend.h"
#include "midi_backend.h"
#include <string.h>
using namespace ol;
using namespace audiere;
#include "audio.h"
#include "theme.h"
#include "display.h"
#include "ui_indexes.h"
#include "midi.h"
#include "audio_visu.h"

// Forward declarations — defined in CORE_6.cpp
int toggle(int index_to_toggle);
// Forward declarations — defined in gui_boutons_rebuild1.cpp
int raccrochage_midi_visuel_horizontal_audio(int Xb, int Yb, int IDmidi, int largeur, int hauteur);
int raccrochage_midi_visuel_vertical(int Xb, int Yb, int IDmidi, int largeur, int hauteur);

int fader_niveau_son(int xp, int yp, int numero)
{
Rect FaderSon(Vec2D(xp,yp),Vec2D(20,127));
FaderSon.SetRoundness(5);
FaderSon.Draw(CouleurNoir);
Rect FaderSonNiveau(Vec2D(xp,yp+(127-player_niveauson[numero])),Vec2D(20,player_niveauson[numero]));
FaderSonNiveau.SetRoundness(5);
Canvas::SetClipping(xp, yp, 20, 127);
FaderSonNiveau.Draw(CouleurNiveau);
Canvas::DisableClipping();
FaderSon.DrawOutline(CouleurLigne.WithAlpha(0.5));

petitpetitchiffre.Print(string_niveauson[numero],xp+22,yp+10);
petitpetitchiffre.Print(ol::ToString(player_niveauson[numero]),xp+22,yp+20);

if(window_focus_id==919 && Midi_Faders_Affectation_Type!=0 && mouse_x>xp && mouse_x<xp+20 && mouse_y>=yp && mouse_y<=yp+127 )
{
FaderSon.DrawOutline(CouleurBlind);
}

Line (Vec2D(xp+20,yp+107),Vec2D(xp+30,yp+112)).Draw(CouleurLigne);

Circle(Vec2D(xp+37,yp+117),7).Draw(CouleurBlind.WithAlpha(midi_send_out[616+numero]));
Circle(Vec2D(xp+37,yp+117),7).DrawOutline(CouleurLigne);

raccrochage_midi_visuel_vertical(xp,yp,616+numero,20,127);

return(0);
}

int lecteur_audio( int xp, int yp, int numero)
{
Rect FramePlayer(Vec2D(xp-3,yp-5),Vec2D(340,135));
FramePlayer.SetRoundness(5);
FramePlayer.Draw(CouleurBleuProcedure.WithAlpha(0.7));
Rect Player(Vec2D(xp,yp),Vec2D(200,20));
Player.SetRoundness(5);
Player.Draw(CouleurConfig.WithAlpha(0.8));
Player.DrawOutline(CouleurLigne);

Canvas::SetClipping( xp,yp,200,20);
petitdoomInspekt.Print(ol::ToString(numero+1),xp+5,yp+12);
petitchiffre.Print(sound_files[numero],xp+20,yp+12);
Canvas::DisableClipping();


// SEEKBAR
{
    int seekbar_w = 200;
    int seekbar_h = 8;
    Rect SeekBg(Vec2D(xp, yp+23), Vec2D(seekbar_w, seekbar_h));
    SeekBg.SetRoundness(2);
    SeekBg.Draw(CouleurFond.WithAlpha(0.7));
    SeekBg.DrawOutline(CouleurLigne.WithAlpha(0.5));
    if (length_of_file_in_player[numero] > 0) {
        float ratio = audio_seekbar_dragging[numero]
            ? (float)(mouse_x - xp) / (float)seekbar_w
            : (float)position_of_file_in_player[numero] / (float)length_of_file_in_player[numero];
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        int fill_w = (int)(ratio * seekbar_w);
        if (fill_w > 0) {
            Rect SeekFill(Vec2D(xp, yp+23), Vec2D(fill_w, seekbar_h));
            SeekFill.SetRoundness(2);
            SeekFill.Draw(CouleurBlind.WithAlpha(0.8));
        }
    }
    // cue in / out markers
    if (length_of_file_in_player[numero] > 0 && player_is_onloopCue[numero]) {
        int in_x  = xp + (int)((float)audiofile_cue_in_out_pos[player_has_file_coming_from_pos[numero]][numero][0] / length_of_file_in_player[numero] * seekbar_w);
        int out_x = xp + (int)((float)player_loop_out_position[numero] / length_of_file_in_player[numero] * seekbar_w);
        Line(Vec2D(in_x,  yp+22), Vec2D(in_x,  yp+32)).Draw(CouleurLigne);
        Line(Vec2D(out_x, yp+22), Vec2D(out_x, yp+32)).Draw(CouleurLigne);
    }
}

//PLAY / Pause
Rect Play(Vec2D(xp,yp+35),Vec2D(20,20));
Play.SetRoundness(4);
Play.Draw(CouleurBlind.WithAlpha( player_is_playing[numero]));
Play.Draw(CouleurFond.WithAlpha(0.5));
Play.DrawOutline(CouleurLigne);
Line(Vec2D(xp+6,yp+39),Vec2D(xp+6,yp+51)).Draw(CouleurLigne);
Line(Vec2D(xp+6,yp+39),Vec2D(xp+16,yp+45)).Draw(CouleurLigne);
Line(Vec2D(xp+6,yp+51),Vec2D(xp+16,yp+45)).Draw(CouleurLigne);





petitpetitchiffre.Print(time_is_for_filePos[numero],xp+120,yp+45);

petitpetitchiffre.Print(time_is_for_fileTotal[numero],xp+120,yp+55);



//SEEK TO 0
Rect SeekToZero(Vec2D(xp+25,yp+35),Vec2D(20,20));
SeekToZero.SetRoundness(4);
SeekToZero.Draw(CouleurFond.WithAlpha(0.5));
Line(Vec2D(xp+41,yp+39),Vec2D(xp+41,yp+51)).Draw(CouleurLigne);
Line(Vec2D(xp+41,yp+39),Vec2D(xp+31,yp+45)).Draw(CouleurLigne);
Line(Vec2D(xp+31,yp+45),Vec2D(xp+41,yp+51)).Draw(CouleurLigne);
Line(Vec2D(xp+29,yp+39),Vec2D(xp+29,yp+51)).Draw(CouleurLigne);

SeekToZero.DrawOutline(CouleurLigne);

if(midi_show_flash_seektouch[numero]==1){SeekToZero.Draw(CouleurFader);midi_show_flash_seektouch[numero]=0;}





//GENERAL LOOP
Rect GeneralLoop(Vec2D(xp+50,yp+35),Vec2D(20,20));
GeneralLoop.SetRoundness(4);
GeneralLoop.Draw(CouleurFond.WithAlpha(0.5));
GeneralLoop.DrawOutline(CouleurLigne.WithAlpha(0.5));
Circle(Vec2D(xp+60,yp+45),5).Draw(CouleurFader.WithAlpha(player_is_onloop[numero]));
Circle(Vec2D(xp+60,yp+45),5).DrawOutline(CouleurLigne);




//SEEK TO End
Rect SeekToEnd(Vec2D(xp+75,yp+35),Vec2D(20,20));
SeekToEnd.SetRoundness(4);
SeekToEnd.Draw(CouleurFond.WithAlpha(0.5));
Line(Vec2D(xp+79,yp+39),Vec2D(xp+79,yp+51)).Draw(CouleurLigne);
Line(Vec2D(xp+79,yp+39),Vec2D(xp+89,yp+45)).Draw(CouleurLigne);
Line(Vec2D(xp+89,yp+45),Vec2D(xp+79,yp+51)).Draw(CouleurLigne);
Line(Vec2D(xp+91,yp+39),Vec2D(xp+91,yp+51)).Draw(CouleurLigne);
SeekToEnd.DrawOutline(CouleurLigne);




//Backward position
Rect Backward(Vec2D(xp,yp+63),Vec2D(28,16));
Backward.SetRoundness(0.5);
Backward.Draw(CouleurFond.WithAlpha(0.5));
Backward.DrawOutline(CouleurLigne.WithAlpha(0.5));

if(midi_show_flash_backwardtouch[numero]==1){Backward.Draw(CouleurBlind);midi_show_flash_backwardtouch[numero]=0;}


Line(Vec2D(xp+16,yp+65),Vec2D(xp+16,yp+77)).Draw(CouleurLigne);
Line(Vec2D(xp+16,yp+65),Vec2D(xp+6,yp+71)).Draw(CouleurLigne);
Line(Vec2D(xp+16,yp+77),Vec2D(xp+6,yp+71)).Draw(CouleurLigne);

Line(Vec2D(xp+26,yp+65),Vec2D(xp+26,yp+77)).Draw(CouleurLigne);
Line(Vec2D(xp+26,yp+65),Vec2D(xp+16,yp+71)).Draw(CouleurLigne);
Line(Vec2D(xp+26,yp+77),Vec2D(xp+16,yp+71)).Draw(CouleurLigne);

//Forward position
Rect Forward(Vec2D(xp+35,yp+63),Vec2D(28,16));
Forward.SetRoundness(0.5);
Forward.Draw(CouleurFond.WithAlpha(0.5));
Forward.DrawOutline(CouleurLigne.WithAlpha(0.5));
if(midi_show_flash_forwardtouch[numero]==1){Forward.Draw(CouleurBlind);midi_show_flash_forwardtouch[numero]=0;}


Line(Vec2D(xp+36,yp+65),Vec2D(xp+36,yp+77)).Draw(CouleurLigne);
Line(Vec2D(xp+36,yp+65),Vec2D(xp+46,yp+71)).Draw(CouleurLigne);
Line(Vec2D(xp+36,yp+77),Vec2D(xp+46,yp+71)).Draw(CouleurLigne);

Line(Vec2D(xp+46,yp+65),Vec2D(xp+46,yp+77)).Draw(CouleurLigne);
Line(Vec2D(xp+46,yp+65),Vec2D(xp+56,yp+71)).Draw(CouleurLigne);
Line(Vec2D(xp+46,yp+77),Vec2D(xp+56,yp+71)).Draw(CouleurLigne);



//previous position
Rect Previous(Vec2D(xp,yp+88),Vec2D(45,16));
Previous.SetRoundness(0.5);
Previous.Draw(CouleurFond.WithAlpha(0.5));
Previous.DrawOutline(CouleurLigne.WithAlpha(0.5));
petitchiffre.Print("PREV",xp+6,yp+99);





//nEXT TRACK position
Rect Next(Vec2D(xp+50,yp+88),Vec2D(45,16));
Next.SetRoundness(0.5);
Next.Draw(CouleurFond.WithAlpha(0.5));
Next.DrawOutline(CouleurLigne.WithAlpha(0.5));
petitchiffre.Print("NEXT",xp+56,yp+99);


//NUM FICHIER
Rect NumFichier(Vec2D(xp+70,yp+60),Vec2D(40,19));
NumFichier.SetRoundness(3);
NumFichier.Draw(CouleurFond.WithAlpha(0.5));
neuromoyen.Print(ol::ToString(player_has_file_coming_from_pos[numero]),xp+80,yp+76);
NumFichier.DrawOutline(CouleurLigne.WithAlpha(0.5));




if(window_focus_id==919 && mouse_x>xp+70 && mouse_x<xp+110 && mouse_y>yp+60 && mouse_y<yp+79)
{
NumFichier.DrawOutline(CouleurLigne.WithAlpha(0.5));
}
//autoload cuelist
//audio_auto_load
Rect Autoload(Vec2D(xp,yp+110),Vec2D(28,16));
Autoload.SetRoundness(0.5);
Autoload.Draw(CouleurFond.WithAlpha(0.5));
Autoload.Draw(CouleurBlind.WithAlpha(audio_autoload[numero]));
Autoload.DrawOutline(CouleurLigne.WithAlpha(0.5));


Line(Vec2D(xp+3,yp+112),Vec2D(xp+20,yp+112)).Draw(CouleurLigne);
Line(Vec2D(xp+20,yp+112),Vec2D(xp+20,yp+123)).Draw(CouleurLigne);
Line(Vec2D(xp+20,yp+123),Vec2D(xp+27,yp+118)).Draw(CouleurLigne);
Line(Vec2D(xp+13,yp+118),Vec2D(xp+20,yp+123)).Draw(CouleurLigne);

//autopause
Rect Autostop(Vec2D(xp+35,yp+110),Vec2D(40,16));
Autostop.SetRoundness(0.5);
Autostop.Draw(CouleurFond.WithAlpha(0.5));
Autostop.Draw(CouleurBlind.WithAlpha(audio_autopause[numero]));
Autostop.DrawOutline(CouleurLigne.WithAlpha(0.5));
minichiffre.Print("A.PAUSE",xp+38,yp+120);



//SET cue IN
Rect SetLoopIN(Vec2D(xp+210,yp),Vec2D(20,20));
SetLoopIN.SetRoundness(4);


if(midi_show_flash_cueIntouch[numero]==1){SetLoopIN.Draw(CouleurBlind);midi_show_flash_cueIntouch[numero]=0;}

SetLoopIN.DrawOutline(CouleurLigne);
Line(Vec2D(xp+213,yp),Vec2D(xp+220,yp+9)).Draw(CouleurLigne);
Line(Vec2D(xp+227,yp),Vec2D(xp+220,yp+9)).Draw(CouleurLigne);
petitpetitchiffre.Print("in",xp+215,yp+17);




//SET cue out
Rect SetLoopOut(Vec2D(xp+235,yp),Vec2D(20,20));
SetLoopOut.SetRoundness(4);

if(midi_show_flash_cueOuttouch[numero]==1){SetLoopOut.Draw(CouleurBlind);midi_show_flash_cueOuttouch[numero]=0;}
SetLoopOut.DrawOutline(CouleurLigne);

Line(Vec2D(xp+238,yp),Vec2D(xp+245,yp+9)).Draw(CouleurLigne);
Line(Vec2D(xp+252,yp),Vec2D(xp+245,yp+9)).Draw(CouleurLigne);
petitpetitchiffre.Print("out",xp+235,yp+17);


//CUE ON OFF
Rect CueOn(Vec2D(xp+260,yp),Vec2D(20,20));
CueOn.SetRoundness(4);
CueOn.Draw(CouleurBlind.WithAlpha(player_is_onloopCue[numero]));
petitpetitchiffre.Print("cue",xp+260,yp+12);
CueOn.DrawOutline(CouleurLigne);


petitpetitchiffre.Print(time_is_for_fileCueIn[numero],xp+234,yp+40);
petitpetitchiffre.Print(time_is_for_fileCueOut[numero],xp+234,yp+50);

//Seek cue
Rect SeekCue(Vec2D(xp+210,yp+35),Vec2D(20,20));
SeekCue.SetRoundness(4);
SeekCue.DrawOutline(CouleurLigne);
if(midi_show_flash_cueSeektouch[numero]==1){SeekCue.Draw(CouleurBlind);midi_show_flash_cueSeektouch[numero]=0;}

Line(Vec2D(xp+226,yp+39),Vec2D(xp+226,yp+51)).Draw(CouleurLigne);
Line(Vec2D(xp+226,yp+39),Vec2D(xp+216,yp+45)).Draw(CouleurLigne);
Line(Vec2D(xp+216,yp+45),Vec2D(xp+226,yp+51)).Draw(CouleurLigne);
Line(Vec2D(xp+214,yp+39),Vec2D(xp+214,yp+51)).Draw(CouleurLigne);





//Pitch
Line(Vec2D(xp+130,yp+115),Vec2D(xp+257,yp+115)).Draw(CouleurLigne);//ligne horizontale
Line(Vec2D(xp+194,yp+105),Vec2D(xp+194,yp+125)).Draw(CouleurLigne);//barre 64
Line(Vec2D(xp+257,yp+110),Vec2D(xp+257,yp+115)).Draw(CouleurLigne);//barre 127
Rect PitchPos(Vec2D(xp+120+player_pitch[numero],yp+110),Vec2D(20,10));
PitchPos.SetRoundness(4);
PitchPos.Draw(CouleurGrisMoyen);
PitchPos.DrawOutline(CouleurLigne);
Rect PitchMidi(Vec2D(xp+130,yp+110),Vec2D(127,10));
petitpetitchiffre.Print(string_pitch[numero],(xp+130),(yp+107));
petitpetitchiffre.Print(ol::ToString(player_pitch[numero]),(xp+110),(yp+117));




raccrochage_midi_visuel_horizontal_audio (xp+130, yp+100, 620+numero, 127,10);
//Pan
Line(Vec2D(xp+130,yp+90),Vec2D(xp+257,yp+90)).Draw(CouleurLigne);//ligne horizontale
Line(Vec2D(xp+194,yp+80),Vec2D(xp+194,yp+100)).Draw(CouleurLigne);//barre 64
Line(Vec2D(xp+257,yp+85),Vec2D(xp+257,yp+90)).Draw(CouleurLigne);//barre 127
Rect PanPos(Vec2D(xp+120+player_pan[numero],yp+85),Vec2D(20,10));
PanPos.SetRoundness(4);
PanPos.Draw(CouleurGrisMoyen);
PanPos.DrawOutline(CouleurLigne);
Rect PanMidi(Vec2D(xp+130,yp+85),Vec2D(127,10));

petitpetitchiffre.Print(string_pan[numero],(xp+130),(yp+82));
petitpetitchiffre.Print(ol::ToString(player_pan[numero]),(xp+110),(yp+94));




raccrochage_midi_visuel_horizontal_audio (xp+130, yp+80, 624+numero, 127,10);
//les faders son
fader_niveau_son(xp+290,yp,numero);

if(window_focus_id==919 && Midi_Faders_Affectation_Type!=0 )
{
if( mouse_x>xp && mouse_x<xp+20 && mouse_y>yp+35 && mouse_y<yp+55){Play.DrawOutline(CouleurBlind);}
else if(mouse_x>xp+25 && mouse_x<xp+45 && mouse_y>yp+35 && mouse_y<yp+55){SeekToZero.DrawOutline(CouleurBlind);}
else if( mouse_x>xp+50 && mouse_x<xp+70 && mouse_y>yp+35 && mouse_y<yp+55){GeneralLoop.DrawOutline(CouleurBlind);}
else if( mouse_x>xp+75 && mouse_x<xp+95 && mouse_y>yp+35 && mouse_y<yp+55){SeekToEnd.DrawOutline(CouleurBlind);}
else if( mouse_x>xp && mouse_x<xp+28 && mouse_y>yp+63 && mouse_y<yp+79){Backward.DrawOutline(CouleurBlind);}
else if ( mouse_x>xp+35 && mouse_x<xp+63 && mouse_y>yp+63 && mouse_y<yp+79){Forward.DrawOutline(CouleurBlind);}
else  if( mouse_x>xp && mouse_x<xp+45 && mouse_y>yp+88 && mouse_y<yp+104){Previous.DrawOutline(CouleurBlind);}
else if ( mouse_x>xp+50 && mouse_x<xp+95 && mouse_y>yp+88 && mouse_y<yp+104){Next.DrawOutline(CouleurBlind);}
else if ( mouse_x>xp+70 && mouse_x<xp+110 && mouse_y>yp+60 && mouse_y<yp+79) {NumFichier.DrawOutline(CouleurBlind);}
else if (  mouse_x>xp && mouse_x<xp+28 && mouse_y>yp+110 && mouse_y<yp+126){Autoload.DrawOutline(CouleurBlind);}
else if (  mouse_x>xp+35 && mouse_x<xp+75 && mouse_y>yp+110 && mouse_y<yp+126){Autostop.DrawOutline(CouleurBlind);}
else if ( mouse_x>xp+210 && mouse_x<xp+240 && mouse_y>yp && mouse_y<yp+20) {SetLoopIN.DrawOutline(CouleurBlind);}
else if( mouse_x>xp+235 && mouse_x<xp+255 && mouse_y>yp && mouse_y<yp+20) {SetLoopOut.DrawOutline(CouleurBlind);}
else if( mouse_x>xp+260 && mouse_x<xp+280 && mouse_y>yp && mouse_y<yp+20){CueOn.DrawOutline(CouleurBlind);}
else if(  mouse_x>xp+210 && mouse_x<xp+230 && mouse_y>yp+35 && mouse_y<yp+55){SeekCue.DrawOutline(CouleurBlind);}
else if (  mouse_x>xp+120+player_pitch[numero] && mouse_x<xp+150+player_pitch[numero] && mouse_y>yp+110 && mouse_y<yp+120)
{PitchMidi.DrawOutline(CouleurBlind);}
else if (mouse_x>xp+120+player_pan[numero] && mouse_x<xp+150+player_pan[numero] && mouse_y>yp+85 && mouse_y<yp+95)
{PanMidi.DrawOutline(CouleurBlind);}
}

return(0);
}


int fenetre_audio(int xb,int yb)
{

Rect audio_backg(Vec2D(xb,yb), Vec2D( 600,150+(index_nbre_players_visibles*120)));
audio_backg.SetRoundness(15);
audio_backg.Draw(CouleurFond);
audio_backg.SetLineWidth(epaisseur_bordure_fenetre);
if(window_focus_id==919)
{
audio_backg.DrawOutline(CouleurFader);
}
else
{
audio_backg.DrawOutline(CouleurLigne);
}

neuro.Print( "AudioPlayers",(xb+80), (yb+30));

//FOLDER
Rect AudioFolderis(Vec2D(xb+350,yb+10),Vec2D(240,30));
AudioFolderis.SetRoundness(7.5);
AudioFolderis.Draw(Rgba(0.05, 0.12, 0.40));
Canvas::SetClipping(xb+352, yb+10, 210, 30);
neuromoyen.Print(audio_folder,xb+365,yb+25);
Canvas::DisableClipping();
neuromoyen.Print("v",xb+575,yb+25);


//////////////////////LISTE sons///////////////////////////////////////
// clamp line_audio pour éviter les items inaccessibles après changement de mode
{
    int vis_count = index_nbre_players_visibles*6 - 1;
    int max_la = audio_number_total_in_folder > vis_count ? audio_number_total_in_folder - vis_count : 0;
    if(line_audio > max_la) line_audio = max_la;
    if(line_audio < 0) line_audio = 0;
}

Rect BackDeroule(Vec2D(xb+350,yb+45),Vec2D(240,(index_nbre_players_visibles*120)));
BackDeroule.SetRoundness(15);
BackDeroule.Draw(CouleurConfig.WithAlpha(0.7));
bool ind=0;

for (int y=1;y<(index_nbre_players_visibles*6);y++)
{
Rect OverAudioFile(Vec2D(xb+355,(yb+33+(y*20))),Vec2D(200,18));
OverAudioFile.SetRoundness(7.5);

OverAudioFile.Draw(CouleurFond.WithAlpha(0.5*ind));
ind=toggle(ind);
//affichage qui est selectionné
if(strcmp(audiofile_name,list_audio_files[audiofile_selected])==0 && (audiofile_selected==(y+line_audio)) )
{OverAudioFile.Draw(CouleurBlind);}

if(window_focus_id==919 && mouse_x>xb+355 && mouse_x<xb+505 && mouse_y>(yb+33+(y*20)) && mouse_y<(yb+48+(y*20)))
{
OverAudioFile.DrawOutline(CouleurLigne);
}


petitpetitchiffre.Print(ol::ToString(line_audio+y),xb+360,yb+45+(y*20));
Canvas::SetClipping( xb+355,yb+35+(y*20),230,20);
petitpetitchiffre.Print(list_audio_files[line_audio+y],xb+375,yb+45+(y*20));
Canvas::DisableClipping();
}

//////////////////SCROLLBAR LISTE MORCEAUX/////////////////////
bool need_audio_scroll = (audio_number_total_in_folder > index_nbre_players_visibles*6 - 1);
if(need_audio_scroll)
{
    const int bar_w = 14;
    int bx = xb+350+240-bar_w-1;
    int by = yb+45;
    int bar_h = index_nbre_players_visibles*120;
    int vis_count = index_nbre_players_visibles*6 - 1;
    int total = audio_number_total_in_folder;
    int max_scroll = total - vis_count;
    // fond
    Rect AudioScrollBar(Vec2D(bx, by), Vec2D(bar_w, bar_h));
    AudioScrollBar.SetRoundness(3);
    AudioScrollBar.Draw(Rgba(0, 0, 0));
    // flèche haut
    Rect AudioArrowUp(Vec2D(bx+1, by), Vec2D(bar_w-2, bar_w));
    AudioArrowUp.SetRoundness(2);
    AudioArrowUp.Draw(line_audio>0 ? CouleurGrisAnthracite : CouleurFond);
    petitpetitchiffre.Print("^", bx+4, by+10);
    // flèche bas
    Rect AudioArrowDown(Vec2D(bx+1, by+bar_h-bar_w), Vec2D(bar_w-2, bar_w));
    AudioArrowDown.SetRoundness(2);
    AudioArrowDown.Draw(line_audio<max_scroll ? CouleurGrisAnthracite : CouleurFond);
    petitpetitchiffre.Print("v", bx+4, by+bar_h-bar_w+10);
    // thumb
    int track_h = bar_h - 2*bar_w;
    int thumb_h = std::max(10, track_h * vis_count / total);
    int thumb_y = by + bar_w + (max_scroll>0 ? line_audio * (track_h - thumb_h) / max_scroll : 0);
    if (thumb_y < by + bar_w) thumb_y = by + bar_w;
    if (thumb_y + thumb_h > by + bar_h - bar_w) thumb_y = by + bar_h - bar_w - thumb_h;
    Rect AudioThumb(Vec2D(bx+2, thumb_y), Vec2D(bar_w-4, thumb_h));
    AudioThumb.SetRoundness(3);
    AudioThumb.Draw(audio_filelist_scroll_dragging ? CouleurGrisAnthracite : CouleurGrisAnthracite.WithAlpha(0.7));
}

//RESCAN FOLDER
    Rect AudioRescanDriver( Vec2D(xb+230,yb+10), Vec2D ( 50,20));
    AudioRescanDriver.SetRoundness(7.5);
    AudioRescanDriver.DrawOutline(CouleurLigne);

////////////////////////////////////////////////////////////////////////////////
//EDIT ON OFF
   Rect EditSound( Vec2D(xb+230,yb+40), Vec2D ( 50,20));
    EditSound.SetRoundness(7.5);
    if(index_edit_audio==1){EditSound.Draw(CouleurFader);}
    EditSound.DrawOutline(CouleurLigne);
    petitpetitchiffre.Print("EDIT",xb+240,yb+53);


//////LES LECTEURS//////////////////////////////////////////////////////////////
for(int lop=0;lop<index_nbre_players_visibles;lop++)
{
lecteur_audio(xb+7,yb+70+(lop*140),lop);
}

//////L AFFECTATION AUX FADERS//////////////////////////////////////////////////
//Rect AffectA(Vec2D(xb+440,yb+450),Vec2D(140,20));
Rect AffectA(Vec2D(xb+440,yb+55+(index_nbre_players_visibles*120)),Vec2D(140,20));
AffectA.SetRoundness(5);
AffectA.Draw(CouleurFader.WithAlpha(index_affect_audio_to_dock));
AffectA.DrawOutline(CouleurLigne);
petitchiffre.Print("AFFECT TO DOCK",xb+460,yb+70+(index_nbre_players_visibles*120));

petitchiffre.Print("Volume",xb+400,yb+100+(index_nbre_players_visibles*120));
petitchiffre.Print("Pan",xb+400,yb+120+(index_nbre_players_visibles*120));
petitchiffre.Print("Pitch",xb+400,yb+140+(index_nbre_players_visibles*120));

for(int pl=0;pl<index_nbre_players_visibles;pl++)
{
for (int co=0;co<3;co++)
{
 Rect AffectAudioType(Vec2D(xb+460+(pl*30),yb+90+(index_nbre_players_visibles*120)+(co*20)),Vec2D(20,15));
 AffectAudioType.SetRoundness(3);
 if(player_to_affect_to_dock==pl && audio_type_for_dock_affectation_is==co){AffectAudioType.Draw(CouleurFader);}
 AffectAudioType.DrawOutline(CouleurLigne);
}
 petitpetitchiffre.Print(ol::ToString((pl+1)),xb+467+(pl*30),yb+85+(index_nbre_players_visibles*120));
}


if(window_focus_id==919  )
{
if( mouse_x>xb+230 && mouse_x<xb+280 && mouse_y>yb+10 && mouse_y<yb+30 )
    {
    AudioRescanDriver.Draw(CouleurSurvol);
    }
}


petitpetitchiffre.Print("rescan!",xb+235,yb+23);

// dropdown dossiers audio — dessiné en dernier pour être par-dessus la liste
if (index_show_audio_folder_list) {
    const int row_h = 20;
    const int max_vis = 8;
    const int bar_w = 14;
    int vis = (nbre_audio_folders < max_vis) ? nbre_audio_folders : max_vis;
    int nb = (vis > 0) ? vis : 1;
    bool has_scroll = (nbre_audio_folders > max_vis);
    int drop_w = has_scroll ? 240 : 240;
    int list_w = has_scroll ? drop_w - bar_w - 2 : drop_w - 4;

    Rect FolderDrop(Vec2D(xb+350, yb+40), Vec2D(drop_w, nb*row_h+6));
    FolderDrop.SetRoundness(5);
    FolderDrop.Draw(CouleurFond);
    FolderDrop.DrawOutline(CouleurBlanc);

    if (nbre_audio_folders == 0) {
        petitpetitchiffre.Print("(aucun dossier)", xb+360, yb+52);
    } else {
        for (int vi = 0; vi < vis; vi++) {
            int fi = vi + audio_folder_list_scroll;
            if (fi >= nbre_audio_folders) break;
            Rect FolderRow(Vec2D(xb+352, yb+42+vi*row_h), Vec2D(list_w, row_h));
            FolderRow.SetRoundness(3);
            bool hovered = (window_focus_id==919 && mouse_x>xb+352 && mouse_x<xb+352+list_w
                            && mouse_y>yb+42+vi*row_h && mouse_y<yb+42+(vi+1)*row_h);
            if (strcmp(list_audio_folders[fi], audio_folder)==0)
                FolderRow.Draw(Rgba(0.05, 0.12, 0.40));
            else if (hovered)
                FolderRow.Draw(Rgba(0.05, 0.12, 0.40, 0.5));
            Canvas::SetClipping(xb+354, yb+42+vi*row_h, list_w-4, row_h);
            petitpetitchiffre.Print(list_audio_folders[fi], xb+358, yb+53+vi*row_h);
            Canvas::DisableClipping();
        }
        // scrollbar
        if (has_scroll) {
            int bx = xb+350+drop_w-bar_w-1;
            int by = yb+42;
            int bar_h = nb*row_h;
            // fond scrollbar
            Rect ScrollBar(Vec2D(bx, by), Vec2D(bar_w, bar_h));
            ScrollBar.SetRoundness(3);
            ScrollBar.Draw(Rgba(0, 0, 0));
            // flèche haut
            Rect ArrowUp(Vec2D(bx+1, by), Vec2D(bar_w-2, bar_w));
            ArrowUp.SetRoundness(2);
            ArrowUp.Draw(audio_folder_list_scroll>0 ? CouleurGrisAnthracite : CouleurFond);
            petitpetitchiffre.Print("^", bx+4, by+10);
            // flèche bas
            Rect ArrowDown(Vec2D(bx+1, by+bar_h-bar_w), Vec2D(bar_w-2, bar_w));
            ArrowDown.SetRoundness(2);
            ArrowDown.Draw(audio_folder_list_scroll<nbre_audio_folders-max_vis ? CouleurGrisAnthracite : CouleurFond);
            petitpetitchiffre.Print("v", bx+4, by+bar_h-bar_w+10);
            // thumb — position clampée dans la piste
            int track_h = bar_h - 2*bar_w;
            int thumb_h = std::max(10, track_h * vis / nbre_audio_folders);
            int max_scroll = nbre_audio_folders - max_vis;
            int thumb_y = by + bar_w + (max_scroll>0 ? audio_folder_list_scroll * (track_h - thumb_h) / max_scroll : 0);
            if (thumb_y < by + bar_w) thumb_y = by + bar_w;
            if (thumb_y + thumb_h > by + bar_h - bar_w) thumb_y = by + bar_h - bar_w - thumb_h;
            Rect Thumb(Vec2D(bx+2, thumb_y), Vec2D(bar_w-4, thumb_h));
            Thumb.SetRoundness(3);
            Thumb.Draw(audio_folder_scroll_dragging ? CouleurGrisAnthracite : CouleurGrisAnthracite.WithAlpha(0.7));
        }
    }
}

return(0);
}
