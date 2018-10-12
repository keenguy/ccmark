//
// Created by yong gu on 26/09/2018.
//

#ifndef CPP_MARKDOWN_OPTIONS_H
#define CPP_MARKDOWN_OPTIONS_H

#include <string>
namespace ccm{
    struct Options{
        Options();
        int maxNesting;
        bool html;
        bool xhtmlOut;
        bool breaks;
        std::string langPrefix;

        //added
        bool taskList;
        int debugLevel;

        //not implemented
//        bool highlight = false;
//        bool linkify = false;
//        bool typographer = true;
//        std::string quotes;
    };
}
#endif //CPP_MARKDOWN_OPTIONS_H
