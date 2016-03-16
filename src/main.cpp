#include "System.h"

System sys;

//main function that just initializes the system object and calls the systems's loop
int main(){

    std::string root = "/home/kane/MereEngineData/";

    if(!sys.init(root))
        return 0;

    return sys.loop();

}
