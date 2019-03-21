//
// Created by yong gu on 23/09/2018.
//

#include <tuple>
#include "block_rules.h"
#include "../BlockParser.h"

namespace ccm{
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

    bool math_block(BlockState &state, int startLine, int endLine, bool silent);

    bool footnote_def(BlockState &state, int startLine, int endLine, bool silent);


//    BlockRule heading, paragraph, code, fence, hr, lheading, reference, html_block, blockquote,
//                list, math_block, footnote_def;

    std::unordered_map<std::string, std::vector<BlockRule>> terminatorsOf{
            {"paragraph",  {fence, blockquote, hr, list, heading,
                                                                  html_block, math_block, footnote_def}},
            {"reference",  {fence, blockquote, hr, list, heading, html_block, math_block, footnote_def}},
            {"blockquote", {fence, blockquote, hr, list, heading, html_block, math_block}},
            {"list",       {fence, blockquote, hr, math_block}}

    };

    std::vector<BlockRule> getBlockRules() {
        std::vector<BlockRule> rules{code, fence, blockquote, math_block, hr, list, footnote_def, reference, heading,
                                     lheading, html_block, paragraph};
        return rules;
    }
}
