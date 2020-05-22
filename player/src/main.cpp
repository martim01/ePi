#include <iostream>
#include "resourcemanager.h"
#include "inimanager.h"
#include "log.h"
#include "mongooseserver.h"
#include <memory>
#include "endpoints.h"
using namespace std;

int main()
{

    pml::Log::Get().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));
    iniManager ini;
    ResourceManager rm(ini);

//    rm.AddFile("/home/pi/Downloads/tone.wav", "tone", "30s tone at 48kHz");

    MongooseServer mt(ini);
    Endpoints handler(rm, mt);
    handler.CreateEndpoints();

    mt.Run();

    getchar();


    return 0;
}
