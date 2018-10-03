//
// Created by yong gu on 01/10/2018.
//

#include "block_rules.h"
using std::string;
bool ccm::fence(BlockState &state, int startLine, int endLine, bool silent) {
    bool haveEndMarker = false;
    int pos = state.bMarks[startLine] + state.tShift[startLine];
    int max = state.eMarks[startLine];

    // if it's indented more than 3 spaces, it should be a code block
    if (state.sCount[startLine] - state.blkIndent >= 4) { return false; }

    if (pos + 3 > max) { return false; }

    char marker = state.src[pos];

    if (marker != 0x7E/* ~ */ && marker != 0x60 /* ` */) {
        return false;
    }

    // scan marker length
    int mem = pos;
    pos = state.skipChars(pos, marker);

    int len = pos - mem;

    if (len < 3) { return false; }

    string markup = state.src.substr(mem, pos - mem);
    string params = state.src.substr(pos, max - pos);

    if (params.find(marker) != string::npos) { return false; }

    // Since start is found, we can report success here in validation mode
    if (silent) { return true; }

    // search end of block
    int nextLine = startLine;

    for (;;) {
        nextLine++;
        if (nextLine >= endLine) {
            // unclosed block should be autoclosed by end of document.
            // also block seems to be autoclosed by end of parent
            break;
        }

        pos = mem = state.bMarks[nextLine] + state.tShift[nextLine];
        max = state.eMarks[nextLine];

        if (pos < max && state.sCount[nextLine] < state.blkIndent) {
            // non-empty line with negative indent should stop the list:
            // - ```
            //  test
            break;
        }

        if (state.src[pos] != marker) { continue; }

        if (state.sCount[nextLine] - state.blkIndent >= 4) {
            // closing fence should be indented less than 4 spaces
            continue;
        }

        pos = state.skipChars(pos, marker);

        // closing code fence must be at least as long as the opening one
        if (pos - mem < len) { continue; }

        // make sure tail has spaces only
        pos = state.skipSpaces(pos);

        if (pos < max) { continue; }

        haveEndMarker = true;
        // found!
        break;
    }

    // If a fence has heading spaces, they should be removed from its inner block
    len = state.sCount[startLine];

    state.curLine = nextLine + (haveEndMarker ? 1 : 0);

    Token t1("fence", "code", 0);
    t1.info = params;
    t1.content = state.getLines(startLine + 1, nextLine, len, true);
    t1.markup = markup;
    t1.map = std::make_pair(startLine, state.curLine);
    state.pushToken(t1);

    return true;
};