//
// Created by yong gu on 04/10/2018.
//

#include "block_rules.h"
#include "../BlockParser.h"

namespace ccm {
    bool footnote_def(BlockState &state, int startLine, int endLine, bool silent);
}

bool ccm::footnote_def(BlockState &state, int startLine, int endLine, bool silent) {

    int start = state.bMarks[startLine] + state.tShift[startLine];
    int max = state.eMarks[startLine];

    // line should be at least 5 chars - "[^x]:"
    if (start + 5 > max) { return false; }

    if (state.coreState.src[start] != 0x5B/* [ */) { return false; }
    if (state.coreState.src[start + 1] != 0x5E/* ^ */) { return false; }

    int pos = 0;
    for (pos = start + 2; pos < max; pos++) {
        if (state.coreState.src[pos] == 0x20) { return false; }
        if (state.coreState.src[pos] == 0x5D /* ] */) {
            break;
        }
    }

    if (pos == start + 2) { return false; } // no empty footnote labels
    if (pos + 1 >= max || state.coreState.src[++pos] != 0x3A /* : */) { return false; }
    if (silent) { return true; }
    pos++;

//    if (!state.env.footnotes) { state.env.footnotes = {}; }
//    if (!state.env.footnotes.refs) { state.env.footnotes.refs = {}; }
    string label = state.coreState.src.substr(start + 2, pos - start - 4);
    auto &footnoteIds = state.coreState.footnoteIds;
    footnoteIds.footnoteForLabel[label] = Footnote(label);

    int openIdx = state.coreState.tokens.size();
    Token token("footnote_reference_open", "", 1);
    token.meta.label = label;
//    token.level = state.level++;
    state.pushToken(token);

    int oldBMark = state.bMarks[startLine];
    int oldTShift = state.tShift[startLine];
    int oldSCount = state.sCount[startLine];
    string oldParentType = state.parentType;

    int posAfterColon = pos;
    int initial = state.sCount[startLine] + pos - (state.bMarks[startLine] + state.tShift[startLine]);
    int offset = initial;
    char ch;
    while (pos < max) {
        ch = state.coreState.src[pos];

        if (isSpace(ch)) {
            if (ch == 0x09) {
                offset += 4 - offset % 4;
            } else {
                offset++;
            }
        } else {
            break;
        }

        pos++;
    }

    state.tShift[startLine] = pos - posAfterColon;
    state.sCount[startLine] = offset - initial;

    state.bMarks[startLine] = posAfterColon;
    state.blkIndent += 4;

    state.parentType = "footnote";

    if (state.sCount[startLine] < state.blkIndent) {
        state.sCount[startLine] += state.blkIndent;
    }

    state.coreState.blockParser.tokenize(state, startLine, endLine);

    state.parentType = oldParentType;
    state.blkIndent -= 4;
    state.tShift[startLine] = oldTShift;
    state.sCount[startLine] = oldSCount;
    state.bMarks[startLine] = oldBMark;

    Token token2("footnote_reference_close", "", -1);
//    token2.level = --state.level;
    state.pushToken(token2);


    std::vector<Token> tokens(state.coreState.tokens.begin()+openIdx+1, state.coreState.tokens.end()-1);
    footnoteIds.footnoteForLabel[label].tokens = std::move(tokens);
    state.coreState.tokens.resize(openIdx);
    return true;
}