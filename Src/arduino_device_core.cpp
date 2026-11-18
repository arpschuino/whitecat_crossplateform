/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
       WWWWWWWWWWWWWWW           |
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

/**

* \file arduino_device_core.cpp
* \brief {arduino serial open close fonction}
* \author Christoph Guillermet
* \version {0.8.7}
* \date {21/04/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Gestion des fonctions arduino sur le port série VCOM à travers l'usb pour l'ouverture l'initialistaion et la fermeture du port VCOM
*
*   Arduino transfert fonctions to usb via the serial VCOM PORT for opening the port, close it or initialise it.
*
 **/

#include "wc_tus.h"
#include "Serial.h"

#ifdef _WIN32
#include "serial.cpp"
#endif
CSerial serial0;

int arduino_init(int device)
{
if(device==0)
{
if(serial0.Open(arduino_com0,arduino_baud_rate0))
{
#ifdef _WIN32
sprintf(string_Arduino_status,">> Open Arduino on PORT %d rate: %d >> OK !",arduino_com0,arduino_baud_rate0);
#else
snprintf(string_Arduino_status,sizeof(string_Arduino_status),">> Open Arduino on PORT %d (%s) rate: %d >> OK !",arduino_com0,serial0.PortName(),arduino_baud_rate0);
#endif
arduino_device_0_is_ignited=1;
//rest(1000); //temps d'initialisation avant communication ( sinon pense recuperer script)
}
else
{
#ifdef _WIN32
sprintf(string_Arduino_status,">> Open Arduino on PORT %d >> FAILED !",arduino_com0);
#else
// Linux/macOS : PORT n = n-ieme port serie USB -> on liste ceux qui existent
char ports[WC_SERIAL_MAX_PORTS][64];
int nports=wc_serial_list_ports(ports,WC_SERIAL_MAX_PORTS);
if(nports==0)
{
snprintf(string_Arduino_status,sizeof(string_Arduino_status),">> Open Arduino on PORT %d >> FAILED ! No USB serial port found",arduino_com0);
}
else
{
int len=snprintf(string_Arduino_status,sizeof(string_Arduino_status),">> PORT %d FAILED ! Ports:",arduino_com0);
for(int i=0;i<nports && len<(int)sizeof(string_Arduino_status);i++)
{
len+=snprintf(string_Arduino_status+len,sizeof(string_Arduino_status)-len," %d=%s",i+1,ports[i]+5);//sans "/dev/"
}
}
#endif
arduino_device_0_is_ignited=0;
}
}
return(0);
}

int arduino_close(int device)
{
if(device==0)
{
 serial0.Close();
 sprintf(string_Arduino_status,">> Closed Arduino on PORT %d >> OK !",arduino_com0);
}

return(0);
}
