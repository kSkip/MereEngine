#include "System.h"

System sys;

//main function that just initializes the system object and calls the systems's loop
int main(){
    sys.init();
    return sys.loop();
}
