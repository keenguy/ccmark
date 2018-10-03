//
// Created by yong gu on 27/09/2018.
//

#include "Renderer.h"
#include "helpers/common.h"

using namespace std;
namespace ccm {

    std::string Renderer::renderAttrs(const Token &token) const {
        string result;
        if (token.attrs.empty()) { return ""; }

        result = "";

        for (const auto &attr : token.attrs) {
            result += " " + escapeHtml(attr.first) + "=\"" + escapeHtml(attr.second) + "\"";
        }

        return result;
    }

    std::string Renderer::renderInlineAsText(const vector<Token> &tokens) const {
        string result;

        for (const auto &token : tokens) {
            if (token.type == "text") {
                result += token.content;
            } else if (token.type == "image") {
                result += renderInlineAsText(token.children);
            }
        }

        return result;
    }

    Renderer::Renderer():Renderer(Options()) {
    }

    Renderer::Renderer(const Options& options):options(options) {
        rules["code_inline"] = &Renderer::code_inline;
        rules["code_block"] = &Renderer::code_block;
        rules["fence"] = &Renderer::fence;
        rules["image"] = &Renderer::image;
        rules["hardbreak"] = &Renderer::hardbreak;
        rules["softbreak"] = &Renderer::softbreak;
        rules["text"] = &Renderer::text;
        rules["html_block"] = &Renderer::html_block;
        rules["html_inline"] = &Renderer::html_inline;
    }



    std::string Renderer::code_block( vector<Token> &tokens, int idx) const {

        return "<pre" + renderAttrs(tokens[idx]) + "><code>" +
               escapeHtml(tokens[idx].content) +
               "</code></pre>\n";
    }
    std::string Renderer::code_inline( vector<Token> &tokens, int idx) const {
        return "<code" + renderAttrs(tokens[idx]) + '>' +
               escapeHtml(tokens[idx].content) +
               "</code>";
    }
    std::string Renderer::fence( vector<Token> &tokens, int idx) const {
        Token token = tokens[idx];
        string info = !token.info.empty() ? boost::trim_copy(unescapeAll(token.info)) : "";
        string langName;


        if (!info.empty()) {
            langName = info.substr(0, info.find(' '));
        }

        string highlighted;
//        if (options.highlight) {
//            highlighted = options.highlight(token.content, langName) || escapeHtml(token.content);
//        } else {
//            highlighted = escapeHtml(token.content);
//        }
        highlighted = escapeHtml(token.content);

        if (highlighted.find("<pre") == 0) {
            return highlighted + '\n';
        }

        // If language exists, inject class gently, without modifying original token.
        // May be, one day we will add .clone() for token and simplify this part, but
        // now we prefer to keep things local.
        if (!info.empty()) {

            token.joinAttr("class", options.langPrefix + langName);

            return "<pre><code" + renderAttrs(token) + '>'
                   + highlighted
                   + "</code></pre>\n";
        }
        return "<pre><code" + renderAttrs(token) + '>'
               + highlighted
               + "</code></pre>\n";
    }

    //this modifies tokens, be sure that the modification can occur multiple times safely
    //image may have nested images as its children !
    std::string Renderer::image( vector<Token> &tokens, int idx) const {
        // "alt" attr MUST be set, even if empty. Because it's mandatory and
        // should be placed on proper position for tests.
        // Replace content with actual value

        Token& token = tokens[idx];
        token.setAttr("alt",renderInlineAsText(token.children));

        return renderToken(tokens, idx);
    }



    std::string Renderer::hardbreak( vector<Token> &, int) const {
        return options.xhtmlOut ? "<br />\n" : "<br>\n";
    }

    std::string Renderer::softbreak( vector<Token> &, int) const {
        return options.breaks? (options.xhtmlOut ? "<br />\n" : "<br>\n") : "\n";
    }

    std::string Renderer::text( vector<Token> &tokens, int idx) const {
        return escapeHtml(tokens[idx].content);
    }

    std::string Renderer::html_block( vector<Token> &tokens, int idx) const {
        return tokens[idx].content;
    }
    std::string Renderer::html_inline( vector<Token> &tokens, int idx) const {
        return tokens[idx].content;
    }



    std::string Renderer::renderToken(const vector<Token> &tokens, int idx) const {
//        Token nextToken;
//        string result = "";
//        bool needLf = false;
        const Token &token = tokens[idx];

        // Tight list paragraphs
        if (token.hidden) {
            return "";
        }

        // Insert a newline between hidden paragraph and subsequent opening
        // block-level tag.
        //
        // For example, here we should insert a newline before blockquote:
        //  - a
        //    >
        //
        string result;
        if (token.isBlock && token.nesting != -1 && idx && tokens[idx - 1].hidden) {
            result += '\n';
        }

        // Add token name, e.g. `<img`
        result += (token.nesting == -1 ? "</" : "<") + token.tag;

        // Encode attributes, e.g. `<img src="foo"`
        result += renderAttrs(token);

        // Add a slash for self-closing tags, e.g. `<img src="foo" /`
        if (token.nesting == 0 && options.xhtmlOut) {
            result += " /";
        }

        // Check if we need to add a newline after this tag
        bool needLf = false;
        Token nextToken;
        if (token.isBlock) {
            needLf = true;

            if (token.nesting == 1) {
                if (idx + 1 < tokens.size()) {
                    nextToken = tokens[idx + 1];

                    if (nextToken.type == "inline" || nextToken.hidden) {
                        // Block-level tag containing an inline tag.
                        //
                        needLf = false;

                    } else if (nextToken.nesting == -1 && nextToken.tag == token.tag) {
                        // Opening tag + closing tag of the same type. E.g. `<li></li>`.
                        //
                        needLf = false;
                    }
                }
            }
        }

        result += needLf ? ">\n" : ">";

        return result;
    };

    std::string Renderer::renderInline(vector<Token> &tokens) const {
        string result;
        auto len = tokens.size();
        for (int i = 0; i < len; i++) {
            string type = tokens[i].type;

            if (rules.find(type) != rules.end()) {
                result += (this->*(rules.at(type)))(tokens, i);
            } else {
                result += renderToken(tokens, i);
            }
        }

        return result;
    }

    std::string Renderer::render(std::vector<Token> &tokens, optional<Options> opts) const {
        if (opts){this->options = *opts;}
        string result;
        string type;
        auto len = tokens.size();
        for (int i = 0; i < len; i++) {
            type = tokens[i].type;
            if (type == "inline") {
                result += renderInline(tokens[i].children);
            } else if (rules.find(type) != rules.end()) {
                result += (this->*(rules.at(type)))(tokens, i);
            } else {
                result += renderToken(tokens, i);
            }
        }

        return result;
    }
}
