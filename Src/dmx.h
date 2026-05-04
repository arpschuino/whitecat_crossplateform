#pragma once

////////////////////DMX/////////////////////////////////////////////////////////
extern unsigned char DmxBlock[514];
extern unsigned char DmxBlockPatch[514];
extern unsigned char artnet_backup[514];
extern bool do_send_on_change;

extern bool client_artnet_is_closed;
extern int myDMXinterfaceis;
extern bool index_init_dmx_ok;

///////////////DMX ENTTEC PRO//////////////////////////////////////////////////
extern int enttecpro_detectX, enttecpro_detectY;
extern bool index_list_pro_devices;
extern char DevFoundedEnttecPro[64];
extern int number_of_enttec_pro_devices;
extern bool index_init_EnttecPROIN_ok;
extern int istheresomeone_in_enttecpro;
extern int vcom_inposition_is;

///////////////////FREEZE//////////////////////////////////////////////////////////
extern bool freeze_array[514];
extern unsigned char freeze_state[514];

///////////GRAND MASTER////////////////////////////////////////////////////////
extern int niveauGMaster;
extern int previous_niveauGMaster;
extern char string_niveauGMaster[4];
extern bool index_allow_grand_master;

// DMX interfaces
extern int dmx_interface_active[5];
extern bool index_allow_sunlite_dmxIN;
extern bool index_is_siudi_8C;
extern char string_sunlite_is[32];
extern bool do_send_dmx_on_change;
extern bool do_send_dmx_on_change_siudi;
extern bool index_patch_overide;
extern bool patch_overide[513];
extern bool allow_artnet_in;
