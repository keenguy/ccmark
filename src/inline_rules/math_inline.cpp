//
// Created by yong gu on 03/10/2018.
//

#include "inline_rules.h"

namespace ccm {
    bool math_inline(InlineState &state, bool silent);
}

namespace{
    std::pair<bool,bool> isValidDelim(ccm::InlineState &state, int pos) {
        char prevChar, nextChar;
        int max = state.posMax;
        bool can_open = true;
        bool can_close = true;

        prevChar = pos > 0 ? state.src[pos - 1] : -1;
        nextChar = pos + 1 <= max ? state.src[pos + 1] : -1;

        // Check non-whitespace conditions for opening and closing, and
        // check that closing delimeter isn't followed by a number
        if (prevChar == 0x20/* " " */ || prevChar == 0x09/* \t */ ||
                                                       (nextChar >= 0x30/* "0" */ && nextChar <= 0x39/* "9" */)) {
            can_close = false;
        }
        if (nextChar == 0x20/* " " */ || nextChar == 0x09/* \t */) {
            can_open = false;
        }

        return {can_open, can_close};
    }
}
bool ccm::math_inline(InlineState &state, bool silent){

    if (state.src[state.pos] != '$') { return false; }

    auto res = isValidDelim(state, state.pos);
    if (!res.first) {
        if (!silent) { state.pending += "$"; }
        state.pos += 1;
        return true;
    }

    // First check for and bypass all properly escaped delimieters
    // This loop will assume that the first leading backtick can not
    // be the first character in state.src, which is known since
    // we have found an opening delimieter already.
    int start = state.pos + 1;
    int match = start;
    int pos = 0;
    while ( (match = state.src.find('$', match)) != string::npos) {
        // Found potential $, look for escapes, pos will point to
        // first non escape when complete
        pos = match - 1;
        while (state.src[pos] == '\\') { pos -= 1; }

        // Even number of escapes, potential closing delimiter found
        if ( ((match - pos) % 2) == 1 ) { break; }
        match += 1;
    }

    // No closing delimter found.  Consume $ and continue.
    if (match == -1) {
        if (!silent) { state.pending += "$"; }
        state.pos = start;
        return true;
    }

    // Check if we have empty content, ie: $$.  Do not parse.
    if (match - start == 0) {
        if (!silent) { state.pending += "$$"; }
        state.pos = start + 1;
        return true;
    }

    // Check for valid closing delimiter
    res = isValidDelim(state, match);
    if (!res.second) {
        if (!silent) { state.pending += "$"; }
        state.pos = start;
        return true;
    }

    if (!silent) {
        Token token("math_inline", "math", 0);
        token.markup  = "$";
        token.content = state.src.substr(start, match-start);
        state.push(token);
    }

    state.pos = match + 1;
    return true;
}