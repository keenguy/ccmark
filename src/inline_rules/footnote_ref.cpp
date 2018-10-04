//
// Created by yong gu on 04/10/2018.
//

#include "inline_rules.h"

namespace ccm {
    bool footnote_ref(InlineState &state, bool silent);
}

bool ccm::footnote_ref(InlineState &state, bool silent) {
//    var label,
//            pos,
//            footnoteId,
//            footnoteSubId,
//            token,
    int max = state.posMax;
    int start = state.pos;

    // should be at least 4 chars - "[^x]"
    if (start + 4 > max) { return false; }

//    if (!state.env.footnotes || !state.env.footnotes.refs) { return false; }
    if (state.src[start] != 0x5B/* [ */) { return false; }
    if (state.src[start + 1] != 0x5E/* ^ */) { return false; }

    int pos = 0;
    for (pos = start + 2; pos < max; pos++) {
        if (state.src[pos] == 0x20) { return false; }
        if (state.src[pos] == 0x0A) { return false; }
        if (state.src[pos] == 0x5D /* ] */) {
            break;
        }
    }

    if (pos == start + 2) { return false; } // no empty footnote labels
    if (pos >= max) { return false; }
    pos++;

    string label = state.src.substr(start + 2, pos - start - 3);
    auto& footnoteIds = state.coreState.footnoteIds;
    if (footnoteIds.footnoteForLabel.find(label) == footnoteIds.footnoteForLabel.end()) { return false; }
    int footnoteId = 0, footnoteSubId = 0;
    auto &fn = footnoteIds.footnoteForLabel[label];
    if (!silent) {
        if (fn.id < 0) {
            footnoteId = footnoteIds.referedLabels.size();    // footnoteId starts at 0
            footnoteIds.referedLabels.push_back (label);
            fn.id = footnoteId;
        } else {
            footnoteId = fn.id;
        }

        footnoteSubId = fn.count;   //subId also starts at 0
        fn.count++;

        Token token("footnote_ref", "", 0);
        token.meta = {footnoteId, footnoteSubId, label};
        state.push(token);
    }

    state.pos = pos;
    state.posMax = max;
    return true;
}