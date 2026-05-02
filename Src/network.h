#pragma once

/////////////////ARTNET//////////////////////////////////////////////////////////
const short MaxNumPorts = 1;
const short MaxExNumPorts = 32;
const short ShortNameLength = 18;
const short LongNameLength = 64;
const short NodeReportLength = 64;
const short PortNameLength = 32;
const short MaxDataLength = 512 - 1;

char ArtPollBuffer[14];
char ArtPollReplyBuffer[240];
char ReceivedArtPollBuffer[14];
char ReceivedArtPollReplyBuffer[240];
char ArtShortName[ShortNameLength] = {"white_cat"};
char ArtLongName[LongNameLength] = {"a PC Lighting Application "};
char ArtNodeReport[NodeReportLength] = {"Is everything ok ?"};
int index_broadcast = 0;

int count_artopoll_received = 0;
bool receiving_bytes = 0;
bool ArtDetected = 0;
bool is_artnet = 0;
bool is_opcode_is_dmx = 0;
bool is_opcode_is_polling = 0;
int is_artnet_version_i1, is_artnet_version_i2;
int seq_artnet = 0;
int artnet_physical = 0;
int incoming_universe;
char artnet_message[530];
char artpollreply_message[250];
unsigned char ArtNet_16xUniverse_Receiving[514][17];
bool index_serveur_artnet_on = 0;
bool index_listen_for_artnet = 1;
bool index_show_artpoll_reply_content = 0;
int artpoll_replyX = 485, artpoll_replyY = 110;
char PollReplyIs[16][100];
char nodefirmware_versinfo[12];
char shortname_device[18];
char subnetis[6];
char paste_reply[16][100];
char ip_artnet[17];
char string_ip[30];
char my_ip_is[4];

// client
SOCKET sockartnet;
SOCKADDR_IN sinS;
int sinsize;
// serveur
SOCKET sock;
SOCKADDR_IN sinServ;
int sinsizeServ;

struct hostent *phe;
char FAR hostnamebuffer[64];
char broadcast = '1';
int nbrbytessended = 0;
int bytesreceived = 0;
int serveurport_artnet = 6454;
int clientport_artnet = 6454;

char tmp_udp_chain[600];
short HeaderLength = 17;
short DataLength = 512;
int Dim(HeaderLength + DataLength);
int Univers = 0;
bool index_do_light_diode_artnet = 0;
bool artnet_serveur_is_initialized = 0;

//////////////////RESEAUX DETECTION////////////////////////////////////////////
char IP_detected_dmxOUT[8][24];
char IP_artnet_IN[24];
char IP_artnet_OUT[24];
char IP_fantastick[24];
int network_OUT_is_selected = 0;
bool index_re_init_client_artnet = 0;
bool index_re_init_serveur_artnet = 0;

char descriptif_network_adapter[8][256];
