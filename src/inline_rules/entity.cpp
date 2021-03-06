//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"

namespace ccm {
    bool entity(InlineState &state, bool silent);

    boost::regex ENTITYS_RE("^&((#(?:x[a-f0-9]{1,8}|[0-9]{1,8}))|([a-z][a-z0-9]{1,31}));", boost::regex::icase);
//        boost::regex ENTITY_RE("&");
//        boost::regex NAMED_RE("^&([a-z][a-z0-9]{1,31});");
}

bool ccm::entity(InlineState &state, bool silent) {
    int pos = state.pos, max = state.posMax;

    if (state.src[pos] != 0x26/* & */) { return false; }
    char ch;
    boost::smatch match;
    int len,l;
    if (pos + 1 < max) {
        std::string content = state.src.substr(pos);

        if (boost::regex_search(content, match, ENTITYS_RE) && match[0].first - content.begin() == 0) {
            if (!silent) {
                len = match[0].second - match[0].first;
                char decoded[34];
                l = decode_html_entities_utf8(decoded, content.substr(0, len).c_str());
                if (decoded[0] == 0 || match[2].matched && decoded[0] == '&' && decoded[l-1] == ';') {// invalid
                    state.pending += "\xEF\xBF\xBD";   // U+FFFD
                }else {
                    state.pending += std::string(decoded);
                }
                
            }
            state.pos += len;
            return true;
        }

    }

    if (!silent) { state.pending += '&'; }
    state.pos++;
    return true;
}