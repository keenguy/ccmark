//
// Created by yong gu on 28/09/2018.
//

#include "inline_rules.h"


namespace ccm {
        using namespace std;

        //global post rules
        void link_pairs(InlineState &state) {
            auto &delimiters = state.delimiters;
            auto max = state.delimiters.size();

            int i, j;
            for (i = 0; i < max; i++) {
                auto &lastDelim = delimiters[i];

                if (!lastDelim.close) { continue; }

                j = i - lastDelim.jump - 1;

                while (j >= 0) {
                    auto &currDelim = delimiters[j];

                    if (currDelim.open &&
                        currDelim.marker == lastDelim.marker &&
                        currDelim.end < 0 &&
                        currDelim.level == lastDelim.level) {

                        // typeofs are for backward compatibility with plugins
                        bool odd_match = (currDelim.close || lastDelim.open) &&
                                         currDelim.length != -1 &&
                                         lastDelim.length != -1 &&
                                         ((currDelim.length + lastDelim.length) % 3 == 0);

                        if (!odd_match) {
                            lastDelim.jump = i - j;
                            lastDelim.open = false;
                            currDelim.end = i;
                            currDelim.jump = 0;
                            break;
                        }
                    }

                    j -= currDelim.jump + 1;
                }
            }
        }

        void text_collapse(InlineState &state) {
            int curr, last;
            int level = 0;
            auto &tokens = state.tokens;
            int max = state.tokens.size();

            for (curr = last = 0; curr < max; curr++) {
                // re-calculate levels
                level += tokens[curr].nesting;
                tokens[curr].level = level;

                if (tokens[curr].type == "text" &&
                    curr + 1 < max &&
                    tokens[curr + 1].type == "text") {

                    // collapse two adjacent text nodes
                    tokens[curr + 1].content = tokens[curr].content + tokens[curr + 1].content;
                } else {
                    if (curr != last) { tokens[last] = tokens[curr]; }

                    last++;
                }
            }

            if (curr != last) {
                tokens.resize(last);
            }
        };


        bool compiled = false;

        void compile() {
            for (auto ch:ESCAPE_CHARS) {
                ESCAPED[ch] = 1;
            }
            compiled = true;
        }



    std::vector<InlineRule> getInlineRules() {
        if (!compiled) { compile(); }
        return {text, newline, escape, backtick, strikethrough, emphasis, link, image, autolink, html_inline, entity};
    }

    std::vector<InlinePostRule> getInlinePostRules() {
        return {link_pairs, strikethrough_post, emphasis_post, text_collapse};
    }
}