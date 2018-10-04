//
// Created by yong gu on 02/10/2018.
//

#ifndef CPP_MARKDOWN_PREPROCESS_H
#define CPP_MARKDOWN_PREPROCESS_H


namespace ccm{
    class CoreState;
    void normalize(CoreState &state);
    void footnote_post(CoreState &state);
//    void replacement(CoreState &state);
//    void smartquotes(CoreState &state);
}

#endif //CPP_MARKDOWN_PREPROCESS_H
