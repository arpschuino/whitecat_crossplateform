// En-tete precompile pour les unites de traduction WhiteCat.
// Precompile automatiquement par build.bat — ne pas inclure avant compilation PCH.
#pragma once
#define WC_SKIP_GLOBALS
#include "graphics_backend.h"
using namespace ol;
#include "whitecat.h"
#include "core.h"
#include "wc_levels.h"  // [Phase 2c] echelle des niveaux 16 bit (dmx8 <-> lvl), dispo dans tous les TUs
