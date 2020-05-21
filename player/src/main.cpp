#include <iostream>
#include "resourcemanager.h"
#include "inimanager.h"
#include "log.h"
#include "mongoosethread.h"
#include <memory>

using namespace std;

int main()
{

    pml::Log::Get().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));
    iniManager ini;
    ResourceManager rm(ini);
    MongooseThread mt(ini, rm);
    mt.Run();

    getchar();


    return 0;
}
