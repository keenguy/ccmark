//
// Created by yong gu on 03/10/2018.
//

#include "block_rules.h"

namespace ccm{
    bool math_block(BlockState &state, int startLine, int endLine, bool);
}

bool ccm::math_block(BlockState &state, int startLine, int endLine, bool silent) {
//    var firstLine, lastLine, nextLine, lastPos, found = false, token,
    string::size_type pos = state.bMarks[startLine] + state.tShift[startLine],
    max = state.eMarks[startLine];

    if (pos + 2 > max) { return false; }
    if (state.coreState.src.substr(pos, 2) != "$$") { return false; }

    pos += 2;
    string firstLine = state.coreState.src.substr(pos, max - pos);

    if (silent) { return true; }
    bool found = false;
    string trimed{boost::trim_copy(firstLine)};
    if (trimed.substr(trimed.length() - 2) == "$$") {
        // Single line expression
        firstLine = trimed.substr(0, trimed.length() - 2);
        found = true;
    }
    int nextLine = 0;
    string::size_type lastPos = 0;
    string lastLine;
    for (nextLine = startLine; !found;) {

        nextLine++;

        if (nextLine >= endLine) { break; }

        pos = state.bMarks[nextLine] + state.tShift[nextLine];
        max = state.eMarks[nextLine];

        if (pos < max && state.tShift[nextLine] < state.blkIndent) {
            // non-empty line with negative indent should stop the list:
            break;
        }
        trimed = boost::trim_copy(state.coreState.src.substr(pos, max - pos));
        if (trimed.substr(trimed.length() - 2) == "$$") {
            lastPos = state.coreState.src.substr(0, max).rfind("$$");
            lastLine = state.coreState.src.substr(pos, lastPos - pos);
            found = true;
        }

    }

    state.curLine = nextLine + 1;

    Token token("math_block", "math", 0);
    token.isBlock = true;
    token.content = (!(boost::trim_copy(firstLine).empty()) ? firstLine + "\n" : "")
                    + state.getLines(startLine + 1, nextLine, state.tShift[startLine], true)
                    + (!(boost::trim_copy(lastLine).empty()) ? lastLine : "");
    token.map = std::make_pair(startLine, state.curLine);
    token.markup = "$$";
    state.pushToken(token);
    return true;
}