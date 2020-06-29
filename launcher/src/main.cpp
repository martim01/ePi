#include <iostream>
#include "inimanager.h"
#include "inisection.h"
#include "log.h"
#include <string>
#include "launcher.h"

using namespace std;



int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        pml::Log::Get(pml::Log::LOG_INFO) << "Usage: launcher [config file full path]" << std::endl;
        return -1;
    }

    iniManager ini;
    if(ini.ReadIniFile(argv[1]) == false)
    {
        pml::Log::Get(pml::Log::LOG_ERROR) << "Could not read ini file" << std::endl;
        return -1;
    }


    Launcher app(ini);
    app.Run();


    return 0;
}
