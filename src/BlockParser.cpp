//
// Created by yong gu on 22/09/2018.
//

#include "BlockParser.h"
#include "block_rules/BlockState.h"
#include "helpers/common.h"
#include "block_rules/block_rules.h"
#include "ccmark.h"
#include <numeric>

namespace ccm {


    BlockParser::BlockParser() {
        rules = getBlockRules();
    }

    void BlockParser::parse(CoreState &coreState) const {
        BlockState blockState(coreState);
        tokenize(blockState, blockState.curLine, blockState.maxLine);
    }

    void BlockParser::tokenize(BlockState &state, int startLine, int endLine) const {
        bool ok;
        int i;

        int len = rules.size();
        int line = startLine;
        bool hasEmptyLines = false;
        int maxNesting = state.coreState.options.maxNesting;

        while (line < endLine) {
            state.curLine = line = state.skipEmptyLines(line);
            if (line >= endLine) { break; }

            // Termination condition for nested calls.
            // Nested calls currently used for blockquotes & lists (which implies this function can be called recursively)
            if (state.sCount[line] < state.blkIndent) { break; }

            // If nesting level exceeded - skip tail to the end. That's not ordinary
            // situation and we should not care about content.
            if (state.level >= maxNesting) {
                state.curLine = endLine;
                break;
            }

            // Try all possible rules.
            // On success, rule should:
            //
            // - update `state.line`
            // - update `state.coreState.tokens`
            // - return true

            for (i = 0; i < len; i++) {
                ok = rules[i](state, line, endLine, false);
                if (ok) { break; }
            }

            // set state.tight if we had an empty line before current tag
            // i.e. latest empty line should not count
            state.tight = !hasEmptyLines;

            // paragraph might "eat" one newline after it in nested lists
            if (state.isEmpty(state.curLine - 1)) {
                hasEmptyLines = true;
            }

            line = state.curLine;

            if (line < endLine && state.isEmpty(line)) {
                hasEmptyLines = true;
                line++;
                state.curLine = line;
            }
        }
    }
}
