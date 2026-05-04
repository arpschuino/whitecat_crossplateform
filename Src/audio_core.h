#pragma once

int Load_audiofiles_cues();
int AffectSoundFile(int player);
int do_audio_midi_function_next_prev_track();
int sound_core_processing();
int Control_Audio_thruth_faders(int ff, int dd, int typ);
void ShowSupportedAudioDevices();
int InitSound();
int do_logical_fader_niveau_son(int xp, int yp, int numero);
int do_logical_lecteur_audio(int xp, int yp, int numero);
int do_logical_fenetre_audio(int xb, int yb);
