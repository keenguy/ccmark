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

    //inline rules
    bool text(InlineState &state, bool silent);
    bool newline(InlineState &state, bool silent);
    bool escape(InlineState &state, bool silent);
    bool backtick(InlineState &state, bool silent);
    bool strikethrough(InlineState &state, bool silent);
    void strikethrough_post(InlineState &state);
    bool emphasis(InlineState &state, bool silent);
    void emphasis_post(InlineState &state);
    bool link(InlineState &state, bool silent);
    bool image(InlineState &state, bool silent);
    bool autolink(InlineState &state, bool silent);
    bool html_inline(InlineState &state, bool silent);
    bool entity(InlineState &state, bool silent);

    //global post rules
    void link_pairs(InlineState &state);
    void text_collapse(InlineState &state);

    //auxiliary data and function
    extern std::vector<int> ESCAPED;
    extern std::string ESCAPE_CHARS;
    int parseLinkLabel(InlineState &state, int start, bool disableNested = false);

}

#endif //CPP_MARKDOWN_INLINE_RULES_H
