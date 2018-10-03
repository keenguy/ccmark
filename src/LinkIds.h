//
// Created by yong gu on 25/09/2018.
//

#ifndef CPP_MARKDOWN_LINKIDS_H
#define CPP_MARKDOWN_LINKIDS_H

#include <string>
#include <unordered_map>

namespace ccm {
    class LinkIds {
    public:
        struct Target {
            std::string url;
            std::string title;

            Target(std::string url_, std::string title_) :
                    url(std::move(url_)), title(std::move(title_)) {}
        };

        std::optional<Target> find(const std::string &id) const;

        void add(const std::string &id, const std::string &url, const
        std::string &title);

    private:
        typedef std::unordered_map<std::string, Target> Table;

        static std::string _scrubKey(std::string str);
    public:
        Table mTable;
    };
}

#endif //CPP_MARKDOWN_LINKIDS_H
