//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"

bool ccm::newline(InlineState &state, bool silent) {
    int pmax, max, pos = state.pos;

    if (state.src[pos] != 0x0A/* \n */) { return false; }

    pmax = state.pending.length() - 1;
    max = state.posMax;

    // "  \n" -> hardbreak
    // Lookup in pending chars is bad practice! Don't copy to other rules!
    // Pending string is stored in concat mode, indexed lookups will cause
    // convertion to flat mode.
    Token t;
    if (!silent) {
        if (pmax >= 0 && state.pending[pmax] == 0x20) {
            if (pmax >= 1 && state.pending[pmax - 1] == 0x20) {
                boost::trim_right(state.pending);
                t = Token("hardbreak", "br", 0);
            } else {
                state.pending.erase(state.pending.length() - 1, 1);
                t = Token("softbreak", "br", 0);
            }

        } else {
            t = Token("softbreak", "br", 0);
        }
        state.push(t);
    }

    pos++;

    // skip heading spaces for next line
    while (pos < max && isSpace(state.src[pos])) { pos++; }

    state.pos = pos;
    return true;
};