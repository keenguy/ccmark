//
// Created by yong gu on 14/09/2018.
//
#include "../src/ccmark.h"
#include "gtest/gtest.h"
#include <fstream>
//#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <filesystem>
namespace fs = std::filesystem;

void replace_all(std::string& str){
    std::string::size_type pos = 0u;
    std::string::size_type end = str.size();
    while(pos != end) {
        if (str[pos] == '\n' || str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\v') {
            str.erase(pos,1);
            end--;
        }else {
            pos++;
        }
    }
    boost::replace_all(str,"&lt;","<");
    boost::replace_all(str,"&gt;",">");
    boost::replace_all(str,"&amp;","&");
    boost::replace_all(str,"&quot;","\"");

}

bool testFiles(const std::string& mdFile, const std::string& htmlFile){
    std::ifstream md(mdFile);
    std::ifstream html(htmlFile);

    ccm::CCMark cm;

    std::stringstream ss1,ss2;
    ss1<<md.rdbuf();
    ss2<<html.rdbuf();

    std::string str {cm.render(ss1.str())};
    std::string str1{str};
    std::string str2 {ss2.str()};
    replace_all(str1);
    replace_all(str2);

    bool diff = false;
    int len = std::min(str1.length(),str2.length());
    int i = 0;
    for (i = 0; i < len; i++){
        if(str1[i] != str2[i]){
            diff = true;
            break;
        }
    }
    diff |= (i != str1.length() || i != str2.length());
    if (diff){
        std::cout<<mdFile<<std::endl;
        std::string::size_type pos = htmlFile.rfind('/',std::string::npos);
        std::string outFile {"../tests/tmp" + htmlFile.substr(pos)};
        std::cout<<outFile<<std::endl;
        std::cout<<str1.substr(i,std::min(37,len-i))<<std::endl;
        std::ofstream out {outFile};
        out<<str;
        return false;
    }

    return true;
}


TEST(Markdown_Test, all){
    std::string path = "../tests/Markdown.mdtest";
    for (auto & p : fs::directory_iterator(path)){
        std::string pathName {p.path().generic_string()};
        std::size_t l = pathName.size();
        if (l > 3 && pathName.compare(l-3,3,".md") == 0){
            ASSERT_TRUE(testFiles(pathName,pathName.substr(0,l-2) + "html"));
        }
    }
}
int main(int argc, char** argv){
//    markdown::Document doc;
//    std::ifstream in("../tests/tmp/basic.txt");
//    std::ofstream out("../tests/tmp/basic.html");
//    doc.read(in);
//    doc.write(out);
//    std::cout<<"over"<<std::endl;
//    doc.writeTokens(std::cout);
    ::testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}