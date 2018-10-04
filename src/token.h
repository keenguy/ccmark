//
// Created by yong gu on 22/09/2018.
// complex tokens are divided into several subtokens. e.g. 'list' consists of 'list_open', 'list_item', 'list_close'
//

#ifndef CPP_MARKDOWN_TOKEN_H
#define CPP_MARKDOWN_TOKEN_H


#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <optional>


namespace ccm {
    using std::optional;

    struct Meta{
        int id = 0;
        int subId = 0;
        std::string label;
    };
class Token{

    typedef std::pair<std::string,std::string> Attr;
public:
    Token() = default;
    Token(std::string type, std::string tag, int nesting):type(std::move(type)),tag(std::move(tag)),nesting(nesting){};
    std::string type;
    std::string tag;
    int nesting = 0;

    int level = 0;
    bool isBlock = false;
    bool hidden = false;
    std::string content;
    std::string markup;
    std::string info;
    Meta meta;

    std::pair<int,int> map;
    std::vector<Token> children;


    int indexOfAttr(const std::string& name) const{
        for(int i = 0; i < attrs.size(); i++){
            if (attrs[i].first == name) return i;
        }
        return -1;
    }

    void pushAttr(const Attr& attr){
        attrs.push_back(attr);
    }

    void setAttr(const std::string& name, const std::string& value){
        int idx = indexOfAttr(name);
        if (idx >= 0){
            attrs[idx].second = value;
        }else{
            pushAttr(std::make_pair(name,value));
        }
    }

    optional<std::string> getAttr(const std::string& name){
        int idx = indexOfAttr(name);
        if (idx >= 0){
            return attrs[idx].second;
        }else{
            return {};
        }
    }

    void joinAttr(const std::string& name, const std::string& value){
        int idx = indexOfAttr(name);
        if (idx >= 0){
            attrs[idx].second += ' ' + value;
        }else{
            pushAttr(std::make_pair(name,value));
        }
    }

    std::vector<Attr> attrs;

};


} // namespace ccm



#endif //CPP_MARKDOWN_TOKEN_H
