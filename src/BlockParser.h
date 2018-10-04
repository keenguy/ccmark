//
// Created by yong gu on 22/09/2018.
//

#ifndef CPP_MARKDOWN_BLOCKPARSER_H
#define CPP_MARKDOWN_BLOCKPARSER_H

#include <vector>
#include <string>

namespace ccm {
    class CoreState;
    class Token;
    class BlockState;

    typedef bool (*BlockRule)(BlockState &state, int startLine, int endLine, bool silent);

    class BlockParser {
    public:
        BlockParser();

        void tokenize(BlockState &state, int startLine, int endLine) const;

        void parse(CoreState &state) const;

        std::vector<BlockRule> rules;

    };
}

#endif //CPP_MARKDOWN_BLOCKPARSER_H
