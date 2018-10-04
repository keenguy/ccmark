//
// Created by yong gu on 04/10/2018.
//

#include "InlineState.h"
#include "../helpers/common.h"

namespace ccm{
    void InlineState::pushPending() {
        Token token("text", "", 0);
        token.content = pending;
        token.level = pendingLevel;
        tokens.push_back(std::move(token));
        pending = "";
    }

    InlineState::Delim ccm::InlineState::scanDelims(int start, bool canSplitWord) {
//    var pos = start, lastChar, nextChar, count, can_open, can_close,
//            isLastWhiteSpace, isLastPunctChar,
//            isNextWhiteSpace, isNextPunctChar,
//            left_flanking = true,
//            right_flanking = true,
        char marker = src[start];
        int max = posMax;
        int pos = start;
        // treat beginning of the line as a whitespace
        char lastChar = start > 0 ? src[start - 1] : 0x20;

        while (pos < max && src[pos] == marker) { pos++; }

        int count = pos - start;

        // treat end of the line as a whitespace
        char nextChar = pos < max ? src[pos] : 0x20;

        bool isLastPunctChar = isMdAsciiPunct(lastChar) || isPunctChar(lastChar);
        bool isNextPunctChar = isMdAsciiPunct(nextChar) || isPunctChar(nextChar);

        bool isLastWhiteSpace = isWhiteSpace(lastChar);
        bool isNextWhiteSpace = isWhiteSpace(nextChar);
        bool left_flanking = true;
        bool right_flanking = true;
        if (isNextWhiteSpace) {
            left_flanking = false;
        } else if (isNextPunctChar) {
            if (!(isLastWhiteSpace || isLastPunctChar)) {
                left_flanking = false;
            }
        }

        if (isLastWhiteSpace) {
            right_flanking = false;
        } else if (isLastPunctChar) {
            if (!(isNextWhiteSpace || isNextPunctChar)) {
                right_flanking = false;
            }
        }

        bool can_open;
        bool can_close;
        if (!canSplitWord) {
            can_open = left_flanking && (!right_flanking || isLastPunctChar);
            can_close = right_flanking && (!left_flanking || isNextPunctChar);
        } else {
            can_open = left_flanking;
            can_close = right_flanking;
        }

        return {can_open, can_close, count};
    }


    Token& InlineState::push(const Token& t) {
        Token token {t};
        if (!pending.empty()) {
            pushPending();
        }

        if (token.nesting < 0) { level--; }
        token.level = level;
        if (token.nesting > 0) { level++; }

        pendingLevel = level;
        tokens.push_back(std::move(token));
        return tokens.back();
    }
}