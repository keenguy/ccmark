//
// Created by yong gu on 01/10/2018.
//
#include "block_rules.h"

namespace ccm{
    bool html_block(BlockState &state, int startLine, int endLine, bool);
}

namespace ccm {
    std::vector<HTMLPARSER> HTML_SEQUENCES;
    std::vector<string> html_blocks{
            "address",
            "article",
            "aside",
            "base",
            "basefont",
            "blockquote",
            "body",
            "caption",
            "center",
            "col",
            "colgroup",
            "dd",
            "details",
            "dialog",
            "dir",
            "div",
            "dl",
            "dt",
            "fieldset",
            "figcaption",
            "figure",
            "footer",
            "form",
            "frame",
            "frameset",
            "h1",
            "h2",
            "h3",
            "h4",
            "h5",
            "h6",
            "head",
            "header",
            "hr",
            "html",
            "iframe",
            "legend",
            "li",
            "link",
            "main",
            "menu",
            "menuitem",
            "meta",
            "nav",
            "noframes",
            "ol",
            "optgroup",
            "option",
            "p",
            "param",
            "section",
            "source",
            "summary",
            "table",
            "tbody",
            "td",
            "tfoot",
            "th",
            "thead",
            "title",
            "tr",
            "track",
            "ul"
    };

    bool html_block(BlockState &state, int startLine, int endLine, bool silent) {
        int pos = state.bMarks[startLine] + state.tShift[startLine];
        int max = state.eMarks[startLine];

        // if it's indented more than 3 spaces, it should be a code block
        if (state.sCount[startLine] - state.blkIndent >= 4) { return false; }

        if (!state.coreState.options.html) { return false; }

        if (state.coreState.src[pos] != 0x3C/* < */) { return false; }

        string lineText = state.coreState.src.substr(pos, max - pos);
        int i = 0;
        for (i = 0; i < HTML_SEQUENCES.size(); i++) {
            if (regex_search(lineText, std::get<0>(HTML_SEQUENCES[i]))) { break; }
        }

        if (i == HTML_SEQUENCES.size()) { return false; }

        if (silent) {
            // true if this sequence can be a terminator, false otherwise
            return std::get<2>(HTML_SEQUENCES[i]);
        }

        int nextLine = startLine + 1;

        // If we are here - we detected HTML block.
        // Let's roll down till block end.
        if (!regex_search(lineText, std::get<1>(HTML_SEQUENCES[i]))) {
            for (; nextLine < endLine; nextLine++) {
                if (state.sCount[nextLine] < state.blkIndent) { break; }

                pos = state.bMarks[nextLine] + state.tShift[nextLine];
                max = state.eMarks[nextLine];
                lineText = state.coreState.src.substr(pos, max - pos);

                if (regex_search(lineText, std::get<1>(HTML_SEQUENCES[i]))) {
                    if (lineText.length() != 0) { nextLine++; }
                    break;
                }
            }
        }

        state.curLine = nextLine;

        Token token("html_block", "", 0);
        token.map = std::make_pair(startLine, nextLine);
        token.content = state.getLines(startLine, nextLine, state.blkIndent, true);
        state.pushToken(token);

        return true;
    };
}