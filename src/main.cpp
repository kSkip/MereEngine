#include "System.h"

System sys;

//main function that just initializes the system object and calls the systems's loop
int main(){

    /*
     * This is changed to the directory root of the game data
     */
    std::string root = "/home/kane/MereEngineDemo/";

    if(!sys.init(root))
        return 0;

    return sys.loop();

}
