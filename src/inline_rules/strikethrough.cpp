//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"

bool ccm::strikethrough(InlineState &state, bool silent) {
//            var i, scanned, token, len, ch,
    int start = state.pos;
    char marker = state.src[start];

    if (silent) {
        return false;
    }

    if (marker != 0x7E/* ~ */) {
        return false;
    }

    auto scanned = state.scanDelims(state.pos, true);
    int len = scanned.length;
    char ch = marker;

    if (len < 2) {
        return false;
    }

    if (len % 2) {
        Token token("text", "", 0);
        token.content = ch;
        state.push(token);
        len--;
    }

    for (int i = 0; i < len; i += 2) {
        Token token("text", "", 0);
        token.content = std::string(2, ch);
        state.push(token);
        int tokenIndex = (int) state.tokens.size() - 1;
        state.delimiters.push_back({
                                           marker,
                                           -1, //length
                                           i,
                                           tokenIndex, //token
                                           state.level,
                                           -1,
                                           scanned.can_open,
                                           scanned.can_close
                                   });
    }

    state.pos += scanned.length;

    return true;
}

void ccm::strikethrough_post(InlineState &state) {
//            var i, j,
//                    startDelim,
//                    endDelim,
//                    token,
//                    loneMarkers = []

    auto &delimiters = state.delimiters;
    int max = state.delimiters.size();
    std::vector<int> loneMarkers;
    for (int i = 0; i < max; i++) {
        auto startDelim = delimiters[i];

        if (startDelim.marker != 0x7E/* ~ */) {
            continue;
        }

        if (startDelim.end == -1) {
            continue;
        }

        auto endDelim = delimiters[startDelim.end];

        auto &token = state.tokens[startDelim.token];
        token.type = "s_open";
        token.tag = "s";
        token.nesting = 1;
        token.markup = "~~";
        token.content = "";

        auto &token2 = state.tokens[endDelim.token];
        token2.type = "s_close";
        token2.tag = "s";
        token2.nesting = -1;
        token2.markup = "~~";
        token2.content = "";

        if (state.tokens[endDelim.token - 1].type == "text" &&
            state.tokens[endDelim.token - 1].content == "~") {

            loneMarkers.push_back(endDelim.token - 1);
        }
    }

    // If a marker sequence has an odd number of characters, it's splitted
    // like this: `~~~~~` -> `~` + `~~` + `~~`, leaving one marker at the
    // start of the sequence.
    //
    // So, we have to move all those markers after subsequent s_close tags.
    //
    int i, j;
    while (!loneMarkers.empty()) {
        i = loneMarkers.back();
        loneMarkers.pop_back();
        j = i + 1;

        while (j < state.tokens.size() && state.tokens[j].type == "s_close") {
            j++;
        }

        j--;

        if (i != j) {
            std::swap(state.tokens[i], state.tokens[j]);
        }
    }
}