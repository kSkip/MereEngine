#include "Utilities/DataBlock.h"
#include "Utilities/TextManipulation.h"

DataBlock::DataBlock()
{

}

DataBlock::DataBlock(std::string& data)
{

    parseData(data);

}

void DataBlock::load(const char* filename)
{

    std::string contents = f2str(filename);

    std::string data = rmws(contents," \n\r\t",'\"');

    parseData(data);

}

DataBlock DataBlock::operator[](const char* label)
{

    std::string key(label);

    return childBlocks[key];

}

std::string DataBlock::operator()(const char* label)
{

    std::string key(label);

    return values[key];

}

void DataBlock::parseData(std::string& data)
{

    std::string::iterator it = data.begin();
    std::string::iterator trail = it;

    while(it != data.end())
    {

        if(*it == '=') //found a value assignment
        {

            std::string label(trail,it++);
            trail = it;

            /*
             * value assignments are terminated
             * by a semi-colon
             */
            while(it != data.end() && *it != ';') it++;

            std::string value(trail,it++);
            trail = it;

            values[label] = value;

        }
        else if(*it == '{') //found a nested DataBlock
        {

            std::string label(trail,it++);
            trail = it;

            /*
             * brackets variable helps identify the closing
             * braces of the current nesting depth
             */
            size_t brackets = 0;

            while(it != data.end() && (*it != '}' || brackets ))
            {

                if(*it == '{') brackets++;
                if(*it == '}') brackets--;

                it++;

            }

            std::string childData(trail,it++);
            trail = it;

            childBlocks[label] = DataBlock(childData);

        }
        else it++;

    }

}
