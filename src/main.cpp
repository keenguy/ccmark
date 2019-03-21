//
// Created by yong gu on 14/03/2019.
//

//
// Created by yong gu on 14/09/2018.
//
#include "ccmark.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;




int main(int argc, char** argv){
    string mdText,line;
    if (argc > 1){
        ifstream mdFile(argv[1]);
        stringstream ss;
        ss << mdFile.rdbuf();
        mdText = ss.str();
    }else {
        while (getline(cin, line)) {
            mdText += line + '\n';
        }
    }
    ccm::CCMark cm;
//    cm.options.debugLevel = 1;
    cm.options.breaks = false;
    string res {cm.render(mdText)};
    
    if (argc > 2){
        ofstream htmlFile(argv[2]);
        htmlFile << res << endl;
    }else {
        cout << res << endl;
    }

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