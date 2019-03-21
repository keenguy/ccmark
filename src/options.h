//
// Created by yong gu on 26/09/2018.
//

#ifndef CPP_MARKDOWN_OPTIONS_H
#define CPP_MARKDOWN_OPTIONS_H

#include <string>
namespace ccm{
    struct Options{
        int maxNesting = 100;
        bool html = true;   // parse html code (block and inline)
        bool xhtmlOut = true;
        bool breaks = false;
        std::string langPrefix = "language-";

        //added
        bool taskList = true;
        int debugLevel = 0;

        //not implemented
//        bool highlight = false;
//        bool linkify = false;
//        bool typographer = true;
//        std::string quotes;
    };
}
#endif //CPP_MARKDOWN_OPTIONS_H
