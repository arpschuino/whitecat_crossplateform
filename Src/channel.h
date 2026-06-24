// ============================================================================
// channel.h — Brique de base du futur moteur multi-paramètres (fixtures).
// Phase 1 (cf. FIXTURES_ROADMAP.md). Étape 1a : définition de la classe, isolée,
// sans intégration au rendu. La cohabitation (1b/1c) vient ensuite.
//
// Principes (Phase 0, actés) :
//   - valeur interne en HAUTE RÉSOLUTION entière (uint16) : la résolution interne
//     dépasse la sortie pour absorber la chaîne de multiplications sans banding ;
//   - 8/16 bit = une PROPRIÉTÉ (resolution), pas un type ;
//   - l'univers fait partie de l'adresse dès maintenant (multi-univers prêt) ;
//   - HTP/LTP porté par le Channel ;
//   - composition : une Fixture contiendra un vector<Channel> (plus tard) ;
//   - nommage d'attributs CANONIQUE GDTF.
//
// Un circuit classique = un Channel { Dimmer, 8 bit, HTP }.
// ============================================================================
#ifndef WC_CHANNEL_H
#define WC_CHANNEL_H

#include <cstdint>

namespace wc {

// ---------------------------------------------------------------------------
// Taxonomie d'attributs — noms canoniques GDTF (Phase 0).
// VAGUE 1 (implémentée) : Dimmer, ColorAdd R/G/B/W, Pan, Tilt.
// Le reste (couleur étendue, CMY, CTO, Beam, slots, Control) est réservé et
// sera ajouté au fil des vagues — voir FIXTURES_ROADMAP.md.
// ---------------------------------------------------------------------------
enum AttrId : uint8_t {
    ATTR_NONE = 0,
    ATTR_DIMMER,        // Intensity
    ATTR_COLORADD_R,    // Color (additif)
    ATTR_COLORADD_G,
    ATTR_COLORADD_B,
    ATTR_COLORADD_W,
    ATTR_PAN,           // Position
    ATTR_TILT,
    // --- réservé (vagues suivantes) : ColorAdd_A/UV/Lime…, ColorSub_C/M/Y,
    //     CTO/CTC, Shutter1, Iris, Zoom, Focus1, Gobo1(+rot), Prism1, Frost1,
    //     Color1 (roue), Function/Control … ---
    ATTR_COUNT
};

// Mode de combinaison multi-sources (le mixage réel = moteur, Phase 6).
enum CombineMode : uint8_t {
    COMBINE_HTP = 0,    // le plus fort gagne (intensité)
    COMBINE_LTP = 1     // le dernier gagne (couleur, position, beam)
};

enum Resolution : uint8_t {
    RES_8BIT  = 8,
    RES_16BIT = 16
};

// ---------------------------------------------------------------------------
// Channel — un paramètre logique + son mapping DMX.
// POD léger (pas de vtable) : le rendu temps réel reste un balayage plat.
// ---------------------------------------------------------------------------
struct Channel {
    uint8_t  attribute   = ATTR_DIMMER;   // rôle (AttrId)
    uint16_t value       = 0;             // valeur interne 0..65535 (haute résolution)
    uint8_t  combine     = COMBINE_HTP;   // HTP / LTP (défaut hérité de l'attribut)
    uint8_t  curve       = 0;             // index de courbe (curve_report[curve])

    uint16_t universe    = 0;             // univers DMX (0 au début ; multi-univers prêt)
    uint16_t coarse_addr = 0;             // canal DMX 1..512 (MSB). 0 = non patché.
    uint16_t fine_addr   = 0;             // canal LSB. 0 = pas de fine (8 bit).
    uint8_t  resolution  = RES_8BIT;      // 8 ou 16

    // Écrit la valeur dans le buffer DMX (indices 1..512 ; 0 = start code).
    //   dmx        : DmxBlock (unsigned char[513])
    //   curve_lut  : curve_report (int[16][256]) passé en (*)[256]
    //
    // Étape 1a : reproduit AU BIT PRÈS le calcul historique du circuit 8 bit :
    //   DmxBlock[canal] = 255 - curve_report[courbe][niveau 8 bit]
    // (l'inversion "255 -" et la LUT 256 sont la convention WhiteCat actuelle).
    //
    // 16 bit (resolution==16, fine_addr!=0) + courbes haute résolution : Phase 2.
    void render(unsigned char* dmx, const int (*curve_lut)[256]) const {
        if (coarse_addr == 0) return;                 // non patché
        const uint8_t lvl8 = static_cast<uint8_t>(value >> 8);   // HR -> 8 bit (octet fort)
        dmx[coarse_addr] = static_cast<unsigned char>(255 - curve_lut[curve][lvl8]);
    }
};

} // namespace wc

#endif // WC_CHANNEL_H
