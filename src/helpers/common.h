//
// Created by yong gu on 23/09/2018.
//

#ifndef CPP_MARKDOWN_HELPER_H
#define CPP_MARKDOWN_HELPER_H

#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <locale>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

namespace ccm {
    using std::string;
    extern "C" size_t decode_html_entities_utf8(char *dest, const char *src);

    std::string joinStrVec(const std::vector<std::string>& v, const std::string& joiner);
    //
    static inline bool isSpace(char ch) { return ch == 0x09 || ch == 0x20; }

    //string unescapeMd(string str) {
//    if (str.indexOf('\\') < 0) { return str; }
//    return str.replace(UNESCAPE_MD_RE, '$1');
//}

    std::string unescapeAll(std::string str);

    std::string escapeHtml(std::string str);

// Zs (unicode class) || [\t\f\v\r\n]
    bool isWhiteSpace(char code);

// Markdown ASCII punctuation characters.
//
// !, ", #, $, %, &, ', (, ), *, +, ,, -, ., /, :, ;, <, =, >, ?, @, [, \, ], ^, _, `, {, |, }, or ~
// http://spec.commonmark.org/0.15/#ascii-punctuation-character
//
// Don't confuse with unicode punctuation !!! It lacks some chars in ascii range.
//
    bool isMdAsciiPunct(char ch);
    bool isPunctChar(char ch);

    // for reference
    struct LinkRefRes {
        bool ok = false;
        int pos = 0;
        int lines = 0;
        std::string str;
    };

    LinkRefRes parseLinkDestination(std::string str, int pos, int max);

    LinkRefRes parseLinkTitle(std::string str, int pos, int max);

    std::string normalizeReference(const std::string str);

    std::string normalizeLink(const std::string str);
    std::string normalizeLinkText(const std::string str);

    bool validateLink(const std::string str);
}


#endif //CPP_MARKDOWN_HELPER_H
