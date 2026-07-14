#pragma once
// ============================================================================
// gdtf_import.h — import d'une personality GDTF (description.xml deja extrait
// du .gdtf) vers le modele wc::Fixture. Sous-ensemble utile de GDTF :
//   GDTF > FixtureType > DMXModes > DMXMode > DMXChannels > DMXChannel[Offset]
//        > LogicalChannel[Attribute]
// Nos wc::AttrId sont nommes d'apres GDTF -> mapping direct des attributs standard.
// (miniz/zip = plus tard ; pour l'instant on lit le description.xml deconpresse.)
// ============================================================================
#include <string>
#include <vector>
#include "wc_fixture.h"

namespace wcgdtf {

struct ModeInfo {
    std::string name;        // Name du DMXMode
    int         nb_channels; // nb de DMXChannel controlables (attribut reconnu, Offset present)
    int         footprint;   // nb d'adresses DMX occupees (max offset)
};

// Liste les modes DMX du fichier + nom fabricant/modele. Retour : 0 OK ; 1 absent ; 2 structure invalide.
int list_modes(const char* xmlpath, std::vector<ModeInfo>& modes, std::string& fixture_name, std::string& manufacturer);

// Construit une wc::Fixture depuis le mode <mode_index> a l'adresse de depart <base>
// (1..512, adresse DMX du 1er canal) sur le circuit <circuit>. footprint renseigne le
// nb d'adresses occupees. Retour : 0 OK ; 1 absent ; 2 invalide ; 3 mode hors bornes.
// Chaque canal recoit sa valeur "home" (defaut GDTF, 16 bit) dans wc::Channel.home.
int build_fixture(const char* xmlpath, int mode_index, int base, int circuit,
                  wc::Fixture& fx, std::string& mode_name, int& footprint);

} // namespace wcgdtf
