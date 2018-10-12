//
// Created by yong gu on 23/09/2018.
//

#include "common.h"
#include "encode.h"
#include <boost/regex.hpp>

namespace ccm {
    using std::string;

    bool validateLink(const std::string str) {    // not supported
        return true;
    }

    std::string normalizeLink(const std::string str) {
        return encode(str);
    }

    std::string normalizeLinkText(const std::string str) {
        return str;
    }

    std::string normalizeReference(const std::string str) {
        std::string newStr = boost::trim_copy(str);
        newStr = boost::regex_replace(newStr, boost::regex("\\s+"), " ");
        std::transform(newStr.begin(), newStr.end(), newStr.begin(), toupper);
        return newStr;
    }

    LinkRefRes parseLinkTitle(std::string str, int pos, int max) {
        char code,
                marker;
        int lines = 0, start = pos;

        LinkRefRes result;

        if (pos >= max) { return result; }

        marker = str[pos];

        if (marker != 0x22 /* " */ && marker != 0x27 /* ' */ && marker != 0x28 /* ( */) { return result; }

        pos++;

        // if opening marker is "(", switch it to closing marker ")"
        if (marker == 0x28) { marker = 0x29; }

        while (pos < max) {
            code = str[pos];
            if (code == marker) {
                result.pos = pos + 1;
                result.lines = lines;
                result.str = unescapeAll(str.substr(start + 1, pos - start - 1));
                result.ok = true;
                return result;
            } else if (code == 0x0A) {
                lines++;
            } else if (code == 0x5C /* \ */ && pos + 1 < max) {
                pos++;
                if (str[pos] == 0x0A) {
                    lines++;
                }
            }

            pos++;
        }

        return result;
    }

    LinkRefRes parseLinkDestination(std::string str, int pos, int max) {
        unsigned int code;
        int level,
                lines = 0,
                start = pos;
        LinkRefRes result;

        if (str[pos] == 0x3C /* < */) {
            pos++;
            while (pos < max) {
                code = str[pos];
                if (code == 0x0A /* \n */ ||
                    isSpace(code)
                        ) {
                    return result;
                }
                if (code == 0x3E /* > */) {
                    result.
                            pos = pos + 1;
                    result.
                            str = unescapeAll(str.substr(start + 1, pos - start - 1));
                    result.
                            ok = true;
                    return
                            result;
                }
                if (code == 0x5C /* \ */ && pos + 1 < max) {
                    pos += 2;
                    continue;
                }

                pos++;
            }

// no closing '>'
            return result;
        }

// this should be ... } else { ... branch

        level = 0;
        while (pos < max) {
            code = str[pos];

            if (code == 0x20) {
                break;
            }

// ascii control characters
            if (code < 0x20 || code == 0x7F) {
                break;
            }

            if (code == 0x5C /* \ */ && pos + 1 < max) {
                pos += 2;
                continue;
            }

            if (code == 0x28 /* ( */) {
                level++;
            }

            if (code == 0x29 /* ) */) {
                if (level == 0) {
                    break;
                }
                level--;
            }

            pos++;
        }

        if (start == pos) {
            return result;
        }
        if (level != 0) {
            return result;
        }

        result.str = unescapeAll(str.substr(start, pos - start));
        result.lines = lines;
        result.pos = pos;
        result.ok = true;
        return result;
    }

    string UNESCAPE_MD = "\\\\([!\"#$%&'()*+,\\-.\\/:;<=>?@[\\\\\\]^_`{|}~])"; // /g
//    string ENTITY = "&([a-z#][a-z0-9]{1,31})"; // /gi;

    boost::regex UNESCAPE_MD_RE(UNESCAPE_MD);
//    boost::regex ENTITY_RE(ENTITY, boost::regex::icase);
//    boost::regex UNESCAPE_ALL_RE(UNESCAPE_MD + '|' + ENTITY, boost::regex::icase);

    string unescapeAll(string str) {
        if (str.find('\\') == string::npos && str.find('&') == string::npos) { return str; }

        str = regex_replace(str, UNESCAPE_MD_RE, "$1");
        char dest[str.length()];
        decode_html_entities_utf8(dest, str.c_str());
        return string(dest);
    }

    std::string escapeHtml(std::string str) {
        boost::algorithm::replace_all(str,"&","&amp;");
        boost::algorithm::replace_all(str,"<","&lt;");
        boost::algorithm::replace_all(str,">","&gt;");
        boost::algorithm::replace_all(str,"\"","&quot;");
        return str;
    }

    bool isWhiteSpace(unsigned int codepoint) {
        if (codepoint >= 0x2000u && codepoint <= 0x200Au) { return true; }
        switch (codepoint) {
            case 0x09u: // \t
            case 0x0Au: // \n
            case 0x0Bu: // \v
            case 0x0Cu: // \f
            case 0x0Du: // \r
            case 0x20u:
            case 0xA0u:
            case 0x1680u:
            case 0x202Fu:
            case 0x205Fu:
            case 0x3000u:
                return true;
        }
        return false;
    }

    bool isMdAsciiPunct(unsigned int ch) {
        switch (ch) {
            case 0x21/* ! */:
            case 0x22/* " */:
            case 0x23/* # */:
            case 0x24/* $ */:
            case 0x25/* % */:
            case 0x26/* & */:
            case 0x27/* ' */:
            case 0x28/* ( */:
            case 0x29/* ) */:
            case 0x2A/* * */:
            case 0x2B/* + */:
            case 0x2C/* , */:
            case 0x2D/* - */:
            case 0x2E/* . */:
            case 0x2F/* / */:
            case 0x3A/* : */:
            case 0x3B/* ; */:
            case 0x3C/* < */:
            case 0x3D/* = */:
            case 0x3E/* > */:
            case 0x3F/* ? */:
            case 0x40/* @ */:
            case 0x5B/* [ */:
            case 0x5C/* \ */:
            case 0x5D/* ] */:
            case 0x5E/* ^ */:
            case 0x5F/* _ */:
            case 0x60/* ` */:
            case 0x7B/* { */:
            case 0x7C/* | */:
            case 0x7D/* } */:
            case 0x7E/* ~ */:
                return true;
            default:
                return false;
        }
    }

    bool isPunctChar(unsigned int ch) {   // not supported
        return false;
    }
}