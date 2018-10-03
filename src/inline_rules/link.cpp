//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"

int ccm::parseLinkLabel(InlineState &state, int start, bool disableNested) {

    int max = state.posMax,
            oldPos = state.pos;

    state.pos = start + 1;
    int level = 1;
    bool found = false;
    char marker;
    int prevPos = 0;
    while (state.pos < max) {
        marker = state.src[state.pos];
        if (marker == 0x5D /* ] */) {
            level--;
            if (level == 0) {
                found = true;
                break;
            }
        }

        prevPos = state.pos;
        state.inlineParser.skipToken(state);
        if (marker == 0x5B /* [ */) {
            if (prevPos == state.pos - 1) {
// increase level if we find text `[`, which is not a part of any token
                level++;
            } else if (disableNested) {
                state.pos = oldPos;
                return -1;
            }
        }
    }
    int labelEnd = -1;
    if (found) {
        labelEnd = state.pos;
    }

// restore old state
    state.pos = oldPos;

    return labelEnd;
}

bool ccm::link(InlineState &state, bool silent) {
//        var attrs,
//                code,
//                label,
//                pos,
//                res,
//                ref,
//                title,
//                token,
//                href = '',
    int oldPos = state.pos;

    int max = state.posMax;
    int start = state.pos;
    std::string label;
    if (state.src[state.pos] != 0x5B/* [ */) { return false; }

    int labelStart = state.pos + 1;
    int labelEnd = parseLinkLabel(state, state.pos, true);

    // parser failed to find ']', so it's not a valid link
    if (labelEnd < 0) { return false; }

    int pos = labelEnd + 1;
    bool parseReference = true;

    char code;
    std::string href, title;

    if (pos < max && state.src[pos] == 0x28/* ( */) {
//
// Inline link
//

// might have found a valid shortcut link, disable reference parsing
        parseReference = false;

// [link](  <href>  "title"  )
//        ^^ skipping these spaces
        pos++;
        for (; pos < max; pos++) {
            code = state.src[pos];
            if (!isSpace(code) && code != 0x0A) { break; }
        }
        if (pos >= max) { return false; }

// [link](  <href>  "title"  )
//          ^^^^^^ parsing link destination
        start = pos;
        auto res = parseLinkDestination(state.src, pos, state.posMax);
        if (res.ok) {
            href = normalizeLink(res.str);
            if (validateLink(href)) {
                pos = res.pos;
            } else {
                href = "";
            }
        }

// [link](  <href>  "title"  )
//                ^^ skipping these spaces
        start = pos;
        for (; pos < max; pos++) {
            code = state.src[pos];
            if (!isSpace(code) && code != 0x0A) { break; }
        }

// [link](  <href>  "title"  )
//                  ^^^^^^^ parsing link title
        res = parseLinkTitle(state.src, pos, state.posMax);
        if (pos < max && start != pos && res.ok) {
            title = res.str;
            pos = res.pos;

// [link](  <href>  "title"  )
//                         ^^ skipping these spaces
            for (; pos < max; pos++) {
                code = state.src[pos];
                if (!isSpace(code) && code != 0x0A) { break; }
            }
        } else {
            title = "";
        }

        if (pos >= max || state.src[pos] != 0x29/* ) */) {
// parsing a valid shortcut link failed, fallback to reference
            parseReference = true;
        }
        pos++;
    }
    if (parseReference) {
//
// Link reference
//
//                if (typeof state.env.references == 'undefined') { return false; }

        if (pos < max && state.src[pos] == 0x5B/* [ */) {
            start = pos + 1;
            pos = parseLinkLabel(state, pos);
            if (pos >= 0) {
                label = state.src.substr(start, pos - start);
                pos++;
            } else {
                pos = labelEnd + 1;
            }
        } else {
            pos = labelEnd + 1;
        }

// covers label == '' and label == undefined
// (collapsed reference link and shortcut reference link respectively)
        if (label.empty()) { label = state.src.substr(labelStart, labelEnd - labelStart); }

        auto ref = state.linkIds.find(normalizeReference(label));
        if (!ref) {
            state.pos = oldPos;
            return false;
        }
        href = (*ref).url;
        title = (*ref).title;
    }

//
// We found the end of the link, and know for a fact it's a valid link;
// so all that's left to do is to call tokenizer.
//
    if (!silent) {
        state.pos = labelStart;
        state.posMax = labelEnd;

        Token token("link_open", "a", 1);
        token.setAttr("href", href);
        if (!title.empty()) {
            token.setAttr("title", title);
        }

        state.push(token);
        state.inlineParser.tokenize(state);

        state.push(Token("link_close", "a", -1));
    }

    state.pos = pos;
    state.posMax = max;
    return true;
}