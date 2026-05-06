#pragma once

extern char ArtDmxBuffer[530];

int diodes_artnet(int x_diods, int y_diods);
int light_temoin_universe(int incoming_artnet, int x_diods, int y_diods);
int light_temoin_emission(int outgoing_artnet, int x_diods, int y_diods);
int reset_poll_list();
int show_artpoll_reply(int apr_X, int apr_Y);
int init_artnet_variables();
int ReceiveArtDmx();
int load_artnet_conf();
int save_artnet_conf();
int detection_reseaux();
int initialisation_serveur_artnet();
int fermeture_serveur_artnet();
int detection_mise_en_place_carte_reseaux();
int initialisation_client_artnet();
int fermeture_client_artnet();
int ConstructArtPoll();
int ArtDmx();
int ConstructArtPollReply();
int AnalyseArtPollReply();
