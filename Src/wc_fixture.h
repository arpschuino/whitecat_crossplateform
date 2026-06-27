// ============================================================================
// wc_fixture.h — Modèle de patch « appareils » (fixtures), cf. FIXTURES_ROADMAP
// Phase 3 anticipée (pour la persistance du patch 16 bit).
//
// SOURCE DE VÉRITÉ du patch = std::vector<wc::Fixture> wc_patch.
// Les tableaux legacy (Patch[], output_fine[], is_fine[], curves[], dimmer_type[])
// deviennent un CACHE DÉRIVÉ, régénéré par rebuild_patch_from_fixtures() ; le rendu
// (hot path) continue de les balayer (cohabitation : wrapper d'abord, remplacer ensuite).
//
//   Fixture = un appareil patché { nom ; vector<Channel> }.
//   Un dimmer = Fixture à 1 Channel Dimmer. Multi-paramètres (Pan/Tilt/RGBW) plus tard.
// ============================================================================
#ifndef WC_FIXTURE_H
#define WC_FIXTURE_H

#include "channel.h"
#include <string>
#include <vector>

namespace wc {

struct Fixture {
    std::string          name;       // libellé optionnel
    std::vector<Channel> channels;   // 1 pour un dimmer ; N pour un multi-paramètres
};

} // namespace wc

// Patch courant — défini dans patch.cpp (WC_SKIP_GLOBALS non requis : pas dans graphics_backend.h).
extern std::vector<wc::Fixture> wc_patch;

#endif // WC_FIXTURE_H
