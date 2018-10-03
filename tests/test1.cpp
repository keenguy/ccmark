//
// Created by yong gu on 22/09/2018.
//

#include "../src/ccmark.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <boost/regex.hpp>

using namespace std;

namespace fs = std::filesystem;

//bool testFiles(const std::string& mdFile, const std::string& htmlFile){
//    const std::string root = "../tests/basic/";
//    std::ifstream md(root + mdFile);
//    std::ifstream html(root+htmlFile);
//
//    cm.readFile(md);
//
//    std::cout<<cm.src<<std::endl;
//    return true;
//}




int main(int argc, char** argv){
    ifstream mdFile("../tests/tmp/basic.txt");
    stringstream ss;
    ss << mdFile.rdbuf();
    string src {ss.str()};


    ccm::CCMark cm;
    cout<<cm.render(src)<<endl;

//    state.writeTokens(std::cout);

    return 0;
}