//
// Created by yong gu on 28/09/2018.
//

#ifndef CPP_MARKDOWN_INLINEPARSER_H
#define CPP_MARKDOWN_INLINEPARSER_H

#include <string>
#include "token.h"
#include "options.h"
#include "LinkIds.h"

namespace ccm {
    class InlineParser;

    class InlineState {
    public:
        InlineState(const std::string &src, std::vector<Token> &outTokens, const InlineParser &inlineParser,
                    LinkIds &linkIds, const Options &options) : src(src),
                                                                inlineParser(
                                                                        inlineParser),
                                                                        options(options),
                                                                linkIds(linkIds),
                                                                tokens(outTokens),
                                                                posMax(src.length()) {};

        const std::string &src;
        LinkIds &linkIds;
        const Options &options;
        std::vector<Token> &tokens;
        const InlineParser &inlineParser;

        size_t pos = 0;
        size_t posMax;
        int level = 0;
        std::unordered_map<int, int> cache;   //jump cache used for skipToken

        std::string pending;
        int pendingLevel = 0;

        void pushPending();

        Token& push(const Token& token);

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

    typedef bool (* InlineRule)(InlineState& state, bool silent);
    typedef void (* InlinePostRule)(InlineState& state);

    class InlineParser {
    public:
        InlineParser();

        void tokenize(InlineState &state) const;

        void
        parse(const std::string &str, std::vector<Token> &outTokens, LinkIds &linkIds, const Options &options) const;

        void skipToken(InlineState &state) const;

        Options options;
        std::vector<InlineRule> rules;
        std::vector<InlinePostRule> postRules;
    };
}


#endif //CPP_MARKDOWN_INLINEPARSER_H
