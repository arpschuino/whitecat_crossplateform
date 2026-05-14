#pragma once
int init_kbd_custom();
int do_keyboard_config(int x_cfg, int y_cfg, int largeur_cfg, int hauteur_cfg);
int do_keyboard_conf(int cfgnetw_X, int cfgnetw_Y);
int do_core_config(int x_cfg_sc, int y_cfg_sc, int largeur_cfg_sc, int hauteur_cfg_sc);
int Box_artnet_udpport(int macx, int macy);
int do_network_config(int x_cfg_sc, int y_cfg_sc, int largeur_cfg_sc, int hauteur_cfg_sc);
int do_main_config(int cfgnetw_X, int cfgnetw_Y, int largeurCFGdmxwindow, int hauteurCFGdmxwindow);
int SelectDmxDevice(int interfacedmx);
int affect_dmx_in(int callb_x, int callb_y);
int do_dmx_config(int cfgdmx_X, int cfgdmx_Y, int largeurCFGdmxwindow, int hauteurCFGdmxwindow);
int save_network_settings();
int load_network_conf();
int do_panel_config(int cfg_X, int cfg_Y);
int config_general_menu();
