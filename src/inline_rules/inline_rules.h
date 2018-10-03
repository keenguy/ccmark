//
// Created by yong gu on 28/09/2018.
//

#ifndef CPP_MARKDOWN_INLINE_RULES_H
#define CPP_MARKDOWN_INLINE_RULES_H

#include <vector>
#include <unordered_map>

#include "../InlineParser.h"
#include "../helpers/common.h"

namespace ccm {
    class InlineState;

    typedef bool (* InlineRule)(InlineState& state, bool silent);
    typedef void (* InlinePostRule)(InlineState& state);

    std::vector<InlineRule> getInlineRules();
    std::vector<InlinePostRule> getInlinePostRules();

    //auxiliary data and function
    extern std::vector<int> ESCAPED;
    extern std::string ESCAPE_CHARS;
    int parseLinkLabel(InlineState &state, int start, bool disableNested = false);

}

#endif //CPP_MARKDOWN_INLINE_RULES_H
