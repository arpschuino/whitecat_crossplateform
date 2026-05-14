#pragma once
#include "opencv2/opencv.hpp"

extern CvCapture* g_capture;
extern IplImage* frame;
extern IplImage* affichage;
extern IplImage* onech_temoin_prev;
extern IplImage* onech_temoin;
extern IplImage* onech_difference;
extern IplImage* onech_visu;
extern IplImage* onech_snap_background;
extern CvCapture* file_capture;
extern IplImage* frame_played;

int set_default_image_size();
int Load_Video_Conf();
int Save_Video_Conf();
int set_camera_size();
int set_image_caches();
int InitVideo();
int CloseVideo();
int Snap_Background();
int Filter_Image(int xorigin, int yorigin, int largeurRoi, int hauteurRoi);
int do_logical_Move_Tracker(int dock_selected, int tracker_selected);
int do_logical_Edit_Tracker_Size(int dock_selected, int tracker_selected);
int ventilation_video_trackers();
int Trackers();
int do_logical_Set_Filtering_Image();
int show_who_is_in_ROI_as_channel(int roipreset, int dock_tracker_selected);
int do_logical_Interface_video_window(int WindowVideoX, int WindowVideoY);
