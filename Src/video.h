/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
       WWWWWWWWWWWWWWW           |
       WWWWWWWWWWWWWWW           |               2026       Jacques Bouault - arpschuino.fr
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

#pragma once

////////////////////////VIDEO //////////////////////////////////////////////////
double image_recording_size;
double image_recorded_size;
int fps_video_rate = 30;
int default_fps_video_rate = 12;
int recup_val_pix_video = 0;
bool ocvfilter_is_on = 0; // acces reglages images oCV
int ocv_calcul_mode = 0;
char string_ocv_mode[8];
int threshold_level = 65;
int erode_level = 0;
int div_facteur = 1;
char string_threshold_is[24];
char string_erode_is[24];
char string_blur_is[24];
char string_div_is[24];
int flip_image = 1;
int threshold_on = 1;
int erode_mode = 0;
int blur_on = 0;
double pixels_changed = 0;
double old_pixels_changed = 0;
double nbre_pixels_changed = 0;
float ratio_pixels_changed = 0.0;

int camera_size_settings_is = 0; // nouveau système selection de taille image
int camera_size_array[2][2];     // x y size à définir 320x240 640x480
int camera_fps_settings_is = 0;  // nouveau systeme selection fps en index
bool manipulating_camera = 0;    // pour éviter de planter lectrue ecriture de données

int camera_modes_and_settings[8][16]; // ocv_calcul_mode /levels
float level_visu = 1.0;
int index_count_trackers = 0;
int frame_video_x, frame_video_y;
int video_size_x = 320, video_size_y = 200;
bool camera_is_on = 0;
int camera_on_open = 0;
int camera_original_fps_is = 15;
float display_fps;
// 6 tracking docks // 12 espaces de tracking par tracking dock//
int tracking_coordonates[6][12][4]; // dock selected / tracker / x y largeur x largeur y
bool tracking_contents[6][12][512]; // channel affectation
int buffer_tracker[514];
int tracker_level[6][12];
int tracker_to_edit = 0;
// smooth
float tracker_target_val[6][12];
float tracker_val[6][12];
float tracker_decay_constant = 1;
float tracker_dt = 1;
int index_decay_tracker = 10;
bool edit_tracker = 0; // size
bool move_tracker = 0; // x y
bool tracker_clearmode = 0;
bool view_levels_tracker = 1;
int videoX = 860, videoY = 20;
int default_videoX = 860, default_videoY = 20;
char string_tracker_edited_dat[48];
// tracking cam
// 6 tracking docks // 12 espaces de tracking par tracking dock//
int tracking_dock_selected = 0;
bool tracking_spaces_on_off[6][12]; // dock selected / tracker

////////////////////VIDEO AVI///////////////////////////////////////////////////
char list_my_video[25][16];   // 24 videos
char annote_my_video[25][64]; // 24 videos
int nbre_de_videos = 0;
