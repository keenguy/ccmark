//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"
#include "../helpers/html_re.h"

namespace ccm{
    bool html_inline(InlineState &state, bool silent);
}

bool ccm::html_inline(InlineState &state, bool silent) {


    if (!state.options.html) { return false; }

    // Check start
    int pos = state.pos, max = state.posMax;
    if (state.src[pos] != 0x3C/* < */ ||
        pos + 2 >= max) {
        return false;
    }

    // Quick fail on second char
    char ch = state.src[pos + 1];
    if (ch != 0x21/* ! */ &&
        ch != 0x3F/* ? */ &&
        ch != 0x2F/* / */ &&
        !isLetter(ch)) {
        return false;
    }

    boost::smatch match;
    bool matched = boost::regex_search(state.src.substr(pos), match, boost::regex(HTML_TAG));
    if (!matched) { return false; }

    if (!silent) {
        Token token("html_inline", "", 0);
        token.content = state.src.substr(pos, match[0].second - match[0].first);
        state.push(token);
    }
    state.pos += match[0].second - match[0].first;
    return true;
}