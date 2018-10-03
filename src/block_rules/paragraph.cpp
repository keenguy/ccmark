//
// Created by yong gu on 01/10/2018.
//
#include "block_rules.h"

bool ccm::paragraph(BlockState &state, int startLine, int, bool) {

    int nextLine = startLine + 1;
    int endLine = state.maxLine;
//            terminatorRules = state.md.block.ruler.getRules("paragraph"),

    std::string oldParentType = state.parentType;
    state.parentType = "paragraph";

    bool terminate = false;
    // jump line-by-line until empty one or EOF
    for (; nextLine < endLine && !state.isEmpty(nextLine); nextLine++) {
        // this would be a code block normally, but after paragraph
        // it's considered a lazy continuation regardless of what's there
        if (state.sCount[nextLine] - state.blkIndent > 3) { continue; }

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
    std::string content = state.getLines(startLine, nextLine, state.blkIndent, false);
    boost::trim(content);

    state.curLine = nextLine;


    Token t1("paragraph_open", "p", 1);
    t1.map = std::make_pair(startLine, state.curLine);
    state.pushToken(t1);


    Token t2("inline", "", 0);
    t2.content = content;
    t2.map = std::make_pair(startLine, state.curLine);
    state.pushToken(t2);
//            token.children = [];

    Token t3("paragraph_close", "p", -1);
    state.pushToken(t3);

    state.parentType = oldParentType;

    return true;
}