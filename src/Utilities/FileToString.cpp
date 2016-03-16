#include "Utilities/FileToString.h"

std::string FileToString(const char* filename)
{
    std::ifstream ifs(filename);
    std::string   fileContents( (std::istreambuf_iterator<char>(ifs) ),
                                (std::istreambuf_iterator<char>()    ) );

    return fileContents;

}
