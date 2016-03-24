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

std::string rmws(const std::string & str, const char* ws, char quote)
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

strvec split(const std::string& str, char delim)
{

    std::stringstream ss(str);
    std::string item;

    strvec tokens;

    while(std::getline(ss,item,delim)) /* the "line" is a string ending in delim */
    {

        std::string item_trimmed = trim(item);

        if(!item_trimmed.empty()) tokens.push_back(item_trimmed);

    }

    return tokens;

}

std::string trim(const std::string& str)
{

    std::string out = str;

    out.erase(out.begin(),
              std::find_if(out.begin(),
                           out.end(),
                           std::not1(std::ptr_fun<int, int>(std::isspace))) );

    out.erase(std::find_if(out.rbegin(),
                           out.rend(),
                           std::not1(std::ptr_fun<int, int>(std::isspace))).base(), out.end());

    return out;

}

std::string trim(const std::string& str, char value)
{

    std::string trimmed = trim(str);

    std::string::iterator it_front;
    std::string::reverse_iterator it_back;

    it_front = trimmed.begin();
    it_back = trimmed.rbegin();

    while(it_front != trimmed.end() && (*it_front == value) ) it_front++;

    while(it_back != trimmed.rend() && (*it_back == value) ) it_back++;

    std::string out(it_front,it_back.base());

    return out;

}
