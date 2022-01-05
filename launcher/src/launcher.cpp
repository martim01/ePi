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
#include "epiutils.h"
#include <string.h>
#include <sys/capability.h>
#include <sys/prctl.h>

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




Launcher::Launcher(iniManager& ini, const std::string& sType) : m_ini(ini), m_nType(CONTROLLER)
{
    pml::LogStream::AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));

    if(sType == "1")
    {
        m_nType = CARTCONTROLLER;
    }

    if(m_nType == CONTROLLER)
    {
        m_sRows = m_ini.GetIniString("layout", "rows","1");
        m_sColumns = m_ini.GetIniString("layout", "columns","1");
    }

    FD_ZERO(&m_master);
    FD_ZERO(&m_read);
    m_nMaxFd = -1;
}


bool Launcher::Run()
{
    if(!InheritCapabilities())
    {
        return false;
    }
    if(m_nType == CONTROLLER)
    {
        LaunchAll();
    }
    else
    {
        LaunchCartController();
    }
    Loop();
    return true;
}


void Launcher::LaunchAll()
{
    for(auto itSection = m_ini.GetSectionBegin(); itSection != m_ini.GetSectionEnd(); ++itSection)
    {
        if(itSection->first != "layout")
        {
            Launch(itSection->second);
        }
    }
}

bool Launcher::LaunchCartController()
{
    auto pSection = m_ini.GetSection("cartcontroller");
    if(pSection)
    {
        return Launch(pSection);
    }
    return false;
}

bool Launcher::Launch(const iniSection* pSection)
{
    std::string sPosition = pSection->GetString("position", "-1");
    std::string sIpAddress = pSection->GetString("address", "");
    std::string sPort = pSection->GetString("port", "");

    if((m_nType == CARTCONTROLLER || sPosition != "-1") && sIpAddress.empty() == false && sPort.empty() == false)
    {
        pmlLog(pml::LOG_INFO) << "Launch: " << pSection->GetSectionName() << std::endl;

        int nPipe[2];
        int nError = pipe(nPipe);
        if(nError != 0)
        {
            pmlLog(pml::LOG_ERROR) << "Could not pipe: " << strerror(errno) << std::endl;
            return false;
        }

        int nPid = fork();
        if(nPid < 0)
        {
            pmlLog(pml::LOG_ERROR) << "Could not fork!" << std::endl;
            close(nPipe[1]);
            close(nPipe[0]);
            return false;
        }

        if(nPid > 0)
        {   // Parent
            close(nPipe[1]);  //close write end
            FD_SET(nPipe[0], &m_master);
            m_mControllers.insert(std::make_pair(nPipe[0], controller(pSection, nPid)));
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

            std::string sController;
            int nError(1);
            if(m_nType == CONTROLLER)
            {
                sController = "/usr/local/bin/controller";
                char* args[] = {&sController[0], &m_sColumns[0], &m_sRows[0], &sPosition[0], &sIpAddress[0], &sPort[0], nullptr};
                nError = execv(sController.c_str(), args);
            }
            else
            {
                sController = "/usr/local/bin/cartcontroller";
                char* args[] = {&sController[0], &sIpAddress[0], &sPort[0], nullptr};
                nError = execv(sController.c_str(), args);
            }


            if(nError)
            {
                    pmlLog(pml::LOG_ERROR) << "Exec failed: " << sController << std::endl;
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
            pmlLog() << "PipeThread\tSelect Error" << std::endl;
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
                if(HandleData(itController->second))
                {
                    ++itController;
                }
                else
                {   //false means we are restarting stuff so exit the loop
                    break;
                }
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


bool Launcher::HandleData(const controller& aController)
{
    for(auto str : aController.vLines)
    {
        std::vector<std::string> vData(SplitString(str, ':'));
        if(vData.size() > 0)
        {
            if(vData[0] == "command")
            {
                if(vData[1] == "restart_all")
                {
                    RestartAll();
                    return false;
                }
            }
            else
            {
                //@todo (martim01) logging?
            }
        }
    }
    return true;
}


void Launcher::RestartAll()
{
    ResetMaxFd();
    for(auto pairController : m_mControllers)
    {
        kill(pairController.second.nPid, SIGTERM);
    }
    m_mControllers.clear();
    FD_ZERO(&m_master);
    FD_ZERO(&m_read);
    LaunchAll();

}


bool Launcher::InheritCapabilities()
{
    cap_t caps = cap_get_proc();
    if(caps == nullptr)
    {
        pmlLog(pml::LOG_ERROR) << "Failed to load cap" << std::endl;
        return false;
    }
    std::cout << "Loaded caps = " << cap_to_text(caps, nullptr) << std::endl;

    cap_value_t cap_list[1];
    cap_list[0] = CAP_SYS_ADMIN;
    if(cap_set_flag(caps, CAP_INHERITABLE, 1, cap_list, CAP_SET) == -1)
    {
        pmlLog(pml::LOG_ERROR) << "Failed to set inheritable " << strerror(errno) << std::endl;
        return false;
    }

    std::cout << "Loaded caps = " << cap_to_text(caps, nullptr) << std::endl;

    if(cap_set_proc(caps) == -1)
    {
        pmlLog(pml::LOG_ERROR) << "Failed to set proc " << strerror(errno) << std::endl;
        return false;
    }

    caps = cap_get_proc();
    if(caps == nullptr)
    {
        pmlLog(pml::LOG_ERROR) << "Failed to load cap" << std::endl;
        return false;
    }

    std::cout << "Loaded caps = " << cap_to_text(caps, nullptr) << std::endl;

    if(prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, CAP_SYS_ADMIN,0,0) == -1)
    {
        pmlLog(pml::LOG_ERROR) << "Failed to rasie cap" << std::endl;
        return false;
    }

    return true;
}
