#pragma once
// ============================================================================
// wc_xml.h — mini DOM XML autonome (pour l'import GDTF : description.xml).
// Volontairement minimal : on ne lit QUE les elements et leurs attributs (le
// texte/CDATA est ignore — GDTF met toute l'info dans des attributs). Gere :
// prologue <?xml?>, commentaires <!-- -->, doctype <!...>, balises auto-fermantes
// <x/>, quotes simples/doubles, entites nommees de base (&amp; &lt; &gt; &quot; &apos;).
// Pas de namespaces (GDTF n'en utilise pas dans description.xml).
// ============================================================================
#include <string>
#include <vector>
#include <cstring>

namespace wcxml {

struct Attr { std::string name, value; };

struct Node {
    std::string        name;
    std::vector<Attr>  attrs;
    std::vector<Node>  children;

    // Valeur d'un attribut, ou NULL si absent.
    const char* attr(const char* n) const {
        for (size_t i = 0; i < attrs.size(); ++i)
            if (attrs[i].name == n) return attrs[i].value.c_str();
        return 0;
    }
    // 1er enfant de nom <cn>, ou NULL.
    const Node* child(const char* cn) const {
        for (size_t i = 0; i < children.size(); ++i)
            if (children[i].name == cn) return &children[i];
        return 0;
    }
};

// --- decodage minimal des entites dans une valeur d'attribut ---
inline void _decode_entities(std::string& s) {
    if (s.find('&') == std::string::npos) return;
    std::string o; o.reserve(s.size());
    for (size_t i = 0; i < s.size(); ) {
        if (s[i] == '&') {
            if      (!s.compare(i,5,"&amp;"))  { o+='&'; i+=5; }
            else if (!s.compare(i,4,"&lt;"))   { o+='<'; i+=4; }
            else if (!s.compare(i,4,"&gt;"))   { o+='>'; i+=4; }
            else if (!s.compare(i,6,"&quot;")) { o+='"'; i+=6; }
            else if (!s.compare(i,6,"&apos;")) { o+='\''; i+=6; }
            else { o+=s[i]; ++i; }
        } else { o+=s[i]; ++i; }
    }
    s.swap(o);
}

inline bool _is_space(char c){ return c==' '||c=='\t'||c=='\r'||c=='\n'; }
inline bool _is_name(char c){ return !_is_space(c) && c!='>' && c!='/' && c!='=' && c!='<'; }

// Parse un buffer XML complet. Retourne un noeud racine synthetique dont
// children[] contient les elements de premier niveau (typiquement <GDTF>).
inline Node parse(const char* p, size_t len) {
    const char* end = p + len;
    std::vector<Node> stack;
    stack.push_back(Node());   // racine synthetique

    while (p < end) {
        // aller au prochain '<'
        while (p < end && *p != '<') ++p;
        if (p >= end) break;
        ++p; // passe '<'
        if (p >= end) break;

        if (*p == '?') {                       // prologue <?xml ... ?>
            while (p < end && !(p[0]=='?' && p+1<end && p[1]=='>')) ++p;
            p += 2; continue;
        }
        if (*p == '!') {                       // <!-- --> ou <!DOCTYPE ...>
            if (p+2 < end && p[1]=='-' && p[2]=='-') {
                p += 3;
                while (p+2 < end && !(p[0]=='-'&&p[1]=='-'&&p[2]=='>')) ++p;
                p += 3;
            } else {
                while (p < end && *p != '>') ++p;
                ++p;
            }
            continue;
        }
        if (*p == '/') {                       // balise fermante </name>
            ++p;
            while (p < end && *p != '>') ++p;
            ++p;
            if (stack.size() > 1) {
                Node n = stack.back();
                stack.pop_back();
                stack.back().children.push_back(n);
            }
            continue;
        }

        // balise ouvrante <name attrs ...> ou auto-fermante <name .../>
        Node node;
        const char* s = p;
        while (p < end && _is_name(*p)) ++p;
        node.name.assign(s, p - s);

        // attributs
        for (;;) {
            while (p < end && _is_space(*p)) ++p;
            if (p >= end || *p=='>' || *p=='/') break;
            const char* ns = p;
            while (p < end && _is_name(*p)) ++p;
            Attr a; a.name.assign(ns, p - ns);
            while (p < end && _is_space(*p)) ++p;
            if (p < end && *p=='=') {
                ++p;
                while (p < end && _is_space(*p)) ++p;
                if (p < end && (*p=='"' || *p=='\'')) {
                    char q = *p++;
                    const char* vs = p;
                    while (p < end && *p != q) ++p;
                    a.value.assign(vs, p - vs);
                    if (p < end) ++p; // passe la quote fermante
                    _decode_entities(a.value);
                }
            }
            node.attrs.push_back(a);
        }

        bool self_closing = (p < end && *p=='/');
        while (p < end && *p != '>') ++p;
        ++p; // passe '>'

        if (self_closing) stack.back().children.push_back(node);   // pas d'enfants
        else              stack.push_back(node);                    // devient le contexte courant
    }

    return stack.empty() ? Node() : stack[0];
}

} // namespace wcxml
