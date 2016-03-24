#ifndef DATABLOCK_H
#define DATABLOCK_H

#include <string>
#include <map>

class DataBlock;

typedef std::map<std::string,std::string>::iterator value_it;
typedef std::map<std::string,DataBlock>::iterator block_it;

typedef std::map<std::string,std::string>::const_iterator const_value_it;
typedef std::map<std::string,DataBlock>::const_iterator const_block_it;

class DataBlock {

public:

    DataBlock();
    DataBlock(std::string& data);

    void load(const char* filename);

    //DataBlock operator[](std::string& label) const;
    //DataBlock & operator[](std::string& label);

    DataBlock operator[](const char* label) const;
    DataBlock & operator[](const char*  label);

    //std::string operator()(std::string& label) const;
    //std::string & operator()(std::string& label);

    std::string operator()(const char* label) const;
    std::string & operator()(const char* label);

    value_it beginValues(){ return values.begin(); }
    value_it endValues(){ return values.end(); }

    block_it beginBlocks(){ return childBlocks.begin(); }
    block_it endBlocks(){ return childBlocks.end(); }

private:

    std::map<std::string,std::string> values;

    std::map<std::string,DataBlock> childBlocks;

    void parseData(std::string& data);

};

#endif // DATABLOCK_H
