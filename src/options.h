//
// Created by yong gu on 26/09/2018.
//

#ifndef CPP_MARKDOWN_OPTIONS_H
#define CPP_MARKDOWN_OPTIONS_H

#include <string>
namespace ccm{
    struct Options{
        int maxNesting = 100;
        bool html = true;
        bool xhtmlOut = true;
        bool breaks = false;
        std::string langPrefix {"language-"};

        //not implemented
//        bool highlight = false;
//        bool linkify = false;
//        bool typographer = true;
//        std::string quotes;
    };
}
#endif //CPP_MARKDOWN_OPTIONS_H
