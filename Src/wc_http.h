#pragma once
// ============================================================================
// wc_http.h — client HTTPS minimal et synchrone (WinHTTP sous Windows).
// Sert a l'integration GDTF Share (login / getList / download). POSIX = stub
// pour l'instant (a remplacer par libcurl pour Linux/Pi). Cf. gdtf_share.*.
// ============================================================================
#include <string>

namespace wchttp {

struct Response {
    bool        ok;          // requete effectuee (pas d'erreur reseau/TLS)
    int         status;      // code HTTP (200, 401, 404...)
    std::string body;        // corps (texte ou binaire)
    std::string set_cookie;  // header Set-Cookie brut (pour recuperer le cookie de session au login)
    std::string error;       // message si !ok
};

// Requete HTTPS synchrone. method = "GET" ou "POST". body non vide -> envoye.
// content_type (ex "application/json") et cookie (ex "PHPSESSID=...") optionnels (NULL/"" pour aucun).
Response request(const char* method, const char* url,
                 const std::string& body, const char* content_type, const std::string& cookie);

} // namespace wchttp
