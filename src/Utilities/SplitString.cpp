#include "Utilities/SplitString.h"

#include <algorithm>
#include <functional>
#include <iostream>

strvec SplitString(const std::string& str, char delim){

    std::stringstream ss(str);
    std::string item;

    strvec tokens;

    while(std::getline(ss,item,delim)) /* the "line" is a string ending in delim */
    {

        item.erase(item.begin(),
                   std::find_if(item.begin(),
                                item.end(),
                                std::not1(std::ptr_fun<int, int>(std::isspace))) );

        item.erase(std::find_if(item.rbegin(),
                                item.rend(),
                                std::not1(std::ptr_fun<int, int>(std::isspace))).base(), item.end());

        if(!item.empty()) tokens.push_back(item);

    }

    return tokens;

}
