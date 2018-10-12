//
// Created by yong gu on 04/10/2018.
//

#ifndef CCMARK_BLOCKSTATE_H
#define CCMARK_BLOCKSTATE_H

#include <string>
#include <vector>

namespace ccm{
    class CoreState;
    class Token;
    class BlockState {
    public:
        BlockState(CoreState& coreState);

        CoreState &coreState;

        Token &pushToken(const Token &token);

        bool isEmpty(int line);

        int skipEmptyLines(int from);

        int skipSpaces(int from);

        int skipSpacesBack(int from, int end);

        int skipChars(int from, char code);

        int skipCharsBack(int from, char code, int end);

        std::string getLines(int begin, int end, int indent, bool keepLastLF);



        int parentIndex = -1;   // index of parent/ancestor "open" token, used for task-list
        std::string parentType;  //different from parentIndex, e.g. inline's parentType may be list (not list_item)
        std::vector<int> bMarks;
        std::vector<int> eMarks;
        std::vector<int> tShift;
        std::vector<int> sCount;
        std::vector<int> bsCount;

        int blkIndent = 0;
        int curLine = 0;
        int maxLine = 0;
        bool tight = false;
        int level = 0;
        std::string result;
    };
}


#endif //CCMARK_BLOCKSTATE_H
