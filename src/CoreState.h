//
// Created by yong gu on 04/10/2018.
//

#ifndef CCMARK_CORESTATE_H
#define CCMARK_CORESTATE_H

#include <string>
#include <vector>
#include "options.h"
#include "token.h"
#include "LinkIds.h"
#include "FootnoteIds.h"

namespace ccm {
    class CCMark;
    class BlockParser;
    class InlineParser;
    class CoreState {
    public:
        CoreState(std::string src, const CCMark &ccMark);

        //for shortcut
        const BlockParser& blockParser;
        const InlineParser& inlineParser;
        const Options &options;

        std::string src;
        std::vector <Token> tokens;
        LinkIds linkIds;    // block links
        FootnoteIds fnIds;  // footnotes

        void writeTokens(std::ostream &out, bool hidden = true) const;
    };
}


#endif //CCMARK_CORESTATE_H
