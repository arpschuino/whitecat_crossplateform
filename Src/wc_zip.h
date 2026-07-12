#pragma once
// ============================================================================
// wc_zip.h — extraction d'UN fichier d'une archive ZIP en memoire, via zlib
// (deja linkee pour les saves gzip). Pas de dependance nouvelle. Suffisant pour
// sortir description.xml d'un .gdtf. Lit le central directory (fiable) puis
// inflate l'entree (methode 0 = stored, 8 = deflate).
// ============================================================================
#include <string>
#include <cstring>
#include <cstdlib>
#include <zlib.h>

namespace wczip {

inline unsigned int  _rd32(const unsigned char* p){ return (unsigned int)p[0] | ((unsigned int)p[1]<<8) | ((unsigned int)p[2]<<16) | ((unsigned int)p[3]<<24); }
inline unsigned short _rd16(const unsigned char* p){ return (unsigned short)(p[0] | (p[1]<<8)); }

inline bool _inflate_raw(const unsigned char* src, size_t srclen, size_t rawlen, std::string& out)
{
    out.assign(rawlen, '\0');
    z_stream zs; memset(&zs, 0, sizeof(zs));
    if(inflateInit2(&zs, -15)!=Z_OK) return false;   // -15 = raw deflate (pas d'en-tete zlib)
    zs.next_in  = (Bytef*)src;   zs.avail_in  = (uInt)srclen;
    zs.next_out = (Bytef*)&out[0]; zs.avail_out = (uInt)rawlen;
    int r = inflate(&zs, Z_FINISH);
    size_t total = zs.total_out;
    inflateEnd(&zs);
    if(r!=Z_STREAM_END && r!=Z_OK) return false;
    out.resize(total);
    return true;
}

// Extrait le fichier <name> de l'archive zip (data,len) dans out. Retour true si trouve+extrait.
inline bool extract(const char* data, size_t len, const char* name, std::string& out)
{
    const unsigned char* buf = (const unsigned char*)data;
    if(len < 22) return false;

    // localiser l'EOCD (End Of Central Directory, signature PK\5\6) en remontant depuis la fin
    size_t limit = (len>65557)? len-65557 : 0;
    size_t eocd = (size_t)-1;
    for(size_t i=len-22; ; --i){
        if(buf[i]==0x50 && buf[i+1]==0x4b && buf[i+2]==0x05 && buf[i+3]==0x06){ eocd=i; break; }
        if(i==limit) break;
    }
    if(eocd==(size_t)-1) return false;

    unsigned int  cd_off   = _rd32(buf+eocd+16);
    unsigned short cd_count = _rd16(buf+eocd+10);

    size_t p = cd_off;
    for(int e=0; e<cd_count && p+46<=len; ++e){
        if(!(buf[p]==0x50 && buf[p+1]==0x4b && buf[p+2]==0x01 && buf[p+3]==0x02)) break;   // PK\1\2
        unsigned short method   = _rd16(buf+p+10);
        unsigned int   csize    = _rd32(buf+p+20);
        unsigned int   usize    = _rd32(buf+p+24);
        unsigned short fnlen    = _rd16(buf+p+28);
        unsigned short extralen = _rd16(buf+p+30);
        unsigned short commlen  = _rd16(buf+p+32);
        unsigned int   lho      = _rd32(buf+p+42);
        std::string fn((const char*)(buf+p+46), fnlen);
        if(fn==name){
            if(lho+30>len) return false;
            if(!(buf[lho]==0x50 && buf[lho+1]==0x4b && buf[lho+2]==0x03 && buf[lho+3]==0x04)) return false; // PK\3\4
            unsigned short lfnlen = _rd16(buf+lho+26);
            unsigned short lexlen = _rd16(buf+lho+28);
            size_t data_off = (size_t)lho + 30 + lfnlen + lexlen;
            if(data_off + csize > len) return false;
            if(method==0){ out.assign((const char*)(buf+data_off), csize); return true; }     // stored
            if(method==8){ return _inflate_raw(buf+data_off, csize, usize, out); }             // deflate
            return false;
        }
        p += 46 + fnlen + extralen + commlen;
    }
    return false;
}

} // namespace wczip
