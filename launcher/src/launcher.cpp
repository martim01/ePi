#include "launcher.h"
#include "log.h"
#include <iostream>
#include "inisection.h"
#include <thread>
#include <chrono>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "utils.h"
#include <string.h>


bool ReadFromPipe(int nFd, Launcher::controller& aController)
{
    char buf[500];
    int nRead=read(nFd, buf, sizeof(buf));
    if (nRead > 0)
    {
        aController.sBuffer.append(buf,nRead);

        //do we have at least \n
        size_t nLastLineBreak = aController.sBuffer.find_last_of('\n');
        if(nLastLineBreak != std::string::npos)
        {
            std::string sComplete = aController.sBuffer.substr(0, nLastLineBreak);
            aController.vLines = SplitString(sComplete, '\n');

            if(nLastLineBreak != aController.sBuffer.size()-1)
            {
                aController.sBuffer = aController.sBuffer.substr(nLastLineBreak+1);
            }
            else
            {
                aController.sBuffer.clear();
            }
        }
        return true;
    }

    return false;
}




Launcher::Launcher(iniManager& ini) : m_ini(ini)
{
    m_sRows = m_ini.GetIniString("layout", "rows","1");
    m_sColumns = m_ini.GetIniString("layout", "columns","1");

    FD_ZERO(&m_master);
    FD_ZERO(&m_read);
    m_nMaxFd = -1;
}


bool Launcher::Run()
{

    for(auto itSection = m_ini.GetSectionBegin(); itSection != m_ini.GetSectionEnd(); ++itSection)
    {
        if(itSection->first != "layout")
        {
            Launch(itSection->second);
        }
    }

}


bool Launcher::Launch(const iniSection* pSection)
{
    std::string sPosition = pSection->GetString("position", "-1");
    std::string sIpAddress = pSection->GetString("address", "");
    std::string sPort = pSection->GetString("port", "");

    if(sPosition != "-1" && sIpAddress.empty() == false && sPort.empty() == false)
    {
        pml::Log::Get(pml::Log::LOG_INFO) << "Launch: " << pSection->GetSectionName() << std::endl;

        int nPipe[2];
        int nError = pipe(nPipe);
        if(nError != 0)
        {
            pml::Log::Get(pml::Log::LOG_ERROR) << "Could not pipe: " << strerror(errno) << std::endl;
            return false;
        }

        int nPid = fork();
        if(nPid < 0)
        {
            pml::Log::Get(pml::Log::LOG_ERROR) << "Could not fork!" << std::endl;
            close(nPipe[1]);
            close(nPipe[0]);
            return false;
        }

        if(nPid > 0)
        {   // Parent
            close(nPipe[1]);  //close write end
            FD_SET(nPipe[0], &m_master);
            m_mControllers.insert(std::make_pair(nPipe[0], controller(pSection)));
            m_nMaxFd = std::max(m_nMaxFd, nPipe[0]);
            return true;
        }
        else
        {   //child
            close(nPipe[0]);
            dup2(nPipe[1],STDOUT_FILENO);

            int fderr = open("/dev/null", O_WRONLY);
            if(fderr >= 0)
            {
                dup2(fderr, STDERR_FILENO);
            }

           std::string sController("/usr/local/bin/controller");

            char* args[] = {&sController[0], &m_sColumns[0], &m_sRows[0], &sPosition[0], &sIpAddress[0], &sPort[0], nullptr};
            int nError = execv(sController.c_str(), args);
            if(nError)
            {
                    pml::Log::Get(pml::Log::LOG_ERROR) << "Exec failed: " << sController << std::endl;
                    exit(-1);
            }
            return true;
        }
    }
    return false;
}


void Launcher::Loop()
{
    timespec timeout = {5,0};



    while(true)
    {
        m_read = m_master;

        int nSelect = pselect(m_nMaxFd+1, &m_read, NULL, NULL, &timeout, NULL);
        if(nSelect == 0)
        {
            //timeout
        }
        else if(nSelect > 0)
        {
            CheckReadSet();
        }
        else    //error
        {
            pml::Log::Get() << "PipeThread\tSelect Error" << std::endl;
            break;
        }
    }
}


void Launcher::CheckReadSet()
{
    for(auto itController = m_mControllers.begin(); itController != m_mControllers.end(); )
    {
        if(FD_ISSET(itController->first, &m_read))
        {
            if(ReadFromPipe(itController->first, itController->second))
            {
                //@todo do something with the data
                ++itController;
            }
            else
            {
                //controller closed
                FD_CLR(itController->first, &m_master);
                close(itController->first);

                const iniSection* pSection = itController->second.pSection;

                //remove the controller from our map
                auto itDelete = itController;
                ++itController;
                m_mControllers.erase(itDelete);

                //Reset the max fd.
                ResetMaxFd();

                //relaunch the controller
                Launch(pSection);

            }
        }
        else
        {
            ++itController;
        }
    }
}


void Launcher::ResetMaxFd()
{
    if(m_mControllers.empty())
    {
        m_nMaxFd = -1;
    }
    else
    {
        auto itMax = m_mControllers.end();
        --itMax;
        m_nMaxFd = itMax->first;
    }
}
