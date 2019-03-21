//
// Created by yong gu on 22/09/2018.
//

#include "ccmark.h"
#include "CoreState.h"
#include "processor.h"

namespace ccm {

    CoreState CCMark::parse(const std::string &src) const {
        CoreState state(src, *this);

        //preprocess
        normalize(state);

        //parse blocks
        blockParser.parse(state);


        // Parse inlines
        for (auto &tok:state.tokens) {
            if (tok.type == "inline") {
                inlineParser.parse(tok.content, tok.children, state);
            }
        }

        //Do not forget to inline-parse block tokens in footnote references
        footnote_post(state);

        if (options.debugLevel > 0) {
            std::cout << "------- tokens begin ------" << std::endl;
            state.writeTokens(std::cout, false);
            std::cout << "------- tokens end------" << std::endl << std::endl;
        }

        return state;
    }

    std::string CCMark::render(const std::string &src) const {
        CoreState state = parse(src);
        return renderer.render(state.tokens, options);
    }
}
