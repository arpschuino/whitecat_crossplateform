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
#include <string>
#include <vector>

namespace wc {

// [devices] Slot nommé (GDTF ChannelSet) : une plage nommée d'un canal à crans
// (roue de gobos/couleur, prisme, canal de mode...). from16 = début de plage en 16 bit.
struct ChannelSlot {
    uint16_t    from16 = 0;   // valeur DMX 16 bit de début de plage
    std::string name;         // libellé GDTF ("Open", "Gobo 3", "Prism 3-facet"...)
};

// [devices] Indice d'affichage dérivé du GDTF (AttributeDefinitions : PhysicalUnit + Feature).
// Décide molette (continu) vs bouton de mode (à crans), façon EOS. Cf. fxc_is_slotted.
enum PhysHint : uint8_t {
    PHYS_CONTINUOUS = 0,  // grandeur physique continue (PhysicalUnit != None : Angle, ColorComponent...) -> MOLETTE
    PHYS_MODE       = 1,  // canal de mode (PhysicalUnit None + Feature "Control.*") -> BOUTON (ex. PositionMSpeed)
    PHYS_PLAIN      = 2    // None + autre -> BOUTON si beaucoup de crans nommés, sinon molette (défaut / shows legacy)
};

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
    ATTR_SHUTTER1,      // Beam — obturateur/strobe (rendu linéaire brut pour l'instant)
    ATTR_ZOOM,          // Beam — angle du faisceau
    // --- réservé (vagues suivantes) : ColorAdd_A/UV/Lime…, ColorSub_C/M/Y,
    //     CTO/CTC, Iris, Focus1, Gobo1(+rot), Prism1, Frost1,
    //     Color1 (roue), Function/Control … ---
    ATTR_RAW,           // [devices] canal GDTF non encore gere : RESERVE le footprint, sortie lineaire 0
                        //           (rendu comme un attribut non-Dimmer ; non pilotable dans Control Fixtures)
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
    uint16_t coarse_addr = 0;             // output DMX 1..512 (MSB). 0 = non patché.
    uint16_t fine_addr   = 0;             // output LSB. 0 = pas de fine (8 bit).
    uint8_t  resolution  = RES_8BIT;      // 8 ou 16
    uint16_t circuit     = 0;             // circuit de contrôle (source du niveau). Côté patch : render() l'ignore.
    uint16_t home        = 0;             // [devices] valeur "home" (défaut GDTF, 16 bit) de ce canal (bouton home)
    char     name[24]    = {0};           // [devices] nom d'attribut GDTF (ex. "Pan", "Gobo1", "Prism1") -> libellé + pilotage générique
    uint8_t  phys_hint   = PHYS_PLAIN;    // [devices] molette/bouton (dérivé GDTF PhysicalUnit+Feature) ; défaut = règle au nombre de crans
    std::vector<ChannelSlot> slots;       // [devices] plages nommées (GDTF ChannelSet) ; vide = paramètre continu (encodeur seul)

    // Écrit la valeur sur l'output DMX (buffer indices 1..512 ; 0 = start code).
    //   dmx        : DmxBlock (unsigned char[513])
    //   curve_lut  : curve_report (int[16][256]) passé en (*)[256]
    //
    // 8 bit  : le rendu dépend de l'ATTRIBUT (Phase devices) :
    //   - Dimmer : convention historique  DmxBlock[output] = 255 - curve_report[courbe][niveau 8 bit]
    //              (l'inversion "255 -" et la LUT 256 sont la convention WhiteCat des gradateurs).
    //   - autre (Color/Pan/Tilt… = paramètre de device) : LINÉAIRE, sans inversion ni courbe —
    //              la valeur du paramètre est écrite telle quelle (octet fort).
    // 16 bit : déplie la valeur haute résolution sur deux outputs — MSB sur coarse_addr,
    //            LSB sur fine_addr. Linéaire (ni inversion ni courbe), valable Dimmer ET Pan/Tilt.
    void render(unsigned char* dmx, const int (*curve_lut)[256]) const {
        if (coarse_addr == 0) return;                 // non patché

        if (resolution == RES_16BIT && fine_addr != 0) {
            dmx[coarse_addr] = static_cast<unsigned char>(value >> 8);     // MSB (octet fort)
            dmx[fine_addr]   = static_cast<unsigned char>(value & 0xFF);   // LSB (octet faible)
            return;
        }

        const uint8_t lvl8 = static_cast<uint8_t>(value >> 8);   // HR -> 8 bit (octet fort)
        if (attribute == ATTR_DIMMER) {
            dmx[coarse_addr] = static_cast<unsigned char>(255 - curve_lut[curve][lvl8]);
        } else {
            dmx[coarse_addr] = static_cast<unsigned char>(lvl8);   // paramètre device : linéaire
        }
    }
};

} // namespace wc

#endif // WC_CHANNEL_H
