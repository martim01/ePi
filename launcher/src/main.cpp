#include <iostream>
#include "inimanager.h"
#include "inisection.h"
#include <thread>
#include <chrono>
#include <unistd.h>
#include <string>
#include "log.h"
#include <thread>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>


using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "Usage: launcher [config file full path]" << std::endl;
        return -1;
    }

    iniManager ini;
    if(ini.ReadIniFile(argv[1]) == false)
    {
        std::cout << "Could not read ini file" << std::endl;
        return -1;
    }

    std::string sRows = ini.GetIniString("layout", "rows","1");
    std::string sColumns = ini.GetIniString("layout", "columns","1");

    for(auto itSection = ini.GetSectionBegin(); itSection != ini.GetSectionEnd(); ++itSection)
    {
        if(itSection->first != "layout")
        {
            std::string sPosition = itSection->second->GetString("position", "-1");
            std::string sIpAddress = itSection->second->GetString("address", "");
            std::string sPort = itSection->second->GetString("port", "");

            if(sPosition != "-1" && sIpAddress.empty() == false && sPort.empty() == false)
            {
		std::cout << "Launch: " << itSection->first << std::endl;
                int nPid = fork();
                if(nPid < 0)
                {
                    std::cout << "Could not fork!" << std::endl;
                }
                else if(nPid > 0)
                {   // Parent
                }
                else
                {   //child
                    //int fderr = open("/dev/null", O_WRONLY);
                    //if(fderr >= 0)
                    //{
                    //    dup2(fderr, STDERR_FILENO);
                    //}

                    std::string sController("/usr/local/bin/controller");

                    char* args[] = {&sController[0], &sColumns[0], &sRows[0], &sPosition[0], &sIpAddress[0], &sPort[0], nullptr};
                    int nError = execv(sController.c_str(), args);
                    if(nError)
                    {
                        std::cout << "Exec failed: " << sController << std::endl;
                        exit(-1);
                    }
                }
            }

        }
    }

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
