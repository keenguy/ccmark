//
// Created by yong gu on 23/09/2018.
//

#ifndef CPP_MARKDOWN_BLOCK_RULES_H
#define CPP_MARKDOWN_BLOCK_RULES_H

#include <vector>
#include <unordered_map>

#include "../helpers/common.h"
#include "../CoreState.h"
#include "BlockState.h"


namespace ccm {

    typedef bool (* BlockRule)(BlockState& state, int startLine, int endLine, bool silent);
    extern std::unordered_map<std::string, std::vector<BlockRule>> terminatorsOf;

    std::vector<BlockRule> getBlockRules();

    typedef std::tuple<boost::regex, boost::regex, bool> HTMLPARSER;
    extern std::vector<HTMLPARSER> HTML_SEQUENCES;
    extern std::vector <std::string> html_blocks;
}

#endif //CPP_MARKDOWN_BLOCK_RULES_H
