#ifndef TEXTMANIPULATION_H
#define TEXTMANIPULATION_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

typedef std::vector<std::string> strvec;

std::string f2str(const char* filename);

std::string rmws(const std::string & str, const char* ws, char quote);

strvec split(const std::string& str, char delim);

std::string trim(const std::string& str);

std::string trim(const std::string& str, char value);

#endif // TEXTMANIPULATION_H
