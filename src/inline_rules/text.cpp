//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"

namespace ccm{
    bool text(InlineState &state, bool silent);
}
namespace {
    bool isTerminatorChar(char ch) {
        switch (ch) {
            case 0x0A/* \n */:
            case 0x21/* ! */:
            case 0x23/* # */:
            case 0x24/* $ */:
            case 0x25/* % */:
            case 0x26/* & */:
            case 0x2A/* * */:
            case 0x2B/* + */:
            case 0x2D/* - */:
            case 0x3A/* : */:
            case 0x3C/* < */:
            case 0x3D/* = */:
            case 0x3E/* > */:
            case 0x40/* @ */:
            case 0x5B/* [ */:
            case 0x5C/* \ */:
            case 0x5D/* ] */:
            case 0x5E/* ^ */:
            case 0x5F/* _ */:
            case 0x60/* ` */:
            case 0x7B/* { */:
            case 0x7D/* } */:
            case 0x7E/* ~ */:
                return true;
            default:
                return false;
        }
    }
}

bool ccm::text(InlineState &state, bool silent) {
    int pos = state.pos;

    while (pos < state.posMax && !isTerminatorChar(state.src[pos])) {
        pos++;
    }

    if (pos == state.pos) { return false; }

    if (!silent) { state.pending += state.src.substr(state.pos, pos - state.pos); }

    state.pos = pos;

    return true;
}