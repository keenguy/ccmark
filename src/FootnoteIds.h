//
// Created by yong gu on 04/10/2018.
//

#ifndef CCMARK_FOOTNOTEIDS_H
#define CCMARK_FOOTNOTEIDS_H

#include <unordered_map>
#include <string>
#include <vector>

#include "token.h"

namespace ccm {
    struct Footnote {
        Footnote() = default;
        Footnote(std::string label):label(label){}
        std::string label;
        int id = -1;
        int count = 0;
        std::vector<Token> tokens;
    };

    class FootnoteIds {
    public:
        std::unordered_map<std::string, Footnote> footnoteForLabel;
        std::vector<std::string> referedLabels;   // store labels that are referred in order of there IDs
        std::string curLabel;  //used for block parsing
    };
}

#endif //CCMARK_FOOTNOTEIDS_H
