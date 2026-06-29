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

* \file arduino_core_6_UNO.cpp
* \brief {arduino fonctions}
* \author Christoph Guillermet
* \version {0.8.6.7}
* \date {21/04/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Gestion des fonctions Arduino.
*   recevoir les infos digital de l'arduino
*   recevoir les infos analogue de l'arduino
*   envoyer les infos de sortie PWM vers l'arduino
*   envoyer les infos de sortie digital vers l'arduino
*   configurer l'arduino automatiquement depuis whitecat avec le sketch arduino whitekitten
*
*   Arduino communication fonctions
*   to get digital data in from arduino,
*   get analog data in from arduino,
*   send PWM output data to the arduino
*   send digital output data to the arduino
*   or configure the arduino in a automatic way from whitecat with the arduino sketch whitekitten
*
 **/





#include "wc_tus.h"
#include "arduino_device_core.h"
#include "Draw3.h"
#include "sequentiel_core.h"
#include "arduino_core.h"
int simulate_midi(int letype, int lechan, int lepitch, int lavaleur);
int do_send_bang();
int do_go_back_bang(int banger_is);

//le volatile int dans arduino car possibilité de changer le rate d appel de la fonction read
volatile int ticks_arduino=0;
int old_ticks_arduino=0;
void ticker_arduino()
{
   ticks_arduino++;
}
END_OF_FUNCTION(ticker_arduino);


int  do_arduino_draw_input()
{
if( dragging_draw==0){snap_state_of_draw_grid(draw_preset_selected); dragging_draw=1;}
draw_point_is_traced[draw_preset_selected]=1;
Draw_point_and_perform_level_on_area_NEW(draw_preset_selected, draw_arduino_xy[0]+draw_arduino_xy[1],draw_arduino_xy[2]+draw_arduino_xy[3]);
return(0);
}


int arduino_do_analog_in_whitecat()
{
int vfader=0;
int temp_ardniveauX1=0;
int temp_ardniveauX2=0;

for(int p=0;p<arduino_max_analog;p++)
{
if(ventilate_analog_data[p]==1)//activation de la reception
{

if(analog_data_from_arduino[p]!=previous_analog_data_from_arduino[p])
{
switch(arduino_analog_function_input[p])
{
case 1://affectation à un fader

vfader=arduino_analog_attribution_input[p];
fader_set_level(vfader,wc::dmx8_to_lvl(analog_data_from_arduino[p]));// [fader 16 bit] analog 8 bit -> 16 bit
//Fader[vfader]=analog_data_from_arduino[p];
//midi_levels[vfader]=(Fader[vfader]/2);
//index_fader_is_manipulated[vfader]=1;
//if(midi_send_out[vfader]==1){ index_send_midi_out[vfader]=1;}
if(lfo_mode_is[vfader]==1 || lfo_mode_is[vfader]==2 || lfo_cycle_is_on[vfader]==1)
{
lfo_mode_is[vfader]=0; lfo_mode_is[vfader]=0; lfo_cycle_is_on[vfader]=0;
}
break;
case 2://affectation au speed d un fader
vfader=arduino_analog_attribution_input[p];
lfo_speed[vfader]=analog_data_from_arduino[p]/2;
if(lfo_speed[vfader]<0){lfo_speed[vfader]=0;}
if(lfo_speed[vfader]>127){lfo_speed[vfader]=127;}
midi_levels[196+vfader]=lfo_speed[vfader];
index_send_midi_out[196+vfader]=1;
break;
case 3://Damper decay
vfader=arduino_analog_attribution_input[p];
Fader_dampered[vfader].set_damper_decay(((float)(analog_data_from_arduino[p]))/255);
midi_levels[1960+vfader]=127-(analog_data_from_arduino[p]/2);
index_send_midi_out[1960+vfader]=1;
break;
case 4://damper delta
vfader=arduino_analog_attribution_input[p];
Fader_dampered[vfader].set_damper_dt((((float)(analog_data_from_arduino[p]))/255)*DAMPER_DT_MAX);// [fader 16 bit] dt 0..DAMPER_DT_MAX
midi_levels[2056+vfader]=127-(analog_data_from_arduino[p]/2);
index_send_midi_out[2056+vfader]=1;
break;
case 5://grand master
//NIVEAU
if(index_allow_grand_master==1)
{
niveauGMaster=wc::dmx8_to_lvl(analog_data_from_arduino[p]); // [GM 16 bit] analog 8 bit -> 16 bit (x257)
midi_levels[615]=(wc::lvl_to_dmx8(niveauGMaster)/2);
if(midi_send_out[615]==1){ index_send_midi_out[615]=1;}
}
break;
case 6://sequenciel : Val 1 stage  Val 2 preset Val 3 speed
if(arduino_analog_attribution_input[p]==0 || arduino_analog_attribution_input[p]==1 )//traitement du crossfade manuel
{
// stage
if(arduino_analog_attribution_input[p]==0 )
{
temp_ardniveauX1=analog_data_from_arduino[p];
if(index_get_back_faders_need_to_be_done==0)
{
index_go=0; index_go_back=0; index_pause=0;
niveauX1=wc::dmx8_to_lvl(analog_data_from_arduino[p]); // [crossfade 16 bit] analog 8 bit -> 16 bit (x257)
midi_levels[491]=wc::lvl_to_dmx8(niveauX1)/2;
if(midi_send_out[491]==1){index_send_midi_out[491]=1;}
}
}
// preset
if(arduino_analog_attribution_input[p]==1 )
{
temp_ardniveauX2=255-analog_data_from_arduino[p];
if(index_get_back_faders_need_to_be_done==0)
{
index_go=0; index_go_back=0; index_pause=0;
niveauX2=wc::dmx8_to_lvl(255-analog_data_from_arduino[p]); // [crossfade 16 bit] analog 8 bit -> 16 bit (x257)
midi_levels[492]=analog_data_from_arduino[p]/2;//inversion faite dans le midi
if(midi_send_out[492]==1){index_send_midi_out[492]=1;}
}
}

//remise à jour du crossfade
if(temp_ardniveauX1==0 && temp_ardniveauX2==255)
{
index_go=0; index_go_back=0; index_pause=0;
next_mem_crossfade_finished(position_preset);
index_x1_x2_together=1;
index_get_back_faders_need_to_be_done=1;
refresh_integrated_gridplayer1();
}
if(temp_ardniveauX1==255 && temp_ardniveauX2==0)
{
index_get_back_faders_need_to_be_done=0;
}
}
// speed
if(arduino_analog_attribution_input[p]==2)
{
crossfade_speed=analog_data_from_arduino[p]/2;
//if(midi_send_out[493]==1){index_send_midi_out[493]=1;}
prepare_crossfade();
//ventilation_temps_vers_char();//icat
someone_changed_in_time_sequences=1;//icat
}
break;
case 7:
 draw_arduino_xy[0]=analog_data_from_arduino[p];
 do_arduino_draw_input();
break;
case 8:
 draw_arduino_xy[1]=analog_data_from_arduino[p];
 do_arduino_draw_input();
break;
case 9:
 draw_arduino_xy[2]=analog_data_from_arduino[p];
 do_arduino_draw_input();
break;
case 10:
 draw_arduino_xy[3]=analog_data_from_arduino[p];
 do_arduino_draw_input();
break;
case 11://chaser track
vfader=arduino_analog_attribution_input[p];
if(vfader>=0 && vfader<24)
{
track_level[chaser_selected][vfader]=analog_data_from_arduino[p]/2;
midi_levels[1023+vfader]=analog_data_from_arduino[p]/2;
midi_levels[998+vfader]=127*track_is_on[chaser_selected][vfader];
index_send_midi_out[1023+vfader]=1;
index_send_midi_out[998+vfader]=1;
}
break;
case 12://MIDI CH0
simulate_midi(4,0,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 13://MIDI CH1
simulate_midi(4,1,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 14://MIDI CH2
simulate_midi(4,2,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 15://MIDI CH3
simulate_midi(4,3,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 16://MIDI CH4
simulate_midi(4,4,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 17://MIDI CH5
simulate_midi(4,5,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 18://MIDI CH6
simulate_midi(4,6,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 19://MIDI CH7
simulate_midi(4,7,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 20://MIDI CH8
simulate_midi(4,8,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 21://MIDI CH9
simulate_midi(4,9,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 22://MIDI CH10
simulate_midi(4,10,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 23://MIDI CH11
simulate_midi(4,11,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 24://MIDI CH12
simulate_midi(4,12,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 25://MIDI CH13
simulate_midi(4,13,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 26://MIDI CH14
simulate_midi(4,14,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
case 27://MIDI CH15
simulate_midi(4,15,arduino_digital_function_input[p][1],analog_data_from_arduino[p]/2);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Ctrl-Chge");
break;
default:
break;

}

}
previous_analog_data_from_arduino[p]=analog_data_from_arduino[p];
}
//mise en boite des datas

}

return(0);
}



int arduino_do_digital_in_whitecat()
{
int vfader=0;

for(int p=0;p<=arduino_max_digital;p++)
{

if((arduino_digital_type[p]==1||arduino_digital_type[p]==2||arduino_digital_type[p]==7)  && digital_data_from_arduino[p]!= previous_digital_data_from_arduino[p] )//si l'affectation est type INPUT
//et qu il y a une différence d'état
{
switch(arduino_digital_function_input[p][0])
{
case 0://desafected
break;
case 1: //fader up
if(digital_data_from_arduino[p]==1) {
vfader=(arduino_digital_function_input[p][1]-1);
start_time_for_delays[vfader]=actual_time;
if(lfo_mode_is[vfader]!=1)//up
{
lfo_mode_is[vfader]=1;
faders_in_float[vfader]=Fader[vfader];
if(index_midi_mute_on_lfo==1){is_raccrochage_midi_remote[vfader]=1;}
}
else {lfo_mode_is[vfader]=0;}
lfo_cycle_is_on[vfader]=0;
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}

break;
case 2://fader down
if(digital_data_from_arduino[p]==1)
{
vfader=(arduino_digital_function_input[p][1]-1);
start_time_for_delays[vfader]=actual_time;
if(lfo_mode_is[vfader]!=2)//down
{
lfo_mode_is[vfader]=2;
faders_in_float[vfader]=Fader[vfader];
if(index_midi_mute_on_lfo==1){is_raccrochage_midi_remote[vfader]=1;}
}
else {lfo_mode_is[vfader]=0;}
lfo_cycle_is_on[vfader]=0;
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}

break;
case 3://Fader saw
if(digital_data_from_arduino[p]==1)
{
vfader=(arduino_digital_function_input[p][1]-1);
start_time_for_delays[vfader]=actual_time;
faders_in_float[vfader]=Fader[vfader];
if(lfo_cycle_is_on[vfader]==0)//
{
lfo_cycle_is_on[vfader]=1;
lfo_mode_is[vfader]=0;
lfo_running_is_upward[vfader]=1;
if(index_midi_mute_on_lfo==1){is_raccrochage_midi_remote[vfader]=1;}
}
else if(lfo_cycle_is_on[vfader]==1)
{
lfo_cycle_is_on[vfader]=0;
}
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}
break;
case 4://Fader: ToPREVDock
if(digital_data_from_arduino[p]==1)
{
vfader=(arduino_digital_function_input[p][1]-1);
if (lfo_do_next_step[vfader][0]==0) {lfo_do_next_step[vfader][0]=1;lfo_do_next_step[vfader][1]=0;}
else if (lfo_do_next_step[vfader][0]==1) {lfo_do_next_step[vfader][0]=0; }
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}
break;
case 5://Fader: ToNEXTDock
if(digital_data_from_arduino[p]==1)
{
vfader=(arduino_digital_function_input[p][1]-1);
if (lfo_do_next_step[vfader][1]==0) {lfo_do_next_step[vfader][1]=1;lfo_do_next_step[vfader][0]=0; ;}
else if (lfo_do_next_step[vfader][1]==1) {lfo_do_next_step[vfader][1]=0; }
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}
break;
case 6://Fader: Up/Down
if(digital_data_from_arduino[p]==1)
{
vfader=(arduino_digital_function_input[p][1]-1);
lfo_cycle_steps[vfader]= toggle(lfo_cycle_steps[vfader]);
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}
break;
case 7://Fader: LOCK
if(digital_data_from_arduino[p]==1) //si appuyé
{
vfader=(arduino_digital_function_input[p][1]-1);
 if (!FaderLocked[vfader])
 {
   FaderLocked[vfader]=1;
   StateOfFaderBeforeLock[vfader]=Fader[vfader];
   if(StateOfFaderBeforeLock[vfader]==65535){LockFader_is_FullLevel[vfader]=1;}// [fader 16 bit]
   else if(StateOfFaderBeforeLock[vfader]<65535){LockFader_is_FullLevel[vfader]=0;}
   sprintf(string_Last_Order,">> LOCKED Fader %d",vfader+1);
   if(LockFader_is_FullLevel[vfader]==0)//quand mis en lock et pas full level rajout 0.7.6
   {
   lfo_mode_is[vfader]=0;
   lfo_cycle_is_on[vfader]=0;
   }
 }
 else
 {
   FaderLocked[vfader]=0;
   //remise à plat du niveau
   Fader[vfader]=(unsigned short)((((float)(StateOfFaderBeforeLock[vfader]))/65535)*locklevel)/* [fader 16 bit] */;
   midi_levels[vfader]=(int)(wc::lvl_to_dmx8(Fader[vfader])/2);// [fader 16 bit]
   sprintf(string_Last_Order,">> UNLOCKED Fader %d",vfader+1);
 }
   previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}
break;
case 8://FLASH
vfader=(arduino_digital_function_input[p][1]-1);
FaderIsFlash[vfader]=digital_data_from_arduino[p];
break;
case 9://Fader: All at 0
if(digital_data_from_arduino[p]==1) //si appuyé
{
vfader=(arduino_digital_function_input[p][1]-1);
           Fader[vfader]=0;
           midi_levels[vfader]=0;
           index_send_midi_out[vfader]=1;
           lfo_speed[vfader]=64;
           midi_levels[196+p]=64;
           index_send_midi_out[196+p]=1;
           FaderLocked[vfader]=0;
           lfo_mode_is[vfader]=0;//no lfo
           lfo_cycle_is_on[vfader]=0;//saw
           lfo_cycle_steps[vfader]=0;
           lfo_do_next_step[vfader][0]=0;
           lfo_do_next_step[vfader][1]=0;
           for(int d=0;d<6;d++)
           {
           is_dock_for_lfo_selected[vfader][d]=0;
           }
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}
break;
case 10://Fader: L/Unloop dock
if(digital_data_from_arduino[p]==1) //si appuyé
{
vfader=(arduino_digital_function_input[p][1]-1);
is_dock_for_lfo_selected[vfader][(dock_used_by_fader_is[vfader])]=toggle(is_dock_for_lfo_selected[vfader][(dock_used_by_fader_is[vfader])]);
}
digital_data_from_arduino[p]=toggle(digital_data_from_arduino[p]);
break;
case 11://Fader: L/Unloop all
if(digital_data_from_arduino[p]==1) //si appuyé
{
vfader=(arduino_digital_function_input[p][1]-1);
for(int d=0;d<6;d++)
           {
           is_dock_for_lfo_selected[vfader][d]=toggle(is_dock_for_lfo_selected[vfader][d]);
           }
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}
break;
case 12://Seq: GO
if(digital_data_from_arduino[p]==1) //si appuyé
{
do_go_function();
do_send_bang();
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);//pour ne pas faire un flicker sur le  go/pause/go
}
break;
case 13://Seq: GO BACK
if(digital_data_from_arduino[p]==1) //si appuyé
{
do_go_back_function();
int banger_back_is=Banger_Memoire[position_preset]-1;
do_go_back_bang(banger_back_is);
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);
}
break;
case 14://Seq: JUMP
if(digital_data_from_arduino[p]==1) //si appuyé
{
do_double_go_function();
do_send_bang();
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);
}
break;
case 15://Seq: SHIFT-W
if(digital_data_from_arduino[p]==1) //si appuyé
{
index_go=0;index_pause=0; index_go_back=0;
detect_mem_preset_previous();
refresh_mem_onpreset(position_preset);
niveauX2=0;
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);
someone_changed_in_sequences=1;//icat
}
break;
case 16://Seq: SHIFT-X
if(digital_data_from_arduino[p]==1) //si appuyé
{
index_go=0;index_pause=0; index_go_back=0;
detect_next_preset();
refresh_mem_onpreset(position_preset);
niveauX2=0;
previous_digital_data_from_arduino[p]=toggle(previous_digital_data_from_arduino[p]);
someone_changed_in_sequences=1;//icat
}
break;
case 17:////As Key-On CH0 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,0,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,0,arduino_digital_function_input[p][1],0);
}
break;
case 18:////As Key-On CH1 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,1,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,1,arduino_digital_function_input[p][1],0);
}
break;
case 19:////As Key-On CH2 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,2,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,3,arduino_digital_function_input[p][1],0);
}
break;
case 20:////As Key-On CH3 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,3,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,4,arduino_digital_function_input[p][1],0);
}
break;
case 21:////As Key-On CH4 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,4,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,4,arduino_digital_function_input[p][1],0);
}
break;
case 22:////As Key-On CH5 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,5,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,5,arduino_digital_function_input[p][1],0);
}
break;
case 23:////As Key-On CH6 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,6,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,6,arduino_digital_function_input[p][1],0);
}
break;
case 24:////As Key-On CH7 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,7,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,7,arduino_digital_function_input[p][1],0);
}
break;
case 25:////As Key-On CH8 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,8,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,8,arduino_digital_function_input[p][1],0);
}
break;
case 26:////As Key-On CH9 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,9,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,9,arduino_digital_function_input[p][1],0);
}
break;
case 27:////As Key-On CH10 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,10,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,10,arduino_digital_function_input[p][1],0);
}
break;
case 28:////As Key-On CH11 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,11,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,11,arduino_digital_function_input[p][1],0);
}
break;
case 29:////As Key-On CH12 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,12,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,12,arduino_digital_function_input[p][1],0);
}
break;
case 30:////As Key-On CH13 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,13,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,13,arduino_digital_function_input[p][1],0);
}
break;
case 31:////As Key-On CH14 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,14,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,14,arduino_digital_function_input[p][1],0);
}
break;
case 32:////As Key-On CH15 P:
if(digital_data_from_arduino[p]==1) //si appuyé
{
simulate_midi(1,15,arduino_digital_function_input[p][1],127);
sprintf (my_midi_string, "Arduino Chan:%-2d Pitch:%-2d Vel:%-2d Typ: %s",	ischan,ispitch,isvel, "Key-On");
simulate_midi(2,15,arduino_digital_function_input[p][1],0);
}
break;
default:
break;
}
}
}

 return(0);
}


//OLD CODE FOR ARDUINO
/*int arduino_decode_array_method(unsigned char *tmp_ard)
{
//if(strncmp(tmp_ard,"DG/",3)==0)//3= le code
if(tmp_ard[0]=='D' && tmp_ard[1]=='G' && tmp_ard[2]=='/')
{
for(int o=3;o<3+arduino_max_digital;o++)
{
digital_data_from_arduino[o-3]=(int)tmp_ard[o];
}
sprintf( string_Arduino_status,">>ARDUINO : Received Digital ");
}

//else if(strncmp(tmp_ard,"AN/",3)==0)//
else if(tmp_ard[0]=='A' && tmp_ard[1]=='N' && tmp_ard[2]=='/')//
{
for(int o=3;o<3+arduino_max_analog;o++)
{
//analog_data_from_arduino[o-3]=(int)(tmp_ard[o]) &0xff;     //cast si val=127 -> 127 si val=-1 -> val 255 si val=-128 ->val 128
analog_data_from_arduino[o-3]=(int)(tmp_ard[o]);
}
sprintf( string_Arduino_status,">>ARDUINO : Received Analogic");
}
return(0);
}*/

int new_arduino_decode_array_method(unsigned char *tmp_ard)
{
int tmp_d=0;
if(tmp_ard[0]=='W' && tmp_ard[1]=='C' && tmp_ard[2]=='/' )//Whitecat keyboard
{

for(int o=0;o<arduino_max_digital;o++)
{
tmp_d=(int)tmp_ard[o+3];
if(tmp_d>64){digital_data_from_arduino[o]=1;}
else {digital_data_from_arduino[o]=0;}
}

for(int o=0;o<arduino_max_analog;o++)
{
analog_data_from_arduino[o]=(int)(tmp_ard[3+o+arduino_max_digital])-1;
}
sprintf( string_Arduino_status,">>ARDUINO : Received Data");
}
return(0);
}

int arduino_read()
{
if(arduino_device_0_is_ignited==1)
{
//capture de l'état précédent
for(int y=0;y<=arduino_max_digital;y++)
{
previous_digital_data_from_arduino[y]=digital_data_from_arduino[y];
}

unsigned char arduino_order[8];
arduino_order[0]='S';arduino_order[1]='D';arduino_order[2]='/';arduino_order[3]='\0';


//ne pas enlever nBytes
nBytesSendtoArduino=serial0.SendData(arduino_order,4);

//absolument garder digital limit comme taille !
nBytesReadArduino0=serial0.ReadData(input_str_arduino,digital_limit);
new_arduino_decode_array_method(input_str_arduino);
}
 return(0);
}





//whitecat version 0.8.7 christoph 21/04/15
//merging in one single array ON/OFF and PWM outputs
//no backward compatibility with older sketches

int arduino_merge_and_do_data_out()//refonte en un seul tableau d'envoi DIGITAUX + PWM
{

unsigned char arduino_order[8];
arduino_order[0]='S';arduino_order[1]='D';arduino_order[2]='/';arduino_order[3]='\0';
nBytesSendtoArduino=serial0.SendData(arduino_order,4);

//unsigned char sended to Cserial, if not code is not working properly. C serial was modified as unsigned char
unsigned char temp_send_arduino[(3+arduino_max_digital+3+1)];    //header/tableau/headerfin/return
for(int i=0;i<(3+arduino_max_digital+3+1);i++)
{
    temp_send_arduino[i]=1;//pour éviter tout caractere null cassant la chaine
}

//keyword to set IO and PWM to be readen
temp_send_arduino[0]='D';temp_send_arduino[1]='O';temp_send_arduino[2]='/';

//on off threshold def
int value_dm=0;
if (!dmx_view)
{
}
else
{
}


for(int p=0;p<arduino_max_digital;p++)
{
    switch(arduino_digital_type[p])
    {
    case 2://si l'affectation est type  ON/OFF
    if(arduino_digital_function_output[p][0]==1) //CHANNEL >10
    {
    if(wc::lvl_to_dmx8(MergerArray[(arduino_digital_function_output[p][1])])>value_dm)
    {digital_data_to_arduino[p]=1;temp_send_arduino[3+p]=127;}
    else {digital_data_to_arduino[p]=0;temp_send_arduino[3+p]=32;}
    }
    else if(arduino_digital_function_output[p][0]==2 && arduino_digital_function_output[p][1]>0 && arduino_digital_function_output[p][1]<49) //Fader >10
    {
    if(Fader[(arduino_digital_function_output[p][1]-1)]>value_dm)
    {digital_data_to_arduino[p]=1;temp_send_arduino[3+p]=127;}
    else {digital_data_to_arduino[p]=0;temp_send_arduino[3+p]=32;}
    }
    break;

    case 3://si l'affectation est type  PWM
    //CHANNEL
    if(arduino_digital_function_output[p][0]==1 && arduino_digital_function_output[p][1]>0 && arduino_digital_function_output[p][1]<513)
    {
    pwm_data_to_arduino[p]=wc::lvl_to_dmx8(MergerArray[(arduino_digital_function_output[p][1])]);
    temp_send_arduino[3+p]=pwm_data_to_arduino[p];
    if(temp_send_arduino[3+p]<=0) temp_send_arduino[3+p]=1;//eviter le caractere NULL
    }
    //FADER
    else if(arduino_digital_function_output[p][0]==2 && arduino_digital_function_output[p][1]>0 && arduino_digital_function_output[p][1]<49)
    {
    pwm_data_to_arduino[p]=(int)Fader[(arduino_digital_function_output[p][1])+1];
    temp_send_arduino[3+p]=pwm_data_to_arduino[p];
    if(temp_send_arduino[3+p]<=0) temp_send_arduino[3+p]=1;//eviter le caractere NULL
    }
    break;

    default://0 or input
    temp_send_arduino[3+p]=1;//éviter de casser la chaine avec un caractere null
    break;
    }
}



temp_send_arduino[(3+arduino_max_digital+1)]='E';
temp_send_arduino[(3+arduino_max_digital+2)]='D';
temp_send_arduino[(3+arduino_max_digital+3)]='/';
temp_send_arduino[(3+arduino_max_digital+4)]='\0';

//ne pas enlever ! ENVOI A L ARDUINO
nBytesSendtoArduino=serial0.SendData(temp_send_arduino,sizeof(temp_send_arduino));


return(0);
}


/*
int arduino_do_pwm_out_whitecat()//old protocole
{
index_send_pwm_data=0;

char temp_send_pwm[(3+arduino_max_digital+3+1)];    //header/tableau/headerfin/return

if(need_send_pwm==1)
{
if(ticks_arduino_passe!=ticks_arduino+1&&ticks_arduino<ticks_arduino_passe+2)
{
need_send_pwm=1;
index_send_pwm_data=0;
}
if(ticks_arduino>ticks_arduino_passe+2)
{
need_send_pwm=0;
index_send_pwm_data=1;
}
}

for(int p=0;p<=arduino_max_digital;p++)
{
if(arduino_digital_type[p]==6 || arduino_digital_type[p]==4|| arduino_digital_type[p]==5)//si l'affectation est type OUTPUT   PWM
{
//CHANNEL
if(arduino_digital_function_output[p][0]==1 && arduino_digital_function_output[p][1]>0 && arduino_digital_function_output[p][1]<513)
{
pwm_data_to_arduino[p]=wc::lvl_to_dmx8(MergerArray[(arduino_digital_function_output[p][1])]);
}
//FADER
else if(arduino_digital_function_output[p][0]==2 && arduino_digital_function_output[p][1]>0 && arduino_digital_function_output[p][1]<49)
{
pwm_data_to_arduino[p]=(int)Fader[(arduino_digital_function_output[p][1])-1];
}

//if(index_send_pwm_data==0){index_send_pwm_data=1;}
if(pwm_data_to_arduino[p]!=previous_pwm_data_to_arduino[p]|| need_send_pwm==1){
                                                       index_send_pwm_data=1;}

if(need_send_pwm==0)
{previous_pwm_data_to_arduino[p]=pwm_data_to_arduino[p];}

}
}

if(index_send_pwm_data==1 && need_send_pwm==0)
{
sprintf(temp_send_pwm,"%s","");
temp_send_pwm[0]='P';temp_send_pwm[1]='W';temp_send_pwm[2]='/';

for(int k=0;k<=arduino_max_digital;k++)
{
if(pwm_data_to_arduino[k]<255)
{
temp_send_pwm[3+k]=pwm_data_to_arduino[k]+1;//+1 pour éviter caractere 0
}
else {temp_send_pwm[3+k]=255;}
}
temp_send_pwm[(3+arduino_max_digital+1)]='E';
temp_send_pwm[(3+arduino_max_digital+2)]='D';
temp_send_pwm[(3+arduino_max_digital+3)]='/';
temp_send_pwm[(3+arduino_max_digital+4)]='\0';

//ne pas enlever !
nBytesSendtoArduino=serial0.SendData(temp_send_pwm,sizeof(temp_send_pwm));
}

 return(0);
}



int arduino_do_digital_out_whitecat()
{
index_send_digital_data=0;
char temp_send[(3+arduino_max_digital+3+1)];    //header/tableau/headerfin/return
int value_dm=0;
if (!dmx_view)
{
     value_dm=(int)(10*2.55);
}
else
{
     value_dm=10;
}

for(int p=0;p<=arduino_max_digital;p++)
{
previous_digital_data_to_arduino[p]=digital_data_to_arduino[p];
if(arduino_digital_type[p]==3 )//si l'affectation est type OUTPUT
{
if(arduino_digital_function_output[p][0]==1) //CHANNEL >10
{
if(wc::lvl_to_dmx8(MergerArray[(arduino_digital_function_output[p][1])])>value_dm)

{digital_data_to_arduino[p]=1;}
else {digital_data_to_arduino[p]=0;}

if(digital_data_to_arduino[p]!=previous_digital_data_to_arduino[p])
{need_send_pwm=1;index_send_digital_data=1;ticks_arduino_passe=ticks_arduino;
previous_digital_data_to_arduino[p]=digital_data_to_arduino[p]; }

}

else if(arduino_digital_function_output[p][0]==2 && arduino_digital_function_output[p][1]>0 && arduino_digital_function_output[p][1]<49) //Fader >10
{
//previous_digital_data_to_arduino[p]=digital_data_to_arduino[p];

if(Fader[(arduino_digital_function_output[p][1]-1)]>value_dm)
{digital_data_to_arduino[p]=1;}
else {digital_data_to_arduino[p]=0;}

if(digital_data_to_arduino[p]!=previous_digital_data_to_arduino[p])
{need_send_pwm=1;index_send_digital_data=1;ticks_arduino_passe=ticks_arduino;
previous_digital_data_to_arduino[p]=digital_data_to_arduino[p]; }

//index_send_digital_data=1;
//if(digital_data_to_arduino[p]!=previous_digital_data_to_arduino[p]){index_send_digital_data=1;}
}

}
}

if(index_send_digital_data)
{
sprintf(arduino_order,"%s","");
temp_send[0]='D';temp_send[1]='O';temp_send[2]='/';

for(int k=0;k<=arduino_max_digital;k++)
{
if (!digital_data_to_arduino[k])
{
temp_send[3+k]=32;
}
else
{
temp_send[3+k]=127;
break ;
}
}
temp_send[(3+arduino_max_digital+1)]='E';
temp_send[(3+arduino_max_digital+2)]='D';
temp_send[(3+arduino_max_digital+3)]='/';
temp_send[(3+arduino_max_digital+4)]='\0';

//ne pas enlever
if(need_send_pwm==1)
{nBytesSendtoArduino=serial0.SendData(temp_send,sizeof(temp_send));}

}

 return(0);
}

*/




