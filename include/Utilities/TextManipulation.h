#ifndef TEXTMANIPULATION_H
#define TEXTMANIPULATION_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

typedef std::vector<std::string> strvec;

std::string f2str(const char* filename);

std::string rmws(std::string & str, const char* ws, char quote);

strvec split(const std::string& str, char delim);

#endif // TEXTMANIPULATION_H
