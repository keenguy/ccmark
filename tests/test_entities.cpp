//
// Created by yong gu on 24/09/2018.
//

#include <codecvt>
#include <iostream>
#include <locale>



// Convert UTF-8 byte string to wstring
std::wstring to_wstring(std::string const& s) {
    std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
    return conv.from_bytes(s);
}

// Convert wstring to UTF-8 byte string
std::string to_string(std::wstring const& s) {
    std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
    return conv.to_bytes(s);
}

// Converts a UTF-8 encoded string to upper case
std::string tou(std::string const& s) {
    auto ss = to_wstring(s);
    std::locale utf8("en_US.UTF-8");
    for (auto& c : ss) {
        c = std::toupper(c, utf8);
    }
    return to_string(ss);
}

void test_utf8(std::ostream& os) {
    os << tou("foo" ) << std::endl;
    //os << tou("#foo") << std::endl;
    //os << tou("ßfoo") << std::endl;
    //os << tou("Éfoo") << std::endl;
}

int main(int argc, char** argv) {
    std::locale utf8("en_US.UTF-8");
    test_utf8(std::cout);
    return 0;
}