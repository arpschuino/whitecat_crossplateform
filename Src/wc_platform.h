#pragma once
/*
 * wc_platform.h — abstraction couche OS : Windows / Linux / macOS
 *
 * Inclus en tête de graphics_backend.h.
 * Fournit :
 *   - headers système spécifiques à chaque plateforme
 *   - types compat Winsock pour POSIX (SOCKET, SOCKADDR_IN, WSAStartup…)
 *   - helpers portables : wc_get_exe_dir(), wc_get_temp_dir()
 */

#include <stdlib.h>  // getenv, system
#include <string.h>  // strncpy, strlen
#include <stdio.h>   // snprintf (wc_open_path)

// ============================================================
// Windows
// ============================================================
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define _WINSOCKAPI_    // bloquer l'ancien winsock.h tiré par windows.h
    #include <windows.h>
    #include <winsock2.h>
    #include <direct.h>    // _chdir
    #ifndef chdir
    #define chdir _chdir
    #endif
    #define wc_mkdir(p) _mkdir(p)   // _mkdir Win32 : 1 seul argument
    // socklen_t n'est pas défini par Winsock2 (utilise int) — on l'aligne sur POSIX
    typedef int socklen_t;
    // Sleep Win32 en ms — SDL_Delay est préférable mais Sleep reste utilisé dans l'ancien code
    #define wc_sleep_ms(ms) Sleep(ms)

// ============================================================
// POSIX (Linux + macOS)
// ============================================================
#else
    #include <unistd.h>        // chdir, readlink, close, ssize_t
    #include <sys/socket.h>    // socket, bind, sendto, recvfrom (socklen_t inclus)
    #include <netinet/in.h>    // struct sockaddr_in, IPPROTO_UDP
    #include <arpa/inet.h>     // inet_addr, inet_ntoa
    #include <netdb.h>         // gethostname, gethostbyname
    #include <dirent.h>        // opendir, readdir, closedir
    #include <sys/stat.h>      // stat, struct stat
    #include <sys/ioctl.h>     // ioctl, FIONBIO
    #include <signal.h>        // signal, SIGABRT, SIG_DFL, raise
    #include <stdio.h>         // fprintf (pour les handlers de crash)

    // Types compatibles Winsock
    typedef int             SOCKET;
    typedef struct sockaddr SOCKADDR;
    typedef struct sockaddr_in SOCKADDR_IN;
    #define INVALID_SOCKET  ((SOCKET)(-1))
    #define SOCKET_ERROR    (-1)
    #define closesocket(s)  close(s)
    #define FAR             // FAR est vide sur les plateformes 32/64-bit modernes
    #define MAKEWORD(a,b)   ((unsigned short)(((unsigned char)(a)) | (((unsigned short)(unsigned char)(b)) << 8)))

    typedef struct { unsigned short wVersion; char _pad[126]; } WSADATA;
    static inline int  WSAStartup(unsigned short v, WSADATA* d) { (void)v; (void)d; return 0; }
    static inline void WSACleanup(void) {}

    // ioctlsocket n'existe pas sur POSIX
    #define ioctlsocket(s, cmd, argp) ioctl(s, cmd, argp)

    // Sleep Win32 → usleep POSIX (ms → µs)
    #define wc_sleep_ms(ms) usleep((ms) * 1000)

    // mkdir POSIX : requiert droits (mode_t)
    #define wc_mkdir(p) mkdir((p), 0755)
#endif

// Separateur de chemin (backslash Win32, slash POSIX)
#ifdef _WIN32
    #define WC_DIRSEP     "\\"
    #define WC_DIRSEP_C   '\\'
#else
    #define WC_DIRSEP     "/"
    #define WC_DIRSEP_C   '/'
#endif

// ============================================================
// wc_open_path — ouvre un fichier/URL avec l'application par defaut de l'OS.
// Win32: 'start' (interne cmd) ; Linux: xdg-open ; macOS: open.
// ('start' n'existe QUE sous cmd.exe -> "Help inactif" signale sous Linux.)
// ============================================================
static inline void wc_open_path(const char* path) {
    char _wc_ocmd[1200];
#ifdef _WIN32
    snprintf(_wc_ocmd, sizeof(_wc_ocmd), "start \"\" \"%s\"", path);   // 1er "" = titre de fenetre
#elif defined(__APPLE__)
    snprintf(_wc_ocmd, sizeof(_wc_ocmd), "open \"%s\"", path);
#else
    snprintf(_wc_ocmd, sizeof(_wc_ocmd), "xdg-open \"%s\" >/dev/null 2>&1 &", path);
#endif
    system(_wc_ocmd);
}

// ============================================================
// wc_get_exe_dir — dossier de l'exécutable, sans slash/backslash final
// Exemple : "C:\WhiteCat" ou "/home/user/whitecat"
// ============================================================
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

static inline void wc_get_exe_dir(char* out, int maxlen) {
#ifdef _WIN32
    GetModuleFileNameA(NULL, out, (DWORD)maxlen);
    for (int i = (int)strlen(out) - 1; i >= 0; i--) {
        if (out[i] == '\\' || out[i] == '/') { out[i] = '\0'; break; }
    }
#elif defined(__APPLE__)
    unsigned int sz = (unsigned int)maxlen;
    if (_NSGetExecutablePath(out, &sz) != 0) { out[0] = '.'; out[1] = '\0'; return; }
    for (int i = (int)strlen(out) - 1; i >= 0; i--) {
        if (out[i] == '/') { out[i] = '\0'; break; }
    }
#else  // Linux
    ssize_t n = readlink("/proc/self/exe", out, (size_t)(maxlen - 1));
    if (n <= 0) { out[0] = '.'; out[1] = '\0'; return; }
    out[n] = '\0';
    for (int i = (int)n - 1; i >= 0; i--) {
        if (out[i] == '/') { out[i] = '\0'; break; }
    }
#endif
}

// ============================================================
// wc_get_temp_dir — dossier temporaire avec slash/backslash final
// Exemple : "C:\Users\user\AppData\Local\Temp\" ou "/tmp/"
// ============================================================
static inline void wc_get_temp_dir(char* out, int maxlen) {
#ifdef _WIN32
    GetTempPathA((DWORD)maxlen, out);
#else
    const char* tmp = getenv("TMPDIR");
    if (!tmp || tmp[0] == '\0') tmp = "/tmp/";
    strncpy(out, tmp, (size_t)(maxlen - 1));
    out[maxlen - 1] = '\0';
    int n = (int)strlen(out);
    if (n > 0 && out[n-1] != '/' && n < maxlen - 1) { out[n] = '/'; out[n+1] = '\0'; }
#endif
}
