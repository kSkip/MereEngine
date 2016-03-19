#ifndef DATABLOCK_H
#define DATABLOCK_H

#include <string>
#include <map>

class DataBlock {

public:

    DataBlock();
    DataBlock(std::string& data);

    void load(const char* filename);

    //DataBlock operator[](std::string& label) const;
    //DataBlock & operator[](std::string& label);

    DataBlock operator[](const char* label);
    //DataBlock & operator[](const char*  label);

    //std::string operator()(std::string& label) const;
    //std::string & operator()(std::string& label);

    std::string operator()(const char* label);
    //std::string & operator()(const char* label);

private:

    std::map<std::string,std::string> values;

    std::map<std::string,DataBlock> childBlocks;

    void parseData(std::string& data);

};

#endif // DATABLOCK_H
