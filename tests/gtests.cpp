//
// Created by yong gu on 14/09/2018.
//
#include "../src/ccmark.h"
#include "gtest/gtest.h"
#include <fstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <filesystem>

namespace fs = std::filesystem;

using std::string;
using std::vector;

struct Case {
    string header;
    string md;
    string html;
};

//class MDFileTest :
//        public ::testing::TestWithParam<string> {
//public:
//    ccm::CCMark cm;
//};

class CommonMarkTest :
        public ::testing::TestWithParam<Case> {
public:
    static ccm::CCMark cm;
    static void SetUpTestCase(){
        cm.options.breaks = false;
        cm.options.taskList = false;
    }
};
ccm::CCMark CommonMarkTest::cm;

//void replace_all(std::string &str) {
//    std::string::size_type pos = 0u;
//    std::string::size_type end = str.size();
//    while (pos != end) {
//        if (str[pos] == '\n' || str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\v') {
//            str.erase(pos, 1);
//            end--;
//        } else {
//            pos++;
//        }
//    }
//    boost::replace_all(str, "&lt;", "<");
//    boost::replace_all(str, "&gt;", ">");
//    boost::replace_all(str, "&amp;", "&");
//    boost::replace_all(str, "&quot;", "\"");
//
//}
//
//bool testFile(ccm::CCMark &cm, const std::string &mdFile) {
//    string htmlFile{mdFile.substr(0, mdFile.length() - 2) + "html"};
//    std::ifstream md(mdFile);
//    std::ifstream html(htmlFile);
//    
//    std::stringstream ss1, ss2;
//    ss1 << md.rdbuf();
//    ss2 << html.rdbuf();
//
//    std::string str{cm.render(ss1.str())};
//    std::string str1{str};
//    std::string str2{ss2.str()};
//    replace_all(str1);
//    replace_all(str2);
//
//    bool diff = false;
//    int len = std::min(str1.length(), str2.length());
//    int i = 0;
//    for (i = 0; i < len; i++) {
//        if (str1[i] != str2[i]) {
//            diff = true;
//            break;
//        }
//    }
//    diff |= (i != str1.length() || i != str2.length());
//    if (diff) {
//        std::cout << mdFile << std::endl;
//        std::string::size_type pos = htmlFile.rfind('/', std::string::npos);
//        std::string outFile{"../tests/tmp" + htmlFile.substr(pos)};
//        std::cout << outFile << std::endl;
//        std::cout << str1.substr(i, std::min(37, len - i)) << std::endl;
//        std::ofstream out{outFile};
//        out << str;
//        return false;
//    }
//
//    return true;
//}
//
//
//TEST_P(MDFileTest, Markdown_Test) {
//    ASSERT_TRUE(testFile(cm, GetParam()));
//}
//
//vector<string> getMdFiles() {
//    vector<string> mdFiles;
//    std::string path = "../tests/Markdown.mdtest";
//
//    for (auto &p : fs::directory_iterator(path)) {
//        std::string pathName{p.path().generic_string()};
//        std::size_t l = pathName.size();
//        if (l > 3 && pathName.compare(l - 3, 3, ".md") == 0) {
//            mdFiles.push_back(pathName);
//        }
//    }
//    return mdFiles;
//};
//
//vector<string> mdFiles;
//INSTANTIATE_TEST_CASE_P(FILES,
//                        MDFileTest,
//                        testing::ValuesIn(mdFiles));

string normalize(const string &str) {
    string newStr = boost::regex_replace(str, boost::regex("<blockquote></blockquote>"), "<blockquote>\n</blockquote>");
    return newStr;
}

std::ofstream out("../tests/CommonMark/bad.txt");

TEST_P(CommonMarkTest, Spec) {
    Case c{GetParam()};
    string html{normalize(cm.render(c.md))};
    if (html != c.html) {
        out << "~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << c.header << "\n\n.\n" << c.md << ".\n" << html << ".\n" << c.html
            << ".\n\n";
    }
    ASSERT_EQ(html, c.html) << c.header;
}

vector<Case> specs;

std::string getCaseName(testing::TestParamInfo<Case> info) {
    string name{info.param.header};
    name = name.substr(name.rfind(' ') + 1);
    name = std::to_string(info.index + 1) + "at" + name;
    return name;
}

INSTANTIATE_TEST_CASE_P(CommonMark,
                        CommonMarkTest,
                        testing::ValuesIn(specs.begin(), specs.end()), getCaseName);

void getSpecs(vector<Case> &specs) {
    std::ifstream in("../tests/CommonMark/spec.txt");

    string line;
    Case c;
    int state = 0;
    int lineNo = 0;
    int count = 0;

    string tmp;
    while (getline(in, line)) {
        lineNo ++;
        switch(state) {
            case 0:
                tmp = boost::trim_right_copy(line);
                if (line[0] == '`' && tmp.length() > 7 && line.substr(line.length() - 7) == "example") {
                    c.header = std::to_string(lineNo);
                    c.md = "";
                    c.html = "";
                    state = 1;
                }
                break;
            case 1:
                if (boost::trim_right_copy(line) == ".") {
                    state = 2;
                } else {
                    c.md += line + '\n';
                }
                break;
            case 2:
                tmp = boost::trim_right_copy(line);
                if (tmp.length() > 7 && tmp.find_first_not_of("`",0) == string::npos ){
                    state = 0;
                    count++;
                    c.header = std::to_string(count) + "at" + c.header;
                    c.md = boost::regex_replace(c.md,boost::regex("→"),"\t");
                    c.html = boost::regex_replace(c.html,boost::regex("→"),"\t");
                    specs.push_back(c);
                }else {
                    c.html += line + '\n';
                }
                break;
            default:
                break;
        }

    }


}

int main(int argc, char **argv) {
//    markdown::Document doc;
//    std::ifstream in("../tests/tmp/basic.txt");
//    std::ofstream out("../tests/tmp/basic.html");
//    doc.read(in);
//    doc.write(out);
//    std::cout<<"over"<<std::endl;
//    doc.writeTokens(std::cout);
//    mdFiles = getMdFiles();
    getSpecs(specs);
    std::ofstream goodSpecs("../tests/CommonMark/good.txt");
    for(auto const &c: specs){
        goodSpecs << "~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << c.header << "\n\n.\n" << c.md  << ".\n" << c.html
            << ".\n\n";
    }

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}