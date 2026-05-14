#pragma once

// Forward declarations pour autonomie (eviter d'inclure audio_backend.h complet)
namespace audiere {
    struct WCAudioDevice;
    struct WCAudioPlayer;
    typedef WCAudioDevice *AudioDevicePtr;
    typedef WCAudioPlayer *OutputStreamPtr;
}

////SOUND/////////////////////////////////////////////////////////////////////
extern audiere::AudioDevicePtr device;

extern audiere::OutputStreamPtr player1;
extern audiere::OutputStreamPtr player2;
extern audiere::OutputStreamPtr player3;
extern audiere::OutputStreamPtr player4;

extern char audio_device_name[256];
extern char list_audio_device[16][256];
extern bool index_show_audio_window;
extern int index_nbre_players_visibles;
extern bool index_over_audio;
extern bool index_click_move_audio_window;
extern int XAudio;
extern int YAudio;
extern char list_audio_files[128][72];
extern int line_audio;
extern int audiofile_selected;
extern char audiofile_name[72];
extern char rep_audio[256];
extern char audio_folder[64];
extern char list_audio_folders[64][64];
extern int nbre_audio_folders;
extern int audio_folder_list_scroll;
extern bool audio_folder_scroll_dragging;
extern int audio_folder_scroll_drag_start_y;
extern int audio_folder_scroll_drag_start_scroll;
extern bool audio_filelist_scroll_dragging;
extern int audio_filelist_scroll_drag_start_y;
extern int audio_filelist_scroll_drag_start_scroll;
extern int last_scroll_mouse_for_audio;
extern bool audio_seekbar_dragging[4];
extern volatile int mouse_double_click;
extern bool audio_pan_dragging[4];
extern bool audio_pitch_dragging[4];
extern bool index_show_audio_folder_list;
extern char temp_audio_folder[25];
extern char sound_files[4][72];
extern char soundfile_temp_loader[256 + 72];
extern int index_preloaded_sounds;
extern int audio_ram_limit_mb;

extern int length_of_file_in_player[4];
extern int position_of_file_in_player[4];
extern int player_loop_out_position[4];
extern int player_seek_position[4];

extern bool audio_do_load_midi_prev_file[4];
extern bool audio_do_load_midi_next_file[4];

extern int audiofile_cue_in_out_pos[128][4][2];

extern int audio_number_total_in_folder;

extern bool player_ignited[4];
extern bool player_is_playing[4];
extern bool player_is_onloopCue[4];
extern bool player_is_onloop[4];

extern int player_pitch[4];
extern float show_pitch_value[4];
extern int player_pan[4];
extern float show_pan_value[4];

extern int player_niveauson[4];
extern float show_player_niveauson[4];

extern int sample_rate[4];
extern int sample_nbr_ch[4];
extern bool index_edit_audio;
extern int audio_rate[4];
extern bool index_loading_a_sound_file;

extern bool midi_show_flash_seektouch[4];
extern bool midi_show_flash_backwardtouch[4];
extern bool midi_show_flash_forwardtouch[4];
extern bool midi_show_flash_cueIntouch[4];
extern bool midi_show_flash_cueOuttouch[4];
extern bool midi_show_flash_cueSeektouch[4];

extern char audiofile_name_was[4][72];
extern int audio_position_was[4];
extern int audio_volume_was[4];
extern int audio_pan_was[4];
extern int audio_pitch_was[4];
extern int audio_cue_in_was[4];
extern int audio_cue_out_was[4];
extern bool player_was_playing[4];
extern bool player_was_onloopCue[4];
extern bool player_was_onloop[4];

extern bool index_affect_audio_to_dock;
extern int player_to_affect_to_dock;
extern int audio_type_for_dock_affectation_is;
extern bool index_do_audio_to_dock;
extern int DockHasAudioVolume[48][6];
extern int DockHasAudioPan[48][6];
extern int DockHasAudioPitch[48][6];
extern int Player_is_piloted_by_fader[4][4];

extern char time_is_for_fileCueIn[4][64];
extern char time_is_for_fileCueOut[4][64];
extern char time_is_for_filePos[4][64];
extern char time_is_for_fileTotal[4][64];
extern char string_niveauson[4][16];
extern char string_pitch[4][12];
extern char string_pan[4][12];
extern char string_fader_stop_pos[48][4];
extern bool audio_autoload[4];
extern bool audio_autopause[4];
extern int player_has_file_coming_from_pos[4];
extern int player_position_on_save[4];
