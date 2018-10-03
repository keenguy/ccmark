//
// Created by yong gu on 02/10/2018.
//

#include "processor.h"
#include <boost/regex.hpp>
#include <algorithm>

boost::regex NEWLINES_RE("\\r(\\n|\\xc2\\x85)?|(\\xe2\\x90\\xa4|\\xe2\\x80\\xa8|\\xc2\\x85)");
boost::regex NULL_RE("\\x00");

void ccm::normalize(ccm::CoreState &state) {

    // Normalize newlines
    std::string str = regex_replace(state.src,NEWLINES_RE,"\n");

    // Replace NULL characters
    str = regex_replace(str,NULL_RE,"\xef\xbf\xbd");
    state.src = str;
}

