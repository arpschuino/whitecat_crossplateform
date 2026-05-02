#pragma once

////////////////////DMX/////////////////////////////////////////////////////////
unsigned char DmxBlock[514];
unsigned char DmxBlockPatch[514];
unsigned char artnet_backup[514];
bool do_send_on_change = 0;

bool client_artnet_is_closed = 0;
int myDMXinterfaceis = 0;   // 0=no device 1=Artnet 2=Enttec Open 3=Enttec PRO 4=Sunlite
bool index_init_dmx_ok = 0;

///////////////DMX ENTTEC PRO//////////////////////////////////////////////////
int enttecpro_detectX = 485, enttecpro_detectY = 110;
bool index_list_pro_devices = 0;
char DevFoundedEnttecPro[64];
int number_of_enttec_pro_devices = 0;
bool index_init_EnttecPROIN_ok = 0;
int istheresomeone_in_enttecpro = 0;
int vcom_inposition_is = 0;

///////////////////FREEZE//////////////////////////////////////////////////////////
bool freeze_array[514];
unsigned char freeze_state[514];

///////////GRAND MASTER////////////////////////////////////////////////////////
int niveauGMaster = 255;
int previous_niveauGMaster = 0;
char string_niveauGMaster[4];
bool index_allow_grand_master = 1;

// DMX interfaces
int dmx_interface_active[5] = {0, 0, 0, 0, 0}; // indices 1-4: ArtNet, EnttecOpen, EnttecPro, Sunlite
bool index_allow_sunlite_dmxIN = 0;
bool index_is_siudi_8C = 0;
char string_sunlite_is[32];
bool do_send_dmx_on_change = 0;
bool do_send_dmx_on_change_siudi = 0;
bool index_patch_overide = 0;
bool patch_overide[513];
bool allow_artnet_in = 0;
