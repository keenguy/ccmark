//
// Created by yong gu on 25/09/2018.
//

#include "LinkIds.h"
#include <algorithm>

namespace ccm {
    std::optional<LinkIds::Target> LinkIds::find(const std::string &id) const {
        Table::const_iterator i = mTable.find(_scrubKey(id));
        if (i != mTable.end()) return i->second;
        else return {};
    }

    void LinkIds::add(const std::string &id, const std::string &url, const
    std::string &title) {
        mTable.insert(std::make_pair(_scrubKey(id), Target(url, title)));
    }

    std::string LinkIds::_scrubKey(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
}