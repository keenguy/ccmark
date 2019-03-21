//
// Created by yong gu on 27/09/2018.
//

#ifndef CPP_MARKDOWN_RENDERER_H
#define CPP_MARKDOWN_RENDERER_H

#include <string>
#include <vector>
#include "token.h"
#include "options.h"

namespace ccm {
    class CoreState;

    class Renderer {
    public:
        Renderer();
        mutable Options options;
        typedef std::string (Renderer::* RenderRule)(std::vector<Token>& tokens, int idx) const;

        std::unordered_map<std::string, RenderRule> rules;

        std::string render(std::vector<Token> &tokens, Options options) const;


    private:
        std::string code_inline(std::vector<Token> &tokens, int idx) const;

        std::string code_block(std::vector<Token> &tokens, int idx) const;

        std::string fence(std::vector<Token> &tokens, int idx) const;

        std::string image(std::vector<Token> &tokens, int idx) const;

        std::string hardbreak(std::vector<Token> &tokens, int idx) const;

        std::string softbreak(std::vector<Token> &tokens, int idx) const;

        std::string text(std::vector<Token> &tokens, int idx) const;

        std::string html_block(std::vector<Token> &tokens, int idx) const;

        std::string html_inline(std::vector<Token> &tokens, int idx) const;

        std::string renderAttrs(const Token &token) const;

        std::string renderInlineAsText(const std::vector<Token> &tokens) const;

        std::string renderToken(const std::vector<Token> &tokens, int idx) const;

        std::string renderInline(std::vector<Token> &tokens) const;

        std::string checkbox(std::vector<Token> &tokens, int idx) const;
        std::string math_block(std::vector<Token> &tokens, int idx) const;
        std::string math_inline(std::vector<Token> &tokens, int idx) const;
        std::string footnote_ref(std::vector<Token> &tokens, int idx) const;
        std::string footnote_block_open(std::vector<Token> &tokens, int idx) const;
        std::string footnote_block_close(std::vector<Token> &tokens, int idx) const;
        std::string footnote_open(std::vector<Token> &tokens, int idx) const;
        std::string footnote_close(std::vector<Token> &tokens, int idx) const;
        std::string footnote_anchor(std::vector<Token> &tokens, int idx) const;

    };
}


#endif //CPP_MARKDOWN_RENDERER_H
