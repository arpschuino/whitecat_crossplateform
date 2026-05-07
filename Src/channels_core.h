#pragma once
int snap_channels_selection_array();
int channel_copy();
int channel_paste();
int reset_blind();
int Channel_select_all();
int Channel_select_inv();
int Channel_select_thruth(int fromch, int toch);
int Channel_at_level();
int DoMouseLevel();
int do_logical_ChannelScroller(int ScrollX, int ScrollY);
int do_logical_Draw_Channel_Preset_View(int xchan, int ychan, int prst_v);
int do_logical_Draw_Channel_Preset_Title(int xchan, int ychan, int prst_v);
int do_logical_ClassicalChannelSpace(int xchan, int ychan, int scroll);
int do_logical_ChannelsMenuSelection(int chx, int chy);
