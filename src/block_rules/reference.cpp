//
// Created by yong gu on 01/10/2018.
//

#include "block_rules.h"

namespace ccm{
    bool reference(BlockState &state, int startLine, int endLine, bool);
}

bool ccm::reference(BlockState &state, int startLine, int _endLine, bool silent) {
    int pos = state.bMarks[startLine] + state.tShift[startLine];
    int max = state.eMarks[startLine];
    int nextLine = startLine + 1;

    // if it's indented more than 3 spaces, it should be a code block
    if (state.sCount[startLine] - state.blkIndent >= 4) { return false; }

    if (state.src[pos] != 0x5B/* [ */) { return false; }

    // Simple check to quickly interrupt scan on [link](url) at the start of line.
    // Can be useful on practice: https://github.com/markdown-it/markdown-it/issues/54
    while (++pos < max) {
        if (state.src[pos] == 0x5D /* ] */ &&
            state.src[pos - 1] != 0x5C/* \ */) {
            if (pos + 1 == max) { return false; }
            if (state.src[pos + 1] != 0x3A/* : */) { return false; }
            break;
        }
    }

    int endLine = state.maxLine;

    // jump line-by-line until empty one or EOF

    string oldParentType = state.parentType;
    state.parentType = "reference";

    bool terminate;
    for (; nextLine < endLine && !state.isEmpty(nextLine); nextLine++) {
        // this would be a code block normally, but after paragraph
        // it's considered a lazy continuation regardless of what's there
        if (state.sCount[nextLine] - state.blkIndent > 3) { continue; }

        // quirk for blockquotes, this line should already be checked by that rule
        if (state.sCount[nextLine] < 0) { continue; }

        // Some tags can terminate paragraph without empty line.
        terminate = false;
        for (auto terminatorRule: terminatorsOf["reference"]) {
            if ((*terminatorRule)(state, nextLine, endLine, true)) {
                terminate = true;
                break;
            }
        }
        if (terminate) { break; }
    }

    string str = boost::trim_copy(state.getLines(startLine, nextLine, state.blkIndent, false));
    max = str.length();

    char ch;
    int lines = 0, labelEnd = 0;
    for (pos = 1; pos < max; pos++) {
        ch = str[pos];
        if (ch == 0x5B /* [ */) {
            return false;
        } else if (ch == 0x5D /* ] */) {
            labelEnd = pos;
            break;
        } else if (ch == 0x0A /* \n */) {
            lines++;
        } else if (ch == 0x5C /* \ */) {
            pos++;
            if (pos < max && str[pos] == 0x0A) {
                lines++;
            }
        }
    }

    if (labelEnd < 0 || str[labelEnd + 1] != 0x3A/* : */) { return false; }

    // [label]:   destination   "title"
    //         ^^^ skip optional whitespace here
    for (pos = labelEnd + 2; pos < max; pos++) {
        ch = str[pos];
        if (ch == 0x0A) {
            lines++;
        } else if (isSpace(ch)) {
            /*eslint no-empty:0*/
        } else {
            break;
        }
    }

    // [label]:   destination   "title"
    //            ^^^^^^^^^^^ parse this
    LinkRefRes res;
    res = parseLinkDestination(str, pos, max);
    if (!res.ok) { return false; }

    string href = normalizeLink(res.str);
    if (!validateLink(href)) { return false; }

    pos = res.pos;
    lines += res.lines;

    // save cursor state, we could require to rollback later
    int destEndPos = pos;
    int destEndLineNo = lines;

    // [label]:   destination   "title"
    //                       ^^^ skipping those spaces
    int start = pos;
    for (; pos < max; pos++) {
        ch = str[pos];
        if (ch == 0x0A) {
            lines++;
        } else if (isSpace(ch)) {
            /*eslint no-empty:0*/
        } else {
            break;
        }
    }

    // [label]:   destination   "title"
    //                          ^^^^^^^ parse this
    res = parseLinkTitle(str, pos, max);
    string title;
    if (pos < max && start != pos && res.ok) {
        title = res.str;
        pos = res.pos;
        lines += res.lines;
    } else {
        title = "";
        pos = destEndPos;
        lines = destEndLineNo;
    }

    // skip trailing spaces until the rest of the line
    while (pos < max) {
        ch = str[pos];
        if (!isSpace(ch)) { break; }
        pos++;
    }

    if (pos < max && str[pos] != 0x0A) {
        if (!title.empty()) {
            // garbage at the end of the line after title,
            // but it could still be a valid reference if we roll back
            title = "";
            pos = destEndPos;
            lines = destEndLineNo;
            while (pos < max) {
                ch = str[pos];
                if (!isSpace(ch)) { break; }
                pos++;
            }
        }
    }

    if (pos < max && str[pos] != 0x0A) {
        // garbage at the end of the line
        return false;
    }

    string label = normalizeReference(str.substr(1, labelEnd - 1));
    if (label.empty()) {
        // CommonMark 0.20 disallows empty labels
        return false;
    }

    // Reference can not terminate anything. This check is for safety only.
    /*istanbul ignore if*/
    if (silent) { return true; }


    state.linkIds.add(label, href, title);


    state.parentType = oldParentType;

    state.curLine = startLine + lines + 1;
    return true;
};