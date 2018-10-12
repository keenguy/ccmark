//
// Created by yong gu on 01/10/2018.
//

#include "inline_rules.h"
namespace ccm {
    boost::regex EMAIL_RE(
            "^<([a-zA-Z0-9.!#$%&'*+\\/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*)>");
    boost::regex AUTOLINK_RE("^<([a-zA-Z][a-zA-Z0-9+.\\-]{1,31}):([^<>\\x00-\\x20]*)>");
    bool autolink(InlineState &state, bool silent);
}

bool ccm::autolink(InlineState &state, bool silent) {
//            var tail, linkMatch, emailMatch, url, fullUrl, token,
    int pos = state.pos;

    if (state.src[pos] != 0x3C/* < */) { return false; }

    std::string tail = state.src.substr(pos);

    if (tail.find('>') == std::string::npos) { return false; }

    boost::smatch linkMatch;
    std::string url, href, fullUrl;
    if (boost::regex_search(tail, linkMatch, AUTOLINK_RE)) {
        url = std::string(linkMatch[0].first+1, linkMatch[0].second - 1);
        fullUrl = normalizeLink(url);
        if (!validateLink(fullUrl)) { return false; }

        if (!silent) {
            Token token("link_open", "a", 1);
            token.setAttr("href", fullUrl);
            token.markup = "autolink";
            token.info = "auto";
            state.push(token);

            token = Token("text", "", 0);
            token.content = normalizeLinkText(url);
            state.push(token);

            token = Token("link_close", "a", -1);
            token.markup = "autolink";
            token.info = "auto";
            state.push(token);
        }

        state.pos += linkMatch[0].second - linkMatch[0].first;
        return true;
    }

    boost::smatch emailMatch;
    if (boost::regex_search(tail, emailMatch, EMAIL_RE)) {
        url = std::string(emailMatch[0].first + 1, emailMatch[0].second - 1);
        fullUrl = normalizeLink("mailto:"+url);
        if (!validateLink(fullUrl)) { return false; }

        if (!silent) {
            Token token("link_open", "a", 1);
            token.setAttr("href", fullUrl);
            token.markup = "autolink";
            token.info = "auto";
            state.push(token);

            token = Token("text", "", 0);
            token.content = normalizeLinkText(url);
            state.push(token);

            token = Token("link_close", "a", -1);
            token.markup = "autolink";
            token.info = "auto";
            state.push(token);
        }

        state.pos += emailMatch[0].second - emailMatch[0].first;
        return true;
    }

    return false;
}