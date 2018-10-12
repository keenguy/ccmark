//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"

namespace ccm{
    bool backtick(InlineState &state, bool silent);
}

bool ccm::backtick(InlineState &state, bool silent) {
//            var start, max, marker, matchStart, matchEnd, token,
    int pos = state.pos;
    char ch = state.src[pos];

    if (ch != 0x60/* ` */) { return false; }

    int start = pos;
    pos++;
    int max = state.posMax;

    while (pos < max && state.src[pos] == 0x60/* ` */) { pos++; }

    std::string marker = state.src.substr(start, pos - start);

    int matchStart = pos, matchEnd = pos;

    while ((matchStart = state.src.find("`", matchEnd)) != std::string::npos) {
        matchEnd = matchStart + 1;

        while (matchEnd < max && state.src[matchEnd] == 0x60/* ` */) { matchEnd++; }

        if (matchEnd - matchStart == marker.length()) {
            if (!silent) {
                Token token("code_inline", "code", 0);
                token.markup = marker;
                token.content = boost::trim_copy(state.src.substr(pos, matchStart - pos));
                token.content = boost::regex_replace(token.content, boost::regex("[ \\n]+"), " ");
                state.push(token);
            }
            state.pos = matchEnd;
            return true;
        }
    }

    if (!silent) { state.pending += marker; }
    state.pos += marker.length();
    return true;
}