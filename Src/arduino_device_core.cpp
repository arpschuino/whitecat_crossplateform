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

#include "serial.cpp"
CSerial serial0;

int arduino_init(int device)
{
if(device==0)
{
if(serial0.Open(arduino_com0,arduino_baud_rate0))
{
sprintf(string_Arduino_status,">> Open Arduino on PORT %d rate: %d >> OK !",arduino_com0,arduino_baud_rate0);
arduino_device_0_is_ignited=1;
//rest(1000); //temps d'initialisation avant communication ( sinon pense recuperer script)
}
else
{
sprintf(string_Arduino_status,">> Open Arduino on PORT %d >> FAILED !",arduino_com0);
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
