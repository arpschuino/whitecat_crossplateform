#ifndef CROSSPLATEFORM_H_INCLUDED
#define CROSSPLATEFORM_H_INCLUDED

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

* \file Crossplateform.h
* \brief {header file for the specifique crossplateform fonction of whitecat}
* \author Christoph G
* \version {0.8.6}
* \date {28/04/2014}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Header qui contient toutes les DEFINE, variables globales et fonctions permettant le crossplateform pour whitecat
*
*   Header for all the DEFINE and global variables and fonctions used for cross compilation
*
*/




/**
uncomment if whitecat is compilled for windows with MinGW else comment it and uncomment an other White define ex WhitePOSIX
*/
#ifdef _WIN32
// Convertit un chemin ACP (CP-1252) en UTF-8 pour SDL2
inline void wc_acp_to_utf8(const char* acp, char* utf8, int maxlen) {
    wchar_t wpath[512];
    MultiByteToWideChar(CP_ACP, 0, acp, -1, wpath, 512);
    WideCharToMultiByte(CP_UTF8, 0, wpath, -1, utf8, maxlen, NULL, NULL);
}
#else
// POSIX : chemins déjà en UTF-8, copie directe
#include <string.h>
inline void wc_acp_to_utf8(const char* acp, char* utf8, int maxlen) {
    strncpy(utf8, acp, maxlen - 1);
    utf8[maxlen - 1] = '\0';
}
#endif

#endif // CROSSPLATEFORM_H_INCLUDED
