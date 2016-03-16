#ifndef SPLITSTRING_H
#define SPLITSTRING_H

#include <string>
#include <vector>
#include <sstream>

typedef std::vector<std::string> strvec;

strvec SplitString(const std::string& str, char delim);

#endif
