/*-------------------------------------------------------------------------------------------------------------
 White Cat - minimal HTTPS client (WinINet)  -  for GDTF Share integration
 GNU General Public License v2 or later.
 Note : ce MinGW portable fournit wininet.h (pas winhttp.h). WinINet gere HTTPS/TLS et surtout
 les COOKIES de session automatiquement (jar par process) -> login puis getList/download partagent
 le cookie sans gestion manuelle. INTERNET_OPEN_TYPE_PRECONFIG = proxy systeme (utile en salle).
---------------------------------------------------------------------------------------------------------------*/
#include "wc_http.h"

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#include <vector>

namespace wchttp {

Response request(const char* method, const char* url,
                 const std::string& body, const char* content_type, const std::string& cookie)
{
    Response r; r.ok=false; r.status=0;

    URL_COMPONENTSA uc; ZeroMemory(&uc, sizeof(uc)); uc.dwStructSize=sizeof(uc);
    char host[256]={0}, path[4096]={0};
    uc.lpszHostName=host; uc.dwHostNameLength=sizeof(host)-1;
    uc.lpszUrlPath=path;  uc.dwUrlPathLength=sizeof(path)-1;
    if(!InternetCrackUrlA(url, 0, 0, &uc)){ r.error="URL invalide"; return r; }
    bool https = (uc.nScheme==INTERNET_SCHEME_HTTPS);
    INTERNET_PORT port = uc.nPort;

    HINTERNET hNet = InternetOpenA("WhiteCat/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if(!hNet){ r.error="InternetOpen"; return r; }

    HINTERNET hConn = InternetConnectA(hNet, host, port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if(!hConn){ InternetCloseHandle(hNet); r.error="InternetConnect"; return r; }

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_KEEP_CONNECTION;
    if(https) flags |= INTERNET_FLAG_SECURE;
    HINTERNET hReq = HttpOpenRequestA(hConn, method, path, NULL, NULL, NULL, flags, 0);
    if(!hReq){ InternetCloseHandle(hConn); InternetCloseHandle(hNet); r.error="HttpOpenRequest"; return r; }

    // timeouts (evite un blocage si le reseau est lent/absent)
    DWORD tmo = 12000;
    InternetSetOption(hReq, INTERNET_OPTION_CONNECT_TIMEOUT, &tmo, sizeof(tmo));
    InternetSetOption(hReq, INTERNET_OPTION_RECEIVE_TIMEOUT, &tmo, sizeof(tmo));
    InternetSetOption(hReq, INTERNET_OPTION_SEND_TIMEOUT,    &tmo, sizeof(tmo));

    std::string headers;
    if(content_type && *content_type){ headers += "Content-Type: "; headers += content_type; headers += "\r\n"; }
    if(!cookie.empty()){ headers += "Cookie: "; headers += cookie; headers += "\r\n"; }

    BOOL sent = HttpSendRequestA(hReq,
        headers.empty()? NULL : headers.c_str(),
        headers.empty()? 0 : (DWORD)headers.size(),
        body.empty()? NULL : (LPVOID)body.data(),
        (DWORD)body.size());
    if(!sent){
        r.error="requete echouee (reseau/TLS)";
        InternetCloseHandle(hReq); InternetCloseHandle(hConn); InternetCloseHandle(hNet);
        return r;
    }

    DWORD status=0, len=sizeof(status), idx=0;
    HttpQueryInfoA(hReq, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &status, &len, &idx);
    r.status=(int)status;

    // Set-Cookie (diagnostic ; le jar WinINet renvoie deja le cookie tout seul aux requetes suivantes)
    {
        char cbuf[1024]; DWORD clen=sizeof(cbuf); DWORD cidx=0;
        if(HttpQueryInfoA(hReq, HTTP_QUERY_SET_COOKIE, cbuf, &clen, &cidx))
            r.set_cookie.assign(cbuf, clen);
    }

    // corps
    for(;;){
        char buf[8192]; DWORD read=0;
        if(!InternetReadFile(hReq, buf, sizeof(buf), &read)) break;
        if(read==0) break;
        r.body.append(buf, (size_t)read);
    }

    r.ok=true;
    InternetCloseHandle(hReq);
    InternetCloseHandle(hConn);
    InternetCloseHandle(hNet);
    return r;
}

} // namespace wchttp

#else  // ---- POSIX : stub (a remplacer par libcurl pour Linux/Pi) ----
namespace wchttp {
Response request(const char*, const char*, const std::string&, const char*, const std::string&)
{
    Response r; r.ok=false; r.status=0;
    r.error="HTTP indisponible sur cette plateforme (TODO libcurl)";
    return r;
}
}
#endif
