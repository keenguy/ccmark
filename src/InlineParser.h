//
// Created by yong gu on 28/09/2018.
//

#ifndef CPP_MARKDOWN_INLINEPARSER_H
#define CPP_MARKDOWN_INLINEPARSER_H

#include <string>
#include "token.h"

namespace ccm {
    class InlineState;

    class CoreState;

    typedef bool (*InlineRule)(InlineState &state, bool silent);

    typedef void (*InlinePostRule)(InlineState &state);

    class InlineParser {
    public:
        InlineParser();

        void tokenize(InlineState &state) const;

        void
        parse(const std::string &str, std::vector<Token> &outTokens, CoreState &coreState) const;

        void skipToken(InlineState &state) const;

        std::vector<InlineRule> rules;
        std::vector<InlinePostRule> postRules;
    };
}


#endif //CPP_MARKDOWN_INLINEPARSER_H
