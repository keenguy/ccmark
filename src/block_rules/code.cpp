//
// Created by yong gu on 01/10/2018.
//

#include "block_rules.h"

namespace ccm{
    bool code(BlockState &state, int startLine, int endLine, bool);
}

bool ccm::code(BlockState &state, int startLine, int endLine, bool) {
    int nextLine, last;

    if (state.sCount[startLine] - state.blkIndent < 4) {
        return false;
    }

    last = nextLine = startLine + 1;

    while (nextLine < endLine) {
        if (state.isEmpty(nextLine)) {
            nextLine++;
            continue;
        }

        if (state.sCount[nextLine] - state.blkIndent >= 4) {
            nextLine++;
            last = nextLine;
            continue;
        }
        break;
    }
    state.curLine = last;

    Token t1("code_block", "code", 0);
    t1.content = state.getLines(startLine, last, 4 + state.blkIndent, true);
    t1.map = std::make_pair(startLine, state.curLine);
    state.pushToken(t1);
    return true;
}