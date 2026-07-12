#pragma once
// ============================================================================
// wc_json.h — parseur JSON minimal autonome (pour l'API GDTF Share getList).
// Recursif, tolerant, zero dependance. Construit un arbre Value (objet/tableau/
// chaine/nombre/bool/null). Suffisant pour du JSON genere par une API.
// ============================================================================
#include <string>
#include <vector>
#include <cstdlib>

namespace wcjson {

struct Value;
typedef std::vector<Value> Array;
typedef std::vector<std::pair<std::string, Value> > Object;

struct Value {
    enum Type { NUL, BOOL, NUM, STR, ARR, OBJ };
    Type        type;
    bool        b;
    double      num;
    std::string str;
    Array       arr;
    Object      obj;

    Value() : type(NUL), b(false), num(0) {}

    const Value* get(const char* key) const {
        if(type!=OBJ) return 0;
        for(size_t i=0;i<obj.size();++i) if(obj[i].first==key) return &obj[i].second;
        return 0;
    }
    const char* as_str() const { return type==STR ? str.c_str() : ""; }
    int as_int() const {
        if(type==NUM) return (int)num;
        if(type==STR) return atoi(str.c_str());
        return 0;
    }
};

// --- parseur ---
struct _P { const char* p; const char* end; };

inline void _skip_ws(_P& s){ while(s.p<s.end){ char c=*s.p; if(c==' '||c=='\t'||c=='\r'||c=='\n') ++s.p; else break; } }

inline std::string _parse_string(_P& s){
    std::string out;
    if(s.p>=s.end || *s.p!='"') return out;
    ++s.p;
    while(s.p<s.end){
        char c=*s.p++;
        if(c=='"') break;
        if(c=='\\' && s.p<s.end){
            char e=*s.p++;
            switch(e){
                case '"': out+='"'; break;   case '\\': out+='\\'; break;
                case '/': out+='/'; break;   case 'n': out+='\n'; break;
                case 't': out+='\t'; break;  case 'r': out+='\r'; break;
                case 'b': out+='\b'; break;  case 'f': out+='\f'; break;
                case 'u': {
                    // \uXXXX -> on ne garde que l'ASCII (les noms de fixtures sont surtout ASCII)
                    if(s.p+4<=s.end){
                        int code=0; for(int k=0;k<4;k++){ char h=*s.p++; code<<=4;
                            if(h>='0'&&h<='9')code|=h-'0'; else if(h>='a'&&h<='f')code|=h-'a'+10; else if(h>='A'&&h<='F')code|=h-'A'+10; }
                        if(code<128) out+=(char)code; else out+='?';
                    }
                } break;
                default: out+=e; break;
            }
        } else out+=c;
    }
    return out;
}

Value _parse_value(_P& s);

inline Value _parse_object(_P& s){
    Value v; v.type=Value::OBJ; ++s.p; // '{'
    _skip_ws(s);
    if(s.p<s.end && *s.p=='}'){ ++s.p; return v; }
    while(s.p<s.end){
        _skip_ws(s);
        std::string key=_parse_string(s);
        _skip_ws(s);
        if(s.p<s.end && *s.p==':') ++s.p;
        Value val=_parse_value(s);
        v.obj.push_back(std::make_pair(key,val));
        _skip_ws(s);
        if(s.p<s.end && *s.p==','){ ++s.p; continue; }
        if(s.p<s.end && *s.p=='}'){ ++s.p; break; }
        break;
    }
    return v;
}

inline Value _parse_array(_P& s){
    Value v; v.type=Value::ARR; ++s.p; // '['
    _skip_ws(s);
    if(s.p<s.end && *s.p==']'){ ++s.p; return v; }
    while(s.p<s.end){
        Value val=_parse_value(s);
        v.arr.push_back(val);
        _skip_ws(s);
        if(s.p<s.end && *s.p==','){ ++s.p; continue; }
        if(s.p<s.end && *s.p==']'){ ++s.p; break; }
        break;
    }
    return v;
}

inline Value _parse_value(_P& s){
    _skip_ws(s);
    Value v;
    if(s.p>=s.end) return v;
    char c=*s.p;
    if(c=='{') return _parse_object(s);
    if(c=='[') return _parse_array(s);
    if(c=='"'){ v.type=Value::STR; v.str=_parse_string(s); return v; }
    if(c=='t'){ v.type=Value::BOOL; v.b=true;  s.p+=(s.p+4<=s.end?4:(s.end-s.p)); return v; }
    if(c=='f'){ v.type=Value::BOOL; v.b=false; s.p+=(s.p+5<=s.end?5:(s.end-s.p)); return v; }
    if(c=='n'){ v.type=Value::NUL;             s.p+=(s.p+4<=s.end?4:(s.end-s.p)); return v; }
    // nombre
    const char* start=s.p;
    while(s.p<s.end){ char d=*s.p; if((d>='0'&&d<='9')||d=='-'||d=='+'||d=='.'||d=='e'||d=='E') ++s.p; else break; }
    v.type=Value::NUM; v.num=atof(std::string(start,s.p-start).c_str());
    return v;
}

inline Value parse(const char* data, size_t len){
    _P s; s.p=data; s.end=data+len;
    return _parse_value(s);
}

} // namespace wcjson
