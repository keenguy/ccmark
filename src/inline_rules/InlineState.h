//
// Created by yong gu on 04/10/2018.
//

#ifndef CCMARK_INLINESTATE_H
#define CCMARK_INLINESTATE_H

#include <string>
#include <vector>
#include "../token.h"

namespace ccm{
    class CoreState;
    class InlineState {
    public:
        InlineState(const std::string &src, std::vector<Token> &outTokens, CoreState &coreState) :
                src(src),
                tokens(outTokens),
                coreState(coreState),
                posMax(src.length()) {};

        const std::string &src;
        std::vector<Token> &tokens;
        CoreState &coreState;

        size_t pos = 0;
        size_t posMax;
        int level = 0;
        std::unordered_map<int, int> cache;   //jump cache used for skipToken

        std::string pending;
        int pendingLevel = 0;

        void pushPending();

        Token &push(const Token &token);

        // Scan a sequence of emphasis-like markers, and determine whether
// it can start an emphasis sequence or end an emphasis sequence.
//
//  - start - position to scan from (it should point at a valid marker);
//  - canSplitWord - determine if these markers can be found inside a word
//
        struct Delim {
            bool can_open;
            bool can_close;
            int length;
        };

        Delim scanDelims(int start, bool canSplitWord);

        struct Delimiter {
            char marker;
            int length = -1;
            int jump;
            int token;
            int level;
            int end;
            bool open;
            bool close;
        };

        std::vector<Delimiter> delimiters;

    };
}


#endif //CCMARK_INLINESTATE_H
