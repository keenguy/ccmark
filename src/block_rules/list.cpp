//
// Created by yong gu on 01/10/2018.
//

#include "block_rules.h"

namespace ccm{
    bool list(BlockState &state, int startLine, int endLine, bool);
}

namespace ccm {
// Search `[-+*][\n ]`, returns next pos after marker on success
// or -1 on fail.
    int skipBulletListMarker(const BlockState &state, int startLine) {
        char marker, ch;
        int pos = 0, max = 0;

        pos = state.bMarks[startLine] + state.tShift[startLine];
        max = state.eMarks[startLine];

        marker = state.src[pos++];
        // Check bullet
        if (marker != 0x2A/* * */ &&
            marker != 0x2D/* - */ &&
            marker != 0x2B/* + */) {
            return -1;
        }

        if (pos < max) {
            ch = state.src[pos];

            if (!isSpace(ch)) {
                // " -test " - is not a list item
                return -1;
            }
        }

        return pos;
    }

// Search `\d+[.)][\n ]`, returns next pos after marker on success
// or -1 on fail.
    int skipOrderedListMarker(const BlockState &state, int startLine) {
        char ch;
        int start = state.bMarks[startLine] + state.tShift[startLine];
        int pos = start;
        int max = state.eMarks[startLine];

        // List marker should have at least 2 chars (digit + dot)
        if (pos + 1 >= max) { return -1; }

        ch = state.src[pos++];

        if (ch < 0x30/* 0 */ || ch > 0x39/* 9 */) { return -1; }

        for (;;) {
            // EOL -> fail
            if (pos >= max) { return -1; }

            ch = state.src[pos++];

            if (ch >= 0x30/* 0 */ && ch <= 0x39/* 9 */) {

                // List marker should have no more than 9 digits
                // (prevents integer overflow in browsers)
                if (pos - start >= 10) { return -1; }

                continue;
            }

            // found valid marker
            if (ch == 0x29/* ) */ || ch == 0x2e/* . */) {
                break;
            }

            return -1;
        }


        if (pos < max) {
            ch = state.src[pos];

            if (!isSpace(ch)) {
                // " 1.test " - is not a list item
                return -1;
            }
        }
        return pos;
    }

    void markTightParagraphs(BlockState &state, int idx) {
        int level = state.level + 2;

        for (int i = idx + 2, l = state.tokens.size() - 2; i < l; i++) {
            if (state.tokens[i].level == level && state.tokens[i].type == "paragraph_open") {
                state.tokens[i + 2].hidden = true;
                state.tokens[i].hidden = true;
                i += 2;
            }
        }
    }

    bool list(BlockState &state, int startLine, int endLine, bool silent) {
        // if it's indented more than 3 spaces, it should be a code block
        if (state.sCount[startLine] - state.blkIndent >= 4) { return false; }

        // limit conditions when list can interrupt
        // a paragraph (validation mode only)
        bool isTerminatingParagraph = false;
        if (silent && state.parentType == "paragraph") {
            // Next list item should still terminate previous list item;
            //
            // This code can fail if plugins use blkIndent as well as lists,
            // but I hope the spec gets fixed long before that happens.
            //
            if (state.tShift[startLine] >= state.blkIndent) {
                isTerminatingParagraph = true;
            }
        }

        // Detect list type and position after marker
        int posAfterMarker = 0, start = 0, markerValue = 0;
        bool isOrdered = false;
        if ((posAfterMarker = skipOrderedListMarker(state, startLine)) >= 0) {
            isOrdered = true;
            start = state.bMarks[startLine] + state.tShift[startLine];
            markerValue = std::stoi(state.src.substr(start, posAfterMarker - start - 1));

            // If we're starting a new ordered list right after
            // a paragraph, it should start with 1.
            if (isTerminatingParagraph && markerValue != 1) return false;

        } else if ((posAfterMarker = skipBulletListMarker(state, startLine)) >= 0) {
            isOrdered = false;

        } else {
            return false;
        }

        // If we're starting a new unordered list right after
        // a paragraph, first line should not be empty.
        if (isTerminatingParagraph) {
            if (state.skipSpaces(posAfterMarker) >= state.eMarks[startLine]) return false;
        }

        // We should terminate list on style change. Remember first one to compare.
        char markerCharCode = state.src[posAfterMarker - 1];

        // For validation mode we can terminate immediately
        if (silent) { return true; }

        // Start list
        int listTokIdx = state.tokens.size();

        Token t1;
        if (isOrdered) {
            t1 = Token("ordered_list_open", "ol", 1);
            if (markerValue != 1) {
                t1.pushAttr(std::make_pair("start", std::to_string(markerValue)));
            }

        } else {
            t1 = Token("bullet_list_open", "ul", 1);
        }


        t1.map = std::make_pair(startLine, 0);
        t1.markup = string(1, markerCharCode);
        state.pushToken(t1);
        //
        // Iterate list items
        //

        int nextLine = startLine;
        bool prevEmptyEnd = false;

        string oldParentType = state.parentType;
        state.parentType = "list";

        char ch;
        bool tight = true;
        int pos = 0, max = 0, initial = 0, offset = 0, contentStart = 0, indentAfterMarker = 0, indent = 0;
        while (nextLine < endLine) {
            pos = posAfterMarker;
            max = state.eMarks[nextLine];

            initial = offset =
                    state.sCount[nextLine] + posAfterMarker - (state.bMarks[startLine] + state.tShift[startLine]);

            while (pos < max) {
                ch = state.src[pos];

                if (ch == 0x09) {
                    offset += 4 - (offset + state.bsCount[nextLine]) % 4;
                } else if (ch == 0x20) {
                    offset++;
                } else {
                    break;
                }

                pos++;
            }

            contentStart = pos;

            if (contentStart >= max) {
                // trimming space in "-    \n  3" case, indent is 1 here
                indentAfterMarker = 1;
            } else {
                indentAfterMarker = offset - initial;
            }

            // If we have more than 4 spaces, the indent is 1
            // (the rest is just indented code block)
            if (indentAfterMarker > 4) { indentAfterMarker = 1; }

            // "  -  test"
            //  ^^^^^ - calculating total length of this thing
            indent = initial + indentAfterMarker;

            // Run subparser & write tokens
            int itemTokIdx = state.tokens.size();      // used to update its endline of map
            Token itemTok("list_item_open", "li", 1);
            itemTok.markup = string(1, markerCharCode);
            itemTok.map = std::make_pair(startLine, 0);
            state.pushToken(itemTok);
            state.parentIndex = itemTokIdx;

            int oldIndent = state.blkIndent;
            bool oldTight = state.tight;
            int oldTShift = state.tShift[startLine];
            int oldLIndent = state.sCount[startLine];
            state.blkIndent = indent;
            state.tight = true;
            state.tShift[startLine] = contentStart - state.bMarks[startLine];
            state.sCount[startLine] = offset;

            if (contentStart >= max && state.isEmpty(startLine + 1)) {
                // workaround for this case
                // (list item is empty, list terminates before "foo"):
                // ~~~~~~~~
                //   -
                //
                //     foo
                // ~~~~~~~~
                state.curLine = std::min(state.curLine + 2, endLine);
            } else {
                state.blockParser.tokenize(state, startLine, endLine);
            }

            // If any of list item is tight, mark list as tight
            if (!state.tight || prevEmptyEnd) {
                tight = false;
            }
            // Item become loose if finish with empty line,
            // but we should filter last element, because it means list finish
            prevEmptyEnd = (state.curLine - startLine) > 1 && state.isEmpty(state.curLine - 1);

            state.blkIndent = oldIndent;
            state.tShift[startLine] = oldTShift;
            state.sCount[startLine] = oldLIndent;
            state.tight = oldTight;

            Token t2("list_item_close", "li", -1);
            t2.markup = string(1, markerCharCode);
            state.pushToken(t2);

            nextLine = startLine = state.curLine;
            state.tokens[itemTokIdx].map.second = nextLine;
            contentStart = state.bMarks[startLine];

            if (nextLine >= endLine) { break; }

            //
            // Try to check if list is terminated or continued.
            //
            if (state.sCount[nextLine] < state.blkIndent) { break; }

            // fail if terminating block found
            // Some tags can terminate paragraph without empty line.
            bool terminate = false;
            for (auto terminatorRule: terminatorsOf["list"]) {
                if ((*terminatorRule)(state, nextLine, endLine, true)) {
                    terminate = true;
                    break;
                }
            }
            if (terminate) { break; }

            // fail if list has another type
            if (isOrdered) {
                posAfterMarker = skipOrderedListMarker(state, nextLine);
                if (posAfterMarker < 0) { break; }
            } else {
                posAfterMarker = skipBulletListMarker(state, nextLine);
                if (posAfterMarker < 0) { break; }
            }

            if (markerCharCode != state.src[posAfterMarker - 1]) { break; }
        }

        // Finalize list
        Token t3;
        if (isOrdered) {
            t3 = Token("ordered_list_close", "ol", -1);
        } else {
            t3 = Token("bullet_list_close", "ul", -1);
        }
        t3.markup = string(1, markerCharCode);
        state.pushToken(t3);

        state.tokens[listTokIdx].map.second = nextLine;
        state.curLine = nextLine;

        state.parentType = oldParentType;
        state.parentIndex = -1;

        // mark paragraphs tight if needed
        if (tight) {
            markTightParagraphs(state, listTokIdx);
        }

        return true;
    }
}//namespace ccm