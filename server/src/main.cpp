#include <iostream>
#include "resourcemanager.h"
#include "inimanager.h"

#include "mongooseserver.h"
#include <memory>
#include "core.h"
#include "sysinfomanager.h"
#include <thread>
#include <chrono>


using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "Usage: episerver [config file full path]" << std::endl;
        return -1;
    }

    Core theApp;  //Create the endpoints
    theApp.Run(argv[1]);


    return 0;
}
