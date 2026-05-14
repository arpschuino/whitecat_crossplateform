#pragma once
int indicate_wich_fader_is_the_highest();
int asservissement_gridplayer(int cmptfader, int dk);
int DoLock(int masterfader, int locklevel);
int do_logical_fader_damper_commands(int _x, int _y, int fd);
int do_logical_Lock_Preset_Call(int xf, int yf);
int do_logical_lfo_fader_functions(int cmptfader, int x, int y, int largeur, int espacement);
int do_logical_MoveFaderSpace(int ydelimitation);
int do_logical_Touche_Midi_Mute(int xmute, int ymute);
int do_logical_FaderSpace(int x, int y, int largeur, int espacement, int nbr_fader);
