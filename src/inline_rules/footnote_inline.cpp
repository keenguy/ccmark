//
// Created by yong gu on 05/10/2018.
//

#include "inline_rules.h"
#include "../InlineParser.h"

namespace ccm {
    bool footnote_inline(InlineState &state, bool silent);
}

// Process inline footnotes (^[...])
bool ccm::footnote_inline(InlineState &state, bool silent) {
//    var labelStart,
//        labelEnd,
//        footnoteId,
//        token,
//        tokens,
    int max = state.posMax;
    int start = state.pos;

    if (start + 2 >= max) { return false; }
    if (state.src[start] != 0x5E/* ^ */) { return false; }
    if (state.src[start + 1] != 0x5B/* [ */) { return false; }

    int labelStart = start + 2;
    int labelEnd = parseLinkLabel(state, start + 1);

    // parser failed to find ']', so it's not a valid note
    if (labelEnd < 0) { return false; }

    // We found the end of the link, and know for a fact it's a valid link;
    // so all that's left to do is to call tokenizer.
    //
    auto &ids = state.coreState.footnoteIds;
    if (!silent) {
        int footnoteId = ids.referedLabels.size();
        string label {std::to_string(footnoteId)};
        Footnote fn(label);
        state.coreState.inlineParser.parse(state.src.substr(labelStart, labelEnd - labelStart), fn.tokens, state.coreState);
        ids.footnoteForLabel[label] = fn;
        ids.referedLabels.push_back(label);

        Token token("footnote_ref", "", 0);
        token.meta = {footnoteId,-1,label};
        state.push(token);
    }

    state.pos = labelEnd + 1;
    state.posMax = max;
    return true;
}