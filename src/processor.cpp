//
// Created by yong gu on 02/10/2018.
//

#include "processor.h"
#include <boost/regex.hpp>
#include <algorithm>
#include "InlineParser.h"

#include "CoreState.h"

boost::regex NEWLINES_RE("\\r(\\n|\\xc2\\x85)?|(\\xe2\\x90\\xa4|\\xe2\\x80\\xa8|\\xc2\\x85)");
boost::regex NULL_RE("\\x00");

void ccm::normalize(ccm::CoreState &state) {

    // Normalize newlines
    std::string str = regex_replace(state.src, NEWLINES_RE, "\n");

    // Replace NULL characters
    str = regex_replace(str, NULL_RE, "\xef\xbf\xbd");
    state.src = str;
}

void ccm::footnote_post(ccm::CoreState &state) {
//    var i, l, j, t, lastParagraph, list, token, tokens, current, currentLabel,
    int len = state.footnoteIds.referedLabels.size();
    if (len <= 0) return;
    int beginIdx = state.tokens.size();
    Token token;
    token = Token("footnote_block_open", "", 1);
    state.tokens.push_back(token);

    for (int idx = 0; idx < len; idx++) {
        auto &label = state.footnoteIds.referedLabels[idx];
        auto &fn = state.footnoteIds.footnoteForLabel[label];
        token = Token("footnote_open", "", 1);
        token.meta = {idx, -1, label};
        state.tokens.push_back(token);

//        if (!fn.tokens.empty()) {
//            token = Token("paragraph_open", "p", 1);
//            token.isBlock    = true;
//            state.tokens.push_back(token);

        for (auto &t: fn.tokens) {
            if (t.type == "inline") {
                state.inlineParser.parse(t.content, t.children, state);
            }
        }
//            state.tokens.push_back(token);

//            token          = Token("paragraph_close", "p", -1);
//            token.isBlock    = true;
//            state.tokens.push_back(token);

//        }

        Token lastToken = fn.tokens.back();
        if (lastToken.type == "paragraph_close") {
            fn.tokens.pop_back();
        }

        int num = fn.count > 0 ? fn.count : 1;
        for (int j = 0; j < num; j++) {
            Token t("footnote_anchor", "", 0);
            t.meta = {idx, j, fn.label};
            fn.tokens.push_back(t);
        }
        if (lastToken.type == "paragraph_close") {
            fn.tokens.push_back(lastToken);
        }
        state.tokens.insert(state.tokens.end(), fn.tokens.begin(), fn.tokens.end());
        token = Token("footnote_close", "", -1);
        state.tokens.push_back(token);
    }

    token = Token("footnote_block_close", "", -1);
    state.tokens.push_back(token);

}