//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"

namespace ccm{
    bool image(InlineState &state, bool silent);
}

bool ccm::image(InlineState &state, bool silent) {

    int oldPos = state.pos;
    int max = state.posMax;

    if (state.src[state.pos] != 0x21/* ! */) { return false; }
    if (state.src[state.pos + 1] != 0x5B/* [ */) { return false; }

    int labelStart = state.pos + 2;
    int labelEnd = parseLinkLabel(state, state.pos + 1, false);

    // parser failed to find ']', so it's not a valid link
    if (labelEnd < 0) { return false; }

    int pos = labelEnd + 1;
    char code;
    int start = 0;
    std::string href, title, label;
    if (pos < max && state.src[pos] == 0x28/* ( */) {
        //
        // Inline link
        //

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
            state.pos = oldPos;
            return false;
        }
        pos++;
    } else {
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
        std::string content = state.src.substr(labelStart, labelEnd - labelStart);

        std::vector<Token> tokens;
        state.inlineParser.parse(content, tokens, state.linkIds, state.options);

        Token token("image", "img", 0);
        token.setAttr("src", href);
        token.setAttr("alt", "");
        token.children = tokens;
        token.content = content;


        if (!title.empty()) {
            token.setAttr("title", title);
        }
        state.push(token);
    }

    state.pos = pos;
    state.posMax = max;
    return true;
}