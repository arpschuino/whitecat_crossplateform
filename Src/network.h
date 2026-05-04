#pragma once

/////////////////ARTNET//////////////////////////////////////////////////////////
const short MaxNumPorts = 1;
const short MaxExNumPorts = 32;
const short ShortNameLength = 18;
const short LongNameLength = 64;
const short NodeReportLength = 64;
const short PortNameLength = 32;
const short MaxDataLength = 512 - 1;

extern char ArtPollBuffer[14];
extern char ArtPollReplyBuffer[240];
extern char ReceivedArtPollBuffer[14];
extern char ReceivedArtPollReplyBuffer[240];
extern char ArtShortName[ShortNameLength];
extern char ArtLongName[LongNameLength];
extern char ArtNodeReport[NodeReportLength];
extern int index_broadcast;

extern int count_artopoll_received;
extern bool receiving_bytes;
extern bool ArtDetected;
extern bool is_artnet;
extern bool is_opcode_is_dmx;
extern bool is_opcode_is_polling;
extern int is_artnet_version_i1, is_artnet_version_i2;
extern int seq_artnet;
extern int artnet_physical;
extern int incoming_universe;
extern char artnet_message[530];
extern char artpollreply_message[250];
extern unsigned char ArtNet_16xUniverse_Receiving[514][17];
extern bool index_serveur_artnet_on;
extern bool index_listen_for_artnet;
extern bool index_show_artpoll_reply_content;
extern int artpoll_replyX, artpoll_replyY;
extern char PollReplyIs[16][100];
extern char nodefirmware_versinfo[12];
extern char shortname_device[18];
extern char subnetis[6];
extern char paste_reply[16][100];
extern char ip_artnet[17];
extern char string_ip[30];
extern char my_ip_is[4];

// client
extern SOCKET sockartnet;
extern SOCKADDR_IN sinS;
extern int sinsize;
// serveur
extern SOCKET sock;
extern SOCKADDR_IN sinServ;
extern int sinsizeServ;

extern struct hostent *phe;
extern char FAR hostnamebuffer[64];
extern char broadcast;
extern int nbrbytessended;
extern int bytesreceived;
extern int serveurport_artnet;
extern int clientport_artnet;

extern char tmp_udp_chain[600];
extern short HeaderLength;
extern short DataLength;
extern int Dim;
extern int Univers;
extern bool index_do_light_diode_artnet;
extern bool artnet_serveur_is_initialized;

//////////////////RESEAUX DETECTION////////////////////////////////////////////
extern char IP_detected_dmxOUT[8][24];
extern char IP_artnet_IN[24];
extern char IP_artnet_OUT[24];
extern int network_OUT_is_selected;
extern bool index_re_init_client_artnet;
extern bool index_re_init_serveur_artnet;

extern char descriptif_network_adapter[8][256];
