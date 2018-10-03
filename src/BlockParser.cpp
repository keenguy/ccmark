//
// Created by yong gu on 22/09/2018.
//

#include "BlockParser.h"
#include "helpers/common.h"
#include "block_rules/block_rules.h"
#include "ccmark.h"
#include <numeric>

using namespace std;

namespace ccm {
    BlockState::BlockState(std::string &src, Options &options, std::vector<Token> &tokens, LinkIds &linkIds,
                           const BlockParser &blockParser) : blockParser(blockParser),
                                                             src(src),
                                                             options(options),
                                                             tokens(tokens),
                                                             linkIds(linkIds), curLine(0),
                                                             parentType("root") {
        bool indent_found = false;
        int start = 0, pos = 0, indent = 0, offset = 0;
        for (int len = src.size(); pos < len; pos++) {
            char ch = src[pos];

            if (!indent_found) {
                if (ch == 0x09 || ch == 0x20) {      // 0x09(tab) or 0x20(space)
                    indent++;

                    if (ch == 0x09) {
                        offset += 4 - offset % 4;
                    } else {
                        offset++;
                    }
                    continue;
                } else {
                    indent_found = true;
                }
            }

            if (ch == 0x0A || pos == len - 1) {    // newline or end of file
                if (ch !=
                    0x0A) { pos++; } //valid contents should be in [bmMarks[i], eMarks[i]), eMarks[i] may be src.len
                bMarks.push_back(start);
                eMarks.push_back(pos);
                tShift.push_back(indent);  //not expanded
                sCount.push_back(offset);  //expanded
                bsCount.push_back(0);

                indent_found = false;
                indent = 0;
                offset = 0;
                start = pos + 1;
            }
        }//end for

        // push_back fake entry to simplify cache bounds checks
        bMarks.push_back(src.size());
        eMarks.push_back(src.size());
        tShift.push_back(0);
        sCount.push_back(0);
        bsCount.push_back(0);

        maxLine = bMarks.size() - 1; // don't count last fake line

    }

    bool BlockState::isEmpty(int line) {
//        assert(line < maxLine);
        return bMarks[line] + tShift[line] >= eMarks[line];
    }

    int BlockState::skipEmptyLines(int from) {
        for (int max = maxLine; from < max; from++) {
            if (bMarks[from] + tShift[from] < eMarks[from]) {
                break;
            }
        }
        return from;
    };

    //return the first nonspace in range [from, src.length())
    int BlockState::skipSpaces(int from) {
        char ch;
        for (int max = src.length(); from < max; from++) {
            ch = src[from];
            if (!isSpace(ch)) { break; }
        }
        return from;
    };

    //return the last space in range (end,from]
    int BlockState::skipSpacesBack(int from, int end) {
        end = end < 0 ? 0 : end;
        if (from <= end) { return from; }
        while (from > end) {
            if (!isSpace(src[--from])) { return from + 1; }
        }
        return from;
    };

    //return the first non code in range [from, src.length())
    int BlockState::skipChars(int from, char code) {
        char ch;
        for (int max = src.length(); from < max; from++) {
            ch = src[from];
            if (ch != code) { break; }
        }
        return from;
    };

    //return the last non code in range (end,from]
    int BlockState::skipCharsBack(int from, int end, char code) {
        end = end < 0 ? 0 : end;
        if (from <= end) { return from; }
        while (from > end) {
            if (src[--from] != code) { return from + 1; }
        }
        return from;
    }

    std::string BlockState::getLines(int begin, int end, int indent, bool keepLastLF) {
        if (begin >= end) return "";
        vector<string> lines(end - begin, "");

        int line = begin;
        int lineIndent, lineStart, first, last; //used for each line
        char ch;
        for (int i = 0; line < end; line++, i++) {
            lineIndent = 0;
            lineStart = first = bMarks[line];

            if (line + 1 < end || keepLastLF) {
                // No need for bounds check because we have fake entry on tail.
                last = eMarks[line] + 1;
            } else {
                last = eMarks[line];
            }

            while (first < last && lineIndent < indent) {
                ch = src[first];

                if (isSpace(ch)) {
                    if (ch == 0x09) {  //tab
                        lineIndent += 4 - (lineIndent + bsCount[line]) % 4;
                    } else {
                        lineIndent++;
                    }
                } else if (first - lineStart < tShift[line]) {
                    // patched tShift masked characters to look like spaces (blockquotes, list markers)
                    lineIndent++;
                } else {
                    break;
                }

                first++;
            }
            if (lineIndent > indent) {
                // partially expanding tabs in code blocks, e.g '\t\tfoobar'
                // with indent=2 becomes '  \tfoobar'
                lines[i] = string(lineIndent - indent + 1, ' ') + src.substr(first, last - first);
            } else {
                lines[i] = src.substr(first, last - first);
            }
        }
        return accumulate(lines.begin(), lines.end(), string(""));
    }

    Token &BlockState::pushToken(const Token &t) {
        Token token{t};
        token.isBlock = true;

        if (token.nesting < 0) { level--; }
        token.level = level;
        if (token.nesting > 0) { level++; }
        tokens.push_back(std::move(token));
        return tokens.back();
    }


    void BlockParser::parse(CoreState &coreState) const {
        BlockState blockState(coreState.src, coreState.options, coreState.tokens, coreState.linkIds, *this);
        tokenize(blockState, blockState.curLine, blockState.maxLine);
    }

    void BlockParser::tokenize(BlockState &state, int startLine, int endLine) const {
        bool ok;
        int i;

        int len = rules.size();
        int line = startLine;
        bool hasEmptyLines = false;
        int maxNesting = state.options.maxNesting;

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
            // - update `state.tokens`
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

    BlockParser::BlockParser() {
        rules = getBlockRules();
    }
}
