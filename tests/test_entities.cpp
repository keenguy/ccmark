//
// Created by yong gu on 24/09/2018.
//

#include <iostream>


int main()
{
    char line[100];
    std::cout << "Enter encoded line: ";
    std::cin.getline(line, sizeof line);
    string str(line);
    str = unescapeAll(str);
    std::cout << str;
    return 0;
}