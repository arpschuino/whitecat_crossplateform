// ============================================================================
// wc_levels.h — Échelle interne des niveaux (Phase 2c, cf. FIXTURES_ROADMAP).
// Le cœur travaille en HAUTE RÉSOLUTION 0..LVL_MAX (16 bit). La sortie DMX 8 bit
// n'est qu'un cas (octet de poids fort). Facteur 257 (255*257 = 65535) → un
// niveau 8 bit converti puis ressorti en 8 bit redonne EXACTEMENT l'original.
// ============================================================================
#ifndef WC_LEVELS_H
#define WC_LEVELS_H

#include <cstdint>

namespace wc {

static const uint16_t LVL_MAX = 65535;   // pleine échelle interne (16 bit)

// 16 bit -> 8 bit (octet de poids fort). lvl_to_dmx8(dmx8_to_lvl(d)) == d.
inline uint8_t  lvl_to_dmx8(uint16_t v) { return static_cast<uint8_t>(v >> 8); }

// 8 bit -> 16 bit pleine échelle. 0->0, 255->65535.
inline uint16_t dmx8_to_lvl(uint8_t d)  { return static_cast<uint16_t>(d * 257); }

// 16 bit -> pourcentage 0..100 (arrondi).
inline int      lvl_to_pct(uint16_t v)  { return static_cast<int>(((long)v * 100 + LVL_MAX / 2) / LVL_MAX); }

// pourcentage 0..100 -> 16 bit (arrondi).
inline uint16_t pct_to_lvl(int p)       { return static_cast<uint16_t>(((long)p * LVL_MAX + 50) / 100); }

} // namespace wc

#endif // WC_LEVELS_H
