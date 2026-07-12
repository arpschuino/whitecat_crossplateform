/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
       WWWWWWWWWWWWWWW           |
       WWWWWWWWWWWWWWW           |               2026       Jacques Bouault - arpschuino.fr
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

* \file whitecat.h
* \brief {header file for all the global variable in whitecat}
* \author Christoph Guillermet
* \modified Jacques Bouault - arpschuino.fr - 2026
* \version {0.8.7.0}
* \date {15/05/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Header qui contient toutes les variables globales déclarées dans whitecat
*
*   Header for all the global variables used in whitecat
*
*/

#include "Crossplateform.h"
#include "wc_levels.h"   // [Phase 2c] echelle des niveaux 16 bit (dispo dans MAIN et tous les TUs)
#include "patch.h"
#include "audio.h"
#include "dmx.h"
#include "midi.h"
#include "network.h"
#include "chasers.h"

#include "globals.h"

#include "display.h"

// gel list + trichro -> trichro.h
#include "trichro.h"

#include "ui_indexes.h"
#include "wc_inline_edit.h"

// faders -> faders.h
#include "faders.h"
#include "channels.h"


#include "video.h"




#include "saveload.h"

// sequenciel, time, memoires -> sequenciel.h
#include "sequenciel.h" 

// couleurs et polices -> theme.h
#include "theme.h"
// import SCHWZ -> schwz.h
#include "schwz.h"

// Freeze -> dmx.h
// fader snapshots -> faders.h

// banger -> banger.h
#include "banger.h"
// Grand Master + DMX extra -> dmx.h



// wizard -> wizard.h
#include "wizard.h"



// arduino -> arduino.h
#include "arduino.h"



// grider -> grider.h
#include "grider.h"

// control fixtures (devices) -> fixturectl.h
#include "fixturectl.h"

// patch a device (devices) -> devicepatch.h
#include "devicepatch.h"

// light plot -> plot.h
#include "plot.h"


// descriptif_network_adapter -> network.h

// draw -> draw.h
#include "draw.h"

// echo -> echo.h
#include "echo.h"
