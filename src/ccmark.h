//
// Created by yong gu on 22/09/2018.
//

#ifndef CPP_MARKDOWN_CCMARK_H
#define CPP_MARKDOWN_CCMARK_H

#include <string>
#include <iostream>
#include <list>
#include <unordered_map>

#include "options.h"
#include "token.h"
#include "LinkIds.h"
#include "BlockParser.h"
#include "Renderer.h"
#include "InlineParser.h"

namespace ccm {

    // Forward references.

    class CoreState{
    public:
        CoreState(std::string src, Options& options):src(std::move(src)),options(options){}
        std::string src;
        std::vector<Token> tokens;
        LinkIds linkIds;
        Options options;

        void writeTokens(std::ostream& out, bool hidden = true) const;
    };
    class CCMark {
    public:
        CCMark() = default;
        explicit CCMark(const Options& options);

        BlockParser blockParser;
        InlineParser inlineParser;
        Renderer renderer;

        // parse src to tokens
        CoreState parse(const std::string& src, optional<Options> options = {}) const;

        // call parse to get tokens, then use renderer.render to render
        std::string render(const std::string& src, optional<Options> options ={}) const;


//        void normalize();

        //global options, which can be override by file-specific options in CoreState and renderer-specific options
        Options options;

    };
} //end namespace ccm

#endif //CPP_MARKDOWN_CCMARK_H
