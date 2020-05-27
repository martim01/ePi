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

int main()
{


    Core theApp;  //Create the endpoints
    theApp.Run();


    return 0;
}
