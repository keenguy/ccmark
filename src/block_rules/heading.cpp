//
// Created by yong gu on 01/10/2018.
//
#include "block_rules.h"

namespace ccm{
    bool heading(BlockState &state, int startLine, int endLine, bool);
}

bool ccm::heading(BlockState &state, int startLine, int endLine, bool silent) {
    int pos = state.bMarks[startLine] + state.tShift[startLine];
    int max = state.eMarks[startLine];

    // if it's indented more than 3 spaces, it should be a code block
    if (state.sCount[startLine] - state.blkIndent >= 4) { return false; }

    char ch = state.src[pos];

    if (ch != 0x23/* # */ || pos >= max) { return false; }

    // count heading level
    int level = 1;
    ch = state.src[++pos];
    while (ch == 0x23/* # */ && pos < max && level <= 6) {
        level++;
        ch = state.src[++pos];
    }

    if (level > 6 || (pos < max && !isSpace(ch))) { return false; }

    if (silent) { return true; }

    // Let's cut tails like '    ###  ' from the end of string

    max = state.skipSpacesBack(max, pos);
    int tmp = state.skipCharsBack(max, 0x23, pos); // #
    if (tmp > pos && isSpace(state.src[tmp - 1])) {
        max = tmp;
    }

    state.curLine = startLine + 1;

    Token t1("heading_open", 'h' + std::to_string(level), 1);
    t1.markup = std::string(level, '#');
    t1.map = std::make_pair(startLine, state.curLine);
    state.pushToken(t1);

    Token t2("inline", "", 0);
    t2.content = boost::trim_copy(state.src.substr(pos, max - pos));
    t2.map = std::make_pair(startLine, state.curLine);
    state.pushToken(t2);

    Token t3("heading_close", 'h' + std::to_string(level), -1);
    t3.markup = std::string(level, '#');
    state.pushToken(t3);

    return true;
}