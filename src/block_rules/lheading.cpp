//
// Created by yong gu on 01/10/2018.
//

#include "block_rules.h"

namespace ccm{
    bool lheading(BlockState &state, int startLine, int endLine, bool);
}

bool ccm::lheading(BlockState &state, int startLine, int endLine, bool) {
    int nextLine = startLine + 1;

    // if it's indented more than 3 spaces, it should be a code block
    if (state.sCount[startLine] - state.blkIndent >= 4) { return false; }

    string oldParentType = state.parentType;
    state.parentType = "paragraph"; // use paragraph to match terminatorRules

    int pos = 0, max = 0, level = 0;
    char marker;
    bool terminate = false;
    // jump line-by-line until empty one or EOF
    for (; nextLine < endLine && !state.isEmpty(nextLine); nextLine++) {
        // this would be a code block normally, but after paragraph
        // it's considered a lazy continuation regardless of what's there
        if (state.sCount[nextLine] - state.blkIndent > 3) { continue; }

        //
        // Check for underline in setext header
        //
        if (state.sCount[nextLine] >= state.blkIndent) {
            pos = state.bMarks[nextLine] + state.tShift[nextLine];
            max = state.eMarks[nextLine];

            if (pos < max) {
                marker = state.src[pos];

                if (marker == 0x2D/* - */ || marker == 0x3D/* = */) {
                    pos = state.skipChars(pos, marker);
                    pos = state.skipSpaces(pos);

                    if (pos >= max) {
                        level = (marker == 0x3D/* = */ ? 1 : 2);
                        break;
                    }
                }
            }
        }

        // quirk for blockquotes, this line should already be checked by that rule
        if (state.sCount[nextLine] < 0) { continue; }

        // Some tags can terminate paragraph without empty line.
        terminate = false;
        for (auto terminatorRule: terminatorsOf["paragraph"]) {
            if ((*terminatorRule)(state, nextLine, endLine, true)) {
                terminate = true;
                break;
            }
        }
        if (terminate) { break; }
    }

    if (level <= 0) {
        // Didn't find valid underline
        state.parentType = oldParentType;
        return false;
    }

    string content = boost::trim_copy(state.getLines(startLine, nextLine, state.blkIndent, false));

    state.curLine = nextLine + 1;

    Token t1("heading_open", 'h' + std::to_string(level), 1);
    t1.markup = string(1, marker);
    t1.map = std::make_pair(startLine, state.curLine);
    state.pushToken(t1);

    Token t2("inline", "", 0);
    t2.content = content;
    t2.map = std::make_pair(startLine, state.curLine - 1);
    state.pushToken(t2);

    Token t3("heading_close", 'h' + std::to_string(level), -1);
    t3.markup = string(1, marker);
    state.pushToken(t3);

    state.parentType = oldParentType;

    return true;
};