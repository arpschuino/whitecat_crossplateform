/*-------------------------------------------------------------------------------------------------------------
 White Cat - GDTF Share API client
 GNU General Public License v2 or later.
---------------------------------------------------------------------------------------------------------------*/
#include "gdtf_share.h"
#include "wc_http.h"
#include "wc_json.h"
#include <cstdio>
#include <cstring>
#include <algorithm>

extern char mondirectory[512];   // dossier de l'exe (globals.h)

namespace gdtfshare {

static const char* BASE = "https://gdtf-share.com/apis/public/";

static std::string        s_cookie;
static std::vector<Fix>   s_index;

const std::vector<Fix>& index(){ return s_index; }
bool logged_in(){ return !s_cookie.empty(); }

// echappe une chaine pour du JSON (guillemets/backslash)
static std::string json_escape(const std::string& s)
{
    std::string o;
    for(size_t i=0;i<s.size();++i){ char c=s[i]; if(c=='"'||c=='\\'){ o+='\\'; } o+=c; }
    return o;
}

// chemin du cache index (user/gdtf_index.json a cote de l'exe)
static void cache_path(char* out, size_t n)
{
    snprintf(out, n, "%s%suser%sgdtf_index.json", mondirectory,
             "/", "/");   // separateurs POSIX acceptes sous Windows aussi
}

static void parse_list(const std::string& body)
{
    s_index.clear();
    wcjson::Value root = wcjson::parse(body.c_str(), body.size());
    const wcjson::Value* list = root.get("list");
    if(!list || list->type!=wcjson::Value::ARR) return;
    s_index.reserve(list->arr.size());
    for(size_t i=0;i<list->arr.size();++i)
    {
        const wcjson::Value& it = list->arr[i];
        if(it.type!=wcjson::Value::OBJ) continue;
        Fix f; f.rid=0;
        const wcjson::Value* v;
        if((v=it.get("rid")))          f.rid=v->as_int();
        if((v=it.get("fixture")))      f.fixture=v->as_str();
        if((v=it.get("manufacturer"))) f.manuf=v->as_str();
        if((v=it.get("modes")) && v->type==wcjson::Value::ARR)
        {
            for(size_t m=0;m<v->arr.size();++m)
            {
                const wcjson::Value& mo=v->arr[m];
                Mode md; md.footprint=0;
                const wcjson::Value* mv;
                if((mv=mo.get("name")))          md.name=mv->as_str();
                if((mv=mo.get("dmxfootprint")))  md.footprint=mv->as_int();
                f.modes.push_back(md);
            }
        }
        if(f.rid>0) s_index.push_back(f);
    }

    // dedoublonnage : plusieurs revisions par fixture -> garder la plus recente (rid max) par (fabricant, modele)
    std::sort(s_index.begin(), s_index.end(), [](const Fix& a, const Fix& b){
        if(a.manuf   != b.manuf)   return a.manuf   < b.manuf;
        if(a.fixture != b.fixture) return a.fixture < b.fixture;
        return a.rid > b.rid;   // rid decroissant -> le 1er de chaque groupe = plus recent
    });
    std::vector<Fix> ded; ded.reserve(s_index.size());
    for(size_t i=0;i<s_index.size();++i){
        if(!ded.empty() && ded.back().manuf==s_index[i].manuf && ded.back().fixture==s_index[i].fixture) continue;
        ded.push_back(s_index[i]);
    }
    s_index.swap(ded);
}

int login(const std::string& user, const std::string& pass, std::string& err)
{
    std::string body = "{\"user\":\"" + json_escape(user) + "\",\"password\":\"" + json_escape(pass) + "\"}";
    std::string url = std::string(BASE) + "login.php";
    wchttp::Response r = wchttp::request("POST", url.c_str(), body, "application/json", "");
    if(!r.ok){ err = r.error; return 1; }
    if(r.status!=200){ err = (r.status==401)?"identifiants refuses (401)":("HTTP "+std::to_string(r.status)); return 2; }
    // cookie de session : garder la partie name=value (avant le 1er ';')
    if(!r.set_cookie.empty()){
        size_t sc=r.set_cookie.find(';');
        s_cookie = (sc==std::string::npos)? r.set_cookie : r.set_cookie.substr(0,sc);
    }
    return 0;
}

int update_index(std::string& err, int& count)
{
    count=0;
    std::string url = std::string(BASE) + "getList.php";
    wchttp::Response r = wchttp::request("GET", url.c_str(), "", "", s_cookie);
    if(!r.ok){ err=r.error; return 1; }
    if(r.status!=200){ err="HTTP "+std::to_string(r.status); return 2; }
    parse_list(r.body);
    count=(int)s_index.size();
    // cache disque (brut)
    char path[600]; cache_path(path,sizeof(path));
    FILE* fp=fopen(path,"wb");
    if(fp){ fwrite(r.body.data(),1,r.body.size(),fp); fclose(fp); }
    return 0;
}

int download(int rid, std::string& bytes, std::string& err)
{
    char url[256]; snprintf(url,sizeof(url),"%sdownloadFile.php?rid=%d", BASE, rid);
    wchttp::Response r = wchttp::request("GET", url, "", "", s_cookie);
    if(!r.ok){ err=r.error; return 1; }
    if(r.status!=200){ err="HTTP "+std::to_string(r.status); return 2; }
    // un .gdtf est un zip (commence par "PK") ; sinon c'est une erreur JSON
    if(r.body.size()<2 || r.body[0]!='P' || r.body[1]!='K'){ err="reponse non-.gdtf"; return 3; }
    bytes.swap(r.body);
    return 0;
}

int load_cache()
{
    char path[600]; cache_path(path,sizeof(path));
    FILE* fp=fopen(path,"rb");
    if(!fp) return 1;
    fseek(fp,0,SEEK_END); long n=ftell(fp); fseek(fp,0,SEEK_SET);
    if(n<=0){ fclose(fp); return 2; }
    std::string body((size_t)n,0);
    size_t rd=fread(&body[0],1,(size_t)n,fp); fclose(fp);
    body.resize(rd);
    parse_list(body);
    return s_index.empty()?2:0;
}

} // namespace gdtfshare
