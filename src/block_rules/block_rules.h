//
// Created by yong gu on 23/09/2018.
//

#ifndef CPP_MARKDOWN_BLOCK_RULES_H
#define CPP_MARKDOWN_BLOCK_RULES_H

#include <vector>
#include <unordered_map>
#include "../BlockParser.h"
#include "../helpers/common.h"


namespace ccm {

    typedef bool (* BlockRule)(BlockState& state, int startLine, int endLine, bool silent);
    extern std::unordered_map<std::string, std::vector<BlockRule>> terminatorsOf;

    std::vector<BlockRule> getBlockRules();

    bool heading(BlockState &state, int startLine, int endLine, bool silent);
    bool paragraph(BlockState &state, int startLine, int, bool);
    bool code(BlockState &state, int startLine, int endLine, bool);
    bool fence(BlockState &state, int startLine, int endLine, bool);
    bool hr(BlockState &state, int startLine, int endLine, bool);
    bool lheading(BlockState &state, int startLine, int endLine, bool);
    bool reference(BlockState &state, int startLine, int endLine, bool);
    bool html_block(BlockState &state, int startLine, int endLine, bool);
    bool blockquote(BlockState &state, int startLine, int endLine, bool);
    bool list(BlockState &state, int startLine, int endLine, bool);

    typedef std::tuple<boost::regex, boost::regex, bool> HTMLPARSER;
    extern std::vector<HTMLPARSER> HTML_SEQUENCES;
    extern std::vector <std::string> html_blocks;


}

#endif //CPP_MARKDOWN_BLOCK_RULES_H
