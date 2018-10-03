//
// Created by yong gu on 01/10/2018.
//

#include "block_rules.h"

namespace ccm {
    bool blockquote(BlockState &state, int startLine, int endLine, bool);
}

bool ccm::blockquote(BlockState &state, int startLine, int endLine, bool silent) {
    int oldLineMax = state.maxLine;
    int pos = state.bMarks[startLine] + state.tShift[startLine];
    int max = state.eMarks[startLine];

    // if it's indented more than 3 spaces, it should be a code block
    if (state.sCount[startLine] - state.blkIndent >= 4) { return false; }

    // check the block quote marker
    if (state.src[pos++] != 0x3E/* > */) { return false; }

    // we know that it's going to be a valid blockquote,
    // so no point trying to find the end of it in silent mode
    if (silent) { return true; }

    // skip spaces after ">" and re-calculate offset
    int offset = state.sCount[startLine] + pos - (state.bMarks[startLine] + state.tShift[startLine]);
    int initial = offset;
    bool adjustTab = false, spaceAfterMarker = false;

    // skip one optional space after '>'
    if (state.src[pos] == 0x20 /* space */) {
        // ' >   test '
        //     ^ -- position start of line here:
        pos++;
        initial++;
        offset++;
        adjustTab = false;
        spaceAfterMarker = true;
    } else if (state.src[pos] == 0x09 /* tab */) {
        spaceAfterMarker = true;

        if ((state.bsCount[startLine] + offset) % 4 == 3) {
            // '  >\t  test '
            //       ^ -- position start of line here (tab has width==1)
            pos++;
            initial++;
            offset++;
            adjustTab = false;
        } else {
            // ' >\t  test '
            //    ^ -- position start of line here + shift bsCount slightly
            //         to make extra space appear
            adjustTab = true;
        }
    } else {
        spaceAfterMarker = false;
    }
    std::vector<int> oldBMarks{state.bMarks[startLine]};
    state.bMarks[startLine] = pos;
    char ch;
    while (pos < max) {
        ch = state.src[pos];

        if (isSpace(ch)) {
            if (ch == 0x09) {
                offset += 4 - (offset + state.bsCount[startLine] + (adjustTab ? 1 : 0)) % 4;
            } else {
                offset++;
            }
        } else {
            break;
        }

        pos++;
    }
    std::vector<int> oldBSCount{state.bsCount[startLine]};
    state.bsCount[startLine] = state.sCount[startLine] + 1 + (spaceAfterMarker ? 1 : 0);

    bool lastLineEmpty = (pos >= max);

    std::vector<int> oldSCount = {state.sCount[startLine]};
    state.sCount[startLine] = offset - initial;

    std::vector<int> oldTShift = {state.tShift[startLine]};
    state.tShift[startLine] = pos - state.bMarks[startLine];

//            terminatorRules = state.md.block.ruler.getRules('blockquote');

    string oldParentType = state.parentType;
    state.parentType = "blockquote";
    bool wasOutdented = false;

    // Search the end of the block
    //
    // Block ends with either:
    //  1. an empty line outside:
    //     ```
    //     > test
    //
    //     ```
    //  2. an empty line inside:
    //     ```
    //     >
    //     test
    //     ```
    //  3. another tag:
    //     ```
    //     > test
    //      - - -
    //     ```
    int nextLine = 0;
    for (nextLine = startLine + 1; nextLine < endLine; nextLine++) {
        // check if it's outdented, i.e. it's inside list item and indented
        // less than said list item:
        //
        // ```
        // 1. anything
        //    > current blockquote
        // 2. checking this line
        // ```
        if (state.sCount[nextLine] < state.blkIndent) wasOutdented = true;

        pos = state.bMarks[nextLine] + state.tShift[nextLine];
        max = state.eMarks[nextLine];

        if (pos >= max) {
            // Case 1: line is not inside the blockquote, and this line is empty.
            break;
        }

        if (state.src[pos++] == 0x3E/* > */ && !wasOutdented) {
            // This line is inside the blockquote.

            // skip spaces after ">" and re-calculate offset
            initial = offset = state.sCount[nextLine] + pos - (state.bMarks[nextLine] + state.tShift[nextLine]);

            // skip one optional space after '>'
            if (state.src[pos] == 0x20 /* space */) {
                // ' >   test '
                //     ^ -- position start of line here:
                pos++;
                initial++;
                offset++;
                adjustTab = false;
                spaceAfterMarker = true;
            } else if (state.src[pos] == 0x09 /* tab */) {
                spaceAfterMarker = true;

                if ((state.bsCount[nextLine] + offset) % 4 == 3) {
                    // '  >\t  test '
                    //       ^ -- position start of line here (tab has width==1)
                    pos++;
                    initial++;
                    offset++;
                    adjustTab = false;
                } else {
                    // ' >\t  test '
                    //    ^ -- position start of line here + shift bsCount slightly
                    //         to make extra space appear
                    adjustTab = true;
                }
            } else {
                spaceAfterMarker = false;
            }

            oldBMarks.push_back(state.bMarks[nextLine]);
            state.bMarks[nextLine] = pos;

            while (pos < max) {
                ch = state.src[pos];

                if (isSpace(ch)) {
                    if (ch == 0x09) {
                        offset += 4 - (offset + state.bsCount[nextLine] + (adjustTab ? 1 : 0)) % 4;
                    } else {
                        offset++;
                    }
                } else {
                    break;
                }

                pos++;
            }

            lastLineEmpty = pos >= max;

            oldBSCount.push_back(state.bsCount[nextLine]);
            state.bsCount[nextLine] = state.sCount[nextLine] + 1 + (spaceAfterMarker ? 1 : 0);

            oldSCount.push_back(state.sCount[nextLine]);
            state.sCount[nextLine] = offset - initial;

            oldTShift.push_back(state.tShift[nextLine]);
            state.tShift[nextLine] = pos - state.bMarks[nextLine];
            continue;
        }

        // Case 2: line is not inside the blockquote, and the last line was empty.
        if (lastLineEmpty) { break; }

        // Case 3: another tag found.
        bool terminate = false;
        for (auto terminatorRule: terminatorsOf["blockquote"]) {
            if ((*terminatorRule)(state, nextLine, endLine, true)) {
                terminate = true;
                break;
            }
        }
        if (terminate) {
            // Quirk to enforce "hard termination mode" for paragraphs;
            // normally if you call `tokenize(state, startLine, nextLine)`,
            // paragraphs will look below nextLine for paragraph continuation,
            // but if blockquote is terminated by another tag, they shouldn't
            state.maxLine = nextLine;

            if (state.blkIndent != 0) {
                // state.blkIndent was non-zero, we now set it to zero,
                // so we need to re-calculate all offsets to appear as
                // if indent wasn't changed
                oldBMarks.push_back(state.bMarks[nextLine]);
                oldBSCount.push_back(state.bsCount[nextLine]);
                oldTShift.push_back(state.tShift[nextLine]);
                oldSCount.push_back(state.sCount[nextLine]);
                state.sCount[nextLine] -= state.blkIndent;
            }

            break;
        }

        oldBMarks.push_back(state.bMarks[nextLine]);
        oldBSCount.push_back(state.bsCount[nextLine]);
        oldTShift.push_back(state.tShift[nextLine]);
        oldSCount.push_back(state.sCount[nextLine]);

        // A negative indentation means that this is a paragraph continuation
        //
        state.sCount[nextLine] = -1;
    }

    int oldIndent = state.blkIndent;
    state.blkIndent = 0;

    Token t1("blockquote_open", "blockquote", 1);
    t1.markup = ">";
    t1.map = std::make_pair(startLine, 0);
    int tokenIdx = state.tokens.size();
    state.pushToken(t1);

    state.blockParser.tokenize(state, startLine, nextLine);

    Token t2("blockquote_close", "blockquote", -1);
    t2.markup = ">";
    state.pushToken(t2);

    state.maxLine = oldLineMax;
    state.parentType = oldParentType;
    state.tokens[tokenIdx].map.second = state.curLine;

    // Restore original tShift; this might not be necessary since the parser
    // has already been here, but just to make sure we can do that.
    for (int i = 0; i < oldTShift.size(); i++) {
        state.bMarks[i + startLine] = oldBMarks[i];
        state.tShift[i + startLine] = oldTShift[i];
        state.sCount[i + startLine] = oldSCount[i];
        state.bsCount[i + startLine] = oldBSCount[i];
    }
    state.blkIndent = oldIndent;

    return true;
}
