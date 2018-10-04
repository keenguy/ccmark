//
// Created by yong gu on 28/09/2018.
//

#include "InlineParser.h"
#include "inline_rules/inline_rules.h"

namespace ccm {

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
        if (state.level < state.coreState.options.maxNesting) {
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
        int maxNesting = state.coreState.options.maxNesting;

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

    void InlineParser::parse(const std::string& str, std::vector<Token>& outTokens, CoreState &coreState) const{
        InlineState state(str, outTokens, coreState);

        tokenize(state);

        int len = postRules.size();

        for (int i = 0; i < len; i++) {
            postRules[i](state);
        }
    }
}