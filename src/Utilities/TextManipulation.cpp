#include "Utilities/TextManipulation.h"

#include <algorithm>
#include <functional>
#include <iostream>

std::string f2str(const char* filename)
{

    std::ifstream ifs(filename);

    std::string   fileContents( (std::istreambuf_iterator<char>(ifs) ),
                                (std::istreambuf_iterator<char>()    ) );

    return fileContents;

}

std::string rmws(std::string & str, const char* ws, char quote)
{

    std::string out;

    /*
     * convert to string type
     * to use find() method
     */
    std::string wsStr = std::string(ws);

    unsigned int i;

    /*
     * Cannot start in the middle
     * of quoted string
     */
    bool inQuotes = false;

    for(i=0;i<str.length();++i)
    {

        /*
         * Check that the next character is not a declared "white space"
         */
        if( (wsStr.find(str[i]) == std::string::npos) || inQuotes )
        {
            out.push_back(str[i]);
        }

        if(str[i] == quote) inQuotes = !inQuotes;

    }

    return out;

}

strvec split(const std::string& str, char delim){

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
