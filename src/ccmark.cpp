//
// Created by yong gu on 22/09/2018.
//

#include "ccmark.h"
#include "processor.h"

using namespace std;

namespace ccm {

    CoreState CCMark::parse(const std::string &src, optional<Options> options) const {
        Options opts = options ? *options : this->options;
        CoreState state(src, opts);

        //preprocess
        normalize(state);

        //parse blocks
        blockParser.parse(state);

#ifndef NDEBUG
        state.writeTokens(std::cout, false);
#endif
        // Parse inlines
        for (auto &tok:state.tokens) {
            if (tok.type == "inline") {
                inlineParser.parse(tok.content, tok.children, state.linkIds, state.options);
            }
        }
        return state;
    }

    std::string CCMark::render(const std::string &src, optional<Options> opts) const {
        Options options{opts ? *opts : this->options};
        CoreState state = parse(src, options);
        return renderer.render(state.tokens, opts);
    }

    CCMark::CCMark(const Options &options) : options(options), renderer(options) {

    }


    void CoreState::writeTokens(std::ostream &out, bool hidden) const {
        for (auto token: tokens) {
            if (!hidden || !token.hidden) {
                if (token.type == "inline") {
                    if (token.children.empty()) {
                        out << token.type << " : " << token.tag << token.content << std::endl;
                    } else {
                        for (auto subTok: token.children) {
                            out << subTok.type << " : " << subTok.tag << subTok.content << std::endl;
                        }
                    }
                } else {
                    out << token.type << " : " << token.tag << token.content << std::endl;
                }
            }
        }
        for (auto const &link: linkIds.mTable) {
            out << "[" << link.first << "]" << "(" << link.second.url << " \"" << link.second.title << "\")"
                << std::endl;
        }
    }
}
