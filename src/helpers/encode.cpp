//
// Created by yong gu on 10/10/2018.
//

#include "encode.h"
#include <vector>
#include <sstream>
#include <iomanip>

namespace {
    using std::string;
    using std::vector;
    using std::stringstream;
    vector<string> cache;
    
    string percentage(unsigned int i){
        stringstream ss;
        ss << std::hex <<std::uppercase<<std::setw(2) << i;
        string tmp {ss.str()};
        return '%'+tmp.substr(tmp.length()-2);
    }

    string exclude {"-_.~!*\'();:@&=+$,/\?#"};   // [ and ] ?
    void getCache() {
        cache.reserve(128);
        for (int i = 0; i < 128; i++) {
            if (std::isalnum(i)) {
                cache.push_back(string(1,i));
            }else{
                cache.push_back(percentage(i));
            }

        }
        for (auto const &ch: exclude){
            cache[ch] = ch;
        }
    }

}

std::string encode(std::string str) {
    if (cache.empty()) {
        getCache();
    }
    unsigned int code, nextCode;
    string result;
    for (unsigned int i = 0, l = str.length(); i < l; i++) {
        code = str[i];

        if (code == 0x25 /* % */ && i + 2 < l) {
            if (std::isalnum(str[i+1]) && std::isalnum(str[i+2])) {
                result += str.substr(i, 3);
                i += 2;
                continue;
            }
        }

        if (code < 128) {
            result += cache[code];
            continue;
        }
        
        result += percentage(code);
    }
    return result;
}
