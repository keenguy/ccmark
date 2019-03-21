//
// Created by yong gu on 04/10/2018.
//

#include "CoreState.h"
#include "ccmark.h"

void ccm::CoreState::writeTokens(std::ostream &out, bool hidden) const {
    out<<tokens.size()<<" tokens"<<std::endl;
    for (auto token: tokens) {
        if (!hidden || !token.hidden) {
            if (token.type == "inline") {
                if (token.children.empty()) {
                    out << token.type << " : " << token.tag << token.content << std::endl;
                } else {
                    for (auto subTok: token.children) {
                        out << subTok.type << " : " << subTok.tag << subTok.content <<subTok.meta.label<< std::endl;
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
    for (auto const &fn: fnIds.footnoteForLabel){
        out<<fn.first<<" -> "<<fn.second.count<<std::endl;
    }
}

ccm::CoreState::CoreState(std::string src, const ccm::CCMark &ccMark): src(std::move(src)), blockParser(ccMark.blockParser),
inlineParser(ccMark.inlineParser),options(ccMark.options){}
