//
// Created by yong gu on 25/09/2018.
//

#include "html_re.h"

namespace ccm {
    using std::string;

    string attr_name = "[a-zA-Z_:][a-zA-Z0-9:._-]*";

    string unquoted = "[^\"\'=<>`\\x00-\\x20]+";
    string single_quoted = "'[^']*'";
    string double_quoted = "\"[^\"]*\"";

    string attr_value = "(?:" + unquoted + '|' + single_quoted + '|' + double_quoted + ")";

    string attribute = "(?:\\s+" + attr_name + "(?:\\s*=\\s*" + attr_value + ")?)";

    string open_tag = "<[A-Za-z][A-Za-z0-9\\-]*" + attribute + "*\\s*\\/?>";

    string close_tag = "<\\/[A-Za-z][A-Za-z0-9\\-]*\\s*>";
    string comment = "<!---->|<!--(?:-?[^>-])(?:-?[^-])*-->";
    string processing = "<[?].*?[?]>";
    string declaration = "<![A-Z]+\\s+[^>]*>";
    string cdata = "<!\\[CDATA\\[[\\s\\S]*?\\]\\]>";

    string HTML_TAG = "^(?:" + open_tag + "|" + close_tag + "|" + comment +
                      "|" + processing + "|" + declaration + '|' + cdata + ')';
    string HTML_OPEN_CLOSE_TAG = "^(?:" + open_tag + '|' + close_tag + ')';

    bool isLetter(char ch) {
        /*eslint no-bitwise:0*/
        unsigned lc = ch | 0x20; // to lower case
        return (lc >= 0x61/* a */) && (lc <= 0x7a/* z */);
    }
}