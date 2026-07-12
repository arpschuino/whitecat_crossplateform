#pragma once
// ============================================================================
// gdtf_share.h — client de l'API GDTF Share (https://gdtf-share.com/apis/public/).
//   login.php (POST {user,password}) -> cookie de session (WinINet le garde).
//   getList.php (GET) -> catalogue (rid, manufacturer, fixture, modes) -> index + cache.
//   downloadFile.php?rid=N (GET) -> .gdtf (zip binaire).
// Cf. wc_http.* (transport) et wc_json.h (parsing). Modele : addon BlenderDMX.
// ============================================================================
#include <string>
#include <vector>

namespace gdtfshare {

struct Mode { std::string name; int footprint; };
struct Fix  { int rid; std::string manuf; std::string fixture; std::vector<Mode> modes; };

// Connexion. Retour 0 = OK ; sinon !=0, err rempli. Stocke le cookie de session.
int login(const std::string& user, const std::string& pass, std::string& err);
bool logged_in();

// Recupere le catalogue (getList) -> remplit l'index memoire + cache disque (user/gdtf_index.json).
// Retour 0 OK (count = nb fixtures), sinon !=0 + err.
int update_index(std::string& err, int& count);

// Telecharge le .gdtf d'une revision -> bytes. Retour 0 OK, sinon !=0 + err.
int download(int rid, std::string& bytes, std::string& err);

// Index en memoire (rempli par update_index ou load_cache).
const std::vector<Fix>& index();

// Charge l'index depuis le cache disque (offline / demarrage). Retour 0 OK, 1 = absent, 2 = invalide.
int load_cache();

} // namespace gdtfshare
