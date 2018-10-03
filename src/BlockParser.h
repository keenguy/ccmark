//
// Created by yong gu on 22/09/2018.
//

#ifndef CPP_MARKDOWN_BLOCKPARSER_H
#define CPP_MARKDOWN_BLOCKPARSER_H

#include <vector>
#include <string>

#include "token.h"
#include "LinkIds.h"
#include "options.h"

namespace ccm {
    class CoreState;

    class BlockParser;

    class BlockState {
    public:
        BlockState(std::string &src, Options &options, std::vector<Token> &tokens, LinkIds &linkIds,
                   const BlockParser &blockParser);


        Token &pushToken(const Token &token);

        bool isEmpty(int line);

        int skipEmptyLines(int from);

        int skipSpaces(int from);

        int skipSpacesBack(int from, int end);

        int skipChars(int from, char code);

        int skipCharsBack(int from, int end, char code);

        std::string getLines(int begin, int end, int indent, bool keepLastLF);

        const BlockParser &blockParser;
        std::string &src;
        std::vector<Token> &tokens;
        LinkIds &linkIds;
        Options &options;

        std::string parentType;
        std::vector<int> bMarks;
        std::vector<int> eMarks;
        std::vector<int> tShift;
        std::vector<int> sCount;
        std::vector<int> bsCount;

        int blkIndent = 0;
        int curLine = 0;
        int maxLine = 0;
        bool tight = false;
        int level = 0;
        std::string result;
    };

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
