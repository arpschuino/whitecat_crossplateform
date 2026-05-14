#pragma once
int do_midi_call_order(int control);
int do_midi_job(int control);
int ventilate_midi_to_midiarray();
int ventilation_midi_sur_crossfade();
int simulate_midi(int letype, int lechan, int lepitch, int lavaleur);
void process_midi_input();
int PrintSlotsInfosDevices(short driverRef);
int midi_change_signal(int xrep, int yrep);
int midi_list_and_choose_devices();
int midi_historic_part(int _x, int _y);
int midipage_affectation(int cfg_midiX, int cfg_midiY, int largeurCFGmidi, int hauteurCFGmidi);
int refresh_launchpad_color_callibration();
int midi_launchpad_colors(int cfg_midiX, int cfg_midiY);
int clock_wheel(int xw, int yw, int rayon_k, float angle_correction);
int midi_clock_part(int _x, int _y);
int midipage_preset_and_options(int cfg_midiX, int cfg_midiY, int largeurCFGmidi, int hauteurCFGmidi);
int midipage_devices(int cfg_midiX, int cfg_midiY, int largeurCFGmidi, int hauteurCFGmidi);
int do_midi_config(int cfg_midiX, int cfg_midiY, int largeurCFGmidi, int hauteurCFGmidi);
