//
// Created by yong gu on 23/09/2018.
//

#include <tuple>
#include "block_rules.h"
#include "../BlockParser.h"
#include "../helpers/html_re.h"

namespace ccm {
    std::unordered_map<std::string, std::vector<BlockRule>> terminatorsOf;

    namespace {   //file static
        bool compiled = false;

        // set terminator rules
        void compile() {
            std::string tmp = joinStrVec(html_blocks, "|");
            HTML_SEQUENCES = {
                    {boost::regex("^<(script|pre|style)(?=(\\s|>|$))", boost::regex_constants::icase),  boost::regex(
                            "<\\/(script|pre|style)>", boost::regex_constants::icase), true},
                    {boost::regex("^<!--"),                                                             boost::regex(
                            "-->"),                                                    true},
                    {boost::regex("^<\\?"),                                                             boost::regex(
                            "\\?>"),                                                   true},
                    {boost::regex("^<![A-Z]"),                                                          boost::regex(
                            ">"),                                                      true},
                    {boost::regex("^<!\\[CDATA\\["),                                                    boost::regex(
                            "\\]\\]>"),                                                true},
                    {boost::regex("^</?(" + tmp + ")(?=(\\s|/?>|$))", boost::regex_constants::icase), boost::regex(
                            "^$"),                                                     true},
                    {boost::regex(HTML_OPEN_CLOSE_TAG + "\\s*$"),                                     boost::regex(
                            "^$"),                                                     false}
            };
            terminatorsOf["paragraph"] = {fence, blockquote, hr, list, heading,
                                          html_block}; //vector<BlockRule>(1,heading);
            terminatorsOf["reference"] = {fence, blockquote, hr, list, heading, html_block};
            terminatorsOf["blockquote"] = {fence, blockquote, hr, list, heading, html_block};
            terminatorsOf["list"] = {fence, blockquote, hr};
            compiled = true;
        }
    }

    std::vector<BlockRule> getBlockRules() {
        if (!compiled) {
            compile();
        }
        std::vector<BlockRule> rules {code,fence,blockquote,hr,list,reference,heading,lheading,html_block,paragraph};
        return rules;
    }
}
