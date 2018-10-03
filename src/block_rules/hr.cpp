//
// Created by yong gu on 01/10/2018.
//
#include "block_rules.h"

bool ccm::hr(BlockState &state, int startLine, int endLine, bool silent) {
    char marker, ch;
    int cnt;
    int pos = state.bMarks[startLine] + state.tShift[startLine];
    int max = state.eMarks[startLine];

    // if it's indented more than 3 spaces, it should be a code block
    if (state.sCount[startLine] - state.blkIndent >= 4) { return false; }

    marker = state.src[pos++];

    // Check hr marker
    if (marker != 0x2A/* * */ &&
        marker != 0x2D/* - */ &&
        marker != 0x5F/* _ */) {
        return false;
    }

    // markers can be mixed with spaces, but there should be at least 3 of them

    cnt = 1;
    while (pos < max) {
        ch = state.src[pos++];
        if (ch != marker && !isSpace(ch)) { return false; }
        if (ch == marker) { cnt++; }
    }

    if (cnt < 3) { return false; }

    if (silent) { return true; }

    state.curLine = startLine + 1;

    Token t1("hr", "hr", 0);
    t1.map = std::make_pair(startLine, state.curLine);
    t1.markup = string(cnt + 1, marker);
    state.pushToken(t1);

    return true;
};