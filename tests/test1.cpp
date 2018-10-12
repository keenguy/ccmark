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

string html_tmpl {R"(<!DOCTYPE html>
<html lang=en>
<head>
    <meta charset="utf-8">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/KaTeX/0.5.1/katex.min.css">
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/github-markdown-css/2.2.1/github-markdown.css"/>
    <link rel="stylesheet" href="custom.css" />
    <script type="text/javascript" async
        src="https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.5/MathJax.js?config=TeX-MML-AM_CHTML">
    </script>
	<style>
	#input {
		display: block;
		font-size: 12pt;
		padding-top: 0.5em;
		padding-left: 0.1em;
		font-family: monospace;
	}
	#button {
		margin-top: 1em;
		width: 120px;
		height: 50px;
		font-size: 14pt;
	}
	#output {
		margin: 20px;
	}
	</style>
</head>
<body>
<article class="markdown-body container-fluid">
{{body}}
</article>
</body>
</html>)"};


int main(int argc, char** argv){
    ifstream mdFile("../tests/tmp/basic.txt");
    ofstream htmlFile("../tests/tmp/basic.html");
    stringstream ss;
    ss << mdFile.rdbuf();
    string src {ss.str()};



    ccm::CCMark cm;
    cm.options.debugLevel = 1;
    string res {cm.render(src)};
    cout<<res<<endl;

    string html = html_tmpl.replace(html_tmpl.find("{{body}}"),8, res);
    htmlFile << html;

//    state.writeTokens(std::cout);

    return 0;
}