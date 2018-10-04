//
// Created by yong gu on 04/10/2018.
//

#include <numeric>

#include "BlockState.h"
#include "../CoreState.h"
#include "../helpers/common.h"

namespace ccm {
    BlockState::BlockState(CoreState &coreState) : coreState(coreState), curLine(0),
                                                   parentType("root") {
        bool indent_found = false;
        int start = 0, pos = 0, indent = 0, offset = 0;
        for (int len = coreState.src.size(); pos < len; pos++) {
            char ch = coreState.src[pos];

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
                    0x0A) { pos++; } //valid contents should be in [bmMarks[i], eMarks[i]), eMarks[i] may be coreState.src.len
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
        bMarks.push_back(coreState.src.size());
        eMarks.push_back(coreState.src.size());
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

//return the first nonspace in range [from, coreState.src.length())
    int BlockState::skipSpaces(int from) {
        char ch;
        for (int max = coreState.src.length(); from < max; from++) {
            ch = coreState.src[from];
            if (!isSpace(ch)) { break; }
        }
        return from;
    };

//return the last space in range (end,from]
    int BlockState::skipSpacesBack(int from, int end) {
        end = end < 0 ? 0 : end;
        if (from <= end) { return from; }
        while (from > end) {
            if (!isSpace(coreState.src[--from])) { return from + 1; }
        }
        return from;
    };

//return the first non code in range [from, coreState.src.length())
    int BlockState::skipChars(int from, char code) {
        char ch;
        for (int max = coreState.src.length(); from < max; from++) {
            ch = coreState.src[from];
            if (ch != code) { break; }
        }
        return from;
    };

//return the last non code in range (end,from]
    int BlockState::skipCharsBack(int from, int end, char code) {
        end = end < 0 ? 0 : end;
        if (from <= end) { return from; }
        while (from > end) {
            if (coreState.src[--from] != code) { return from + 1; }
        }
        return from;
    }

    std::string BlockState::getLines(int begin, int end, int indent, bool keepLastLF) {
        if (begin >= end) return "";
        std::vector<string> lines(end - begin, "");

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
                ch = coreState.src[first];

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
                lines[i] = string(lineIndent - indent + 1, ' ') + coreState.src.substr(first, last - first);
            } else {
                lines[i] = coreState.src.substr(first, last - first);
            }
        }
        return std::accumulate(lines.begin(), lines.end(), string(""));
    }

    Token &BlockState::pushToken(const Token &t) {
        Token token{t};
        token.isBlock = true;

        if (token.nesting < 0) { level--; }
        token.level = level;
        if (token.nesting > 0) { level++; }

        coreState.tokens.push_back(std::move(token));

        return coreState.tokens.back();
    }
}