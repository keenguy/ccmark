//
// Created by yong gu on 28/09/2018.
//

#include "InlineParser.h"
#include "inline_rules/inline_rules.h"

namespace ccm {
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


    InlineParser::InlineParser() {
        rules = getInlineRules();
        postRules = getInlinePostRules();
    }

    void InlineParser::skipToken(InlineState &state) const{
        int pos = state.pos;
        auto& cache = state.cache;


        if (cache.find(pos) != cache.end()) {
            state.pos = cache[pos];
            return;
        }

        bool ok = false;
        auto len = rules.size();
        if (state.level < options.maxNesting) {
            for (int i = 0; i < len; i++) {
                // Increment state.level and decrement it later to limit recursion.
                // It's harmless to do here, because no tokens are created. But ideally,
                // we'd need a separate private state variable for this purpose.
                //
                state.level++;
                ok = rules[i](state, true);
                state.level--;

                if (ok) { break; }
            }
        } else {
            // Too much nesting, just skip until the end of the paragraph.
            //
            // NOTE: this will cause links to behave incorrectly in the following case,
            //       when an amount of `[` is exactly equal to `maxNesting + 1`:
            //
            //       [[[[[[[[[[[[[[[[[[[[[foo]()
            //
            // TODO: remove this workaround when CM standard will allow nested links
            //       (we can replace it by preventing links from being parsed in
            //       validation mode)
            //
            state.pos = state.posMax;
        }

        if (!ok) { state.pos++; }
        cache[pos] = state.pos;
    }

    void InlineParser::tokenize(InlineState &state) const {
        int len = rules.size();
        int end = state.posMax;
        int maxNesting = options.maxNesting;

        bool ok;
        while (state.pos < end) {
            // Try all possible rules.
            // On success, rule should:
            //
            // - update `state.pos`
            // - update `state.tokens`
            // - return true

            if (state.level < maxNesting) {
                for (int i = 0; i < len; i++) {
                    ok = rules[i](state, false);
                    if (ok) { break; }
                }
            }

            if (ok) {
                if (state.pos >= end) { break; }
                continue;
            }

            state.pending += state.src[state.pos++];
        }

        if (!state.pending.empty()) {
            state.pushPending();
        }

    }

    void InlineParser::parse(const std::string& str, std::vector<Token>& outTokens, LinkIds& linkIds, const Options& options) const{
        InlineState state(str, outTokens, *this, linkIds, options);

        tokenize(state);

        int len = postRules.size();

        for (int i = 0; i < len; i++) {
            postRules[i](state);
        }
    }
}