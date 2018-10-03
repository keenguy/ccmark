//
// Created by yong gu on 14/09/2018.
//
#include "../src/markdown.h"
#include "gtest/gtest.h"
#include <fstream>
#include <filesystem>
#include <boost/regex.hpp>

namespace fs = std::filesystem;

bool testFiles(const std::string& mdFile, const std::string& htmlFile){
    const std::string root = "../tests/basic/";
    std::ifstream md(root + mdFile);
    std::ifstream html(root+htmlFile);
    markdown::Document doc;
    doc.read(md);

    std::stringstream ss1,ss2;

    std::ofstream out("../tests/tmp/" + htmlFile);
    doc.write(out);
    doc.write(ss1);
    ss2<<html.rdbuf();
    return ss1.str() == ss2.str();
}




int main(int argc, char** argv){
    markdown::Document doc;
    std::ifstream in("../tests/tmp/basic.txt");
    std::ofstream out("../tests/tmp/basic.html");
    doc.read(in);
    doc.write(std::cout);
//    std::cout<<"over"<<std::endl;
    doc.writeTokens(std::cout);

//    boost::regex cExpression("(?:<((/?)([a-zA-Z0-9]+)(?:( +[a-zA-Z0-9]+?(?: ?= ?(?<quote>\"|').*?\\k<quote>)?)*? */? *))>)" // potential HTML tag or auto-link
//                             );
//    boost::smatch m;
//    std::string src("<some  stuff>");

//    bool matched = boost::regex_search(src.cbegin(),src.cend(),m,cExpression);
//    if(matched){
//        std::cout<<m[0].str()<<std::endl;
//    }else{
//        std::cout<<"not matched !"<<std::endl;
//    }

//    std::cout<<testFiles("anglel-links-and-img.txt","anglel-links-and-img.html");
    return 0;
}