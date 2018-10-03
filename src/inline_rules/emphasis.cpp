//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"

namespace ccm{
    bool emphasis(InlineState &state, bool silent);
    void emphasis_post(InlineState &state);
}

bool ccm::emphasis(InlineState &state, bool silent) {
//            var i, scanned, token,
    int start = state.pos;
    char marker = state.src[start];

    if (silent) { return false; }

    if (marker != 0x5F /* _ */ && marker != 0x2A /* * */) { return false; }

    auto scanned = state.scanDelims(state.pos, marker == 0x2A);

    for (int i = 0; i < scanned.length; i++) {
        Token token("text", "", 0);
        token.content = std::string(1, marker);
        state.push(token);

        state.delimiters.push_back({
                                           // Char code of the starting marker (number).
                                           //
                                           .marker = marker,

                                           // Total length of these series of delimiters.
                                           //
                                           .length = scanned.length,

                                           // An amount of characters before this one that's equivalent to
                                           // current one. In plain English: if this delimiter does not open
                                           // an emphasis, neither do previous `jump` characters.
                                           //
                                           // Used to skip sequences like "*****" in one step, for 1st asterisk
                                           // value will be 0, for 2nd it's 1 and so on.
                                           //
                                           .jump = i,

                                           // A position of the token this delimiter corresponds to.
                                           //
                                           .token =  (int) state.tokens.size() - 1,

                                           // Token level.
                                           //
                                           .level =  state.level,

                                           // If this delimiter is matched as a valid opener, `end` will be
                                           // equal to its position, otherwise it's `-1`.
                                           //
                                           .end =  -1,

                                           // Boolean flags that determine if this delimiter could open or close
                                           // an emphasis.
                                           //
                                           .open =   scanned.can_open,
                                           .close =  scanned.can_close
                                   });
    }

    state.pos += scanned.length;

    return true;
}

void ccm::emphasis_post(InlineState &state) {
//            var i,
//                    endDelim,
//                    token,
//                    ch,
//                    isStrong,
    char ch;
    bool isStrong;
    auto &delimiters = state.delimiters;

    int max = state.delimiters.size();

    for (int i = max - 1; i >= 0; i--) {
        auto &startDelim = delimiters[i];

        if (startDelim.marker != 0x5F/* _ */ && startDelim.marker != 0x2A/* * */) {
            continue;
        }

        // Process only opening markers
        if (startDelim.end == -1) {
            continue;
        }

        auto &endDelim = delimiters[startDelim.end];

        // If the previous delimiter has the same marker and is adjacent to this one,
        // merge those into one strong delimiter.
        //
        // `<em><em>whatever</em></em>` -> `<strong>whatever</strong>`
        //
        isStrong = i > 0 &&
                   delimiters[i - 1].end == startDelim.end + 1 &&
                   delimiters[i - 1].token == startDelim.token - 1 &&
                   delimiters[startDelim.end + 1].token == endDelim.token + 1 &&
                   delimiters[i - 1].marker == startDelim.marker;

        ch = startDelim.marker;

        auto &token = state.tokens[startDelim.token];
        token.type = isStrong ? "strong_open" : "em_open";
        token.tag = isStrong ? "strong" : "em";
        token.nesting = 1;
        token.markup = isStrong ? std::string(2, ch) : std::string(1, ch);
        token.content = "";

        auto &token2 = state.tokens[endDelim.token];
        token2.type = isStrong ? "strong_close" : "em_close";
        token2.tag = isStrong ? "strong" : "em";
        token2.nesting = -1;
        token2.markup = isStrong ? std::string(2, ch) : std::string(1, ch);
        token2.content = "";

        if (isStrong) {
            state.tokens[delimiters[i - 1].token].content = "";
            state.tokens[delimiters[startDelim.end + 1].token].content = "";
            i--;
        }
    }
}