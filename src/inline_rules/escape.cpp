//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"

namespace ccm {
    bool escape(InlineState &state, bool silent);

    std::vector<int> ESCAPED(256, 0);
    std::string ESCAPE_CHARS{"\\!\"#$%&\'()*+,./:;<=>\?@[]^_`{|}~-"};
}

bool ccm::escape(InlineState &state, bool silent) {
    unsigned int ch;
    int pos = state.pos, max = state.posMax;

    if (state.src[pos] != 0x5C/* \ */) { return false; }

    pos++;

    if (pos < max) {
        ch = state.src[pos];

        if (ch < 128 && ESCAPED[ch] != 0) {
            if (!silent) { state.pending += state.src[pos]; }
            state.pos += 2;
            return true;
        }
        if (ch == 0x0A) {
            if (!silent) {
                state.push(Token("hardbreak", "br", 0));
            }

            pos++;
            // skip leading whitespaces from next line
            while (pos < max) {
                ch = state.src[pos];
                if (!isSpace(ch)) { break; }
                pos++;
            }

            state.pos = pos;
            return true;
        }
    }

    if (!silent) { state.pending += '\\'; }
    state.pos++;
    return true;
}