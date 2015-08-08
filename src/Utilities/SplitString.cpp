#include "Utilities/SplitString.h"

void SplitString(std::vector<std::string>& arguments, std::string& line, const std::string& delimiter){

    size_t pos = 0;
    std::string argument;

    while((pos = line.find(delimiter)) != std::string::npos){

        argument = line.substr(0,pos);

        arguments.push_back(argument);

        line.erase(0,pos+delimiter.length());

    }

    arguments.push_back(line);

    line.clear();

}
