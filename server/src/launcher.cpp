#include "launcher.h"
#include "epiutils.h"
#include "resource.h"
#include <unistd.h>
#include <string>
#include "log.h"
#include <thread>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

bool ReadFromPipe(int nFd, std::string& sBuffer, std::vector<std::string>& vLines)
{
    char buf[500];
    int nRead=read(nFd, buf, sizeof(buf));
    if (nRead > 0)
    {
        sBuffer.append(buf,nRead);

        //do we have at least \n
        size_t nLastLineBreak = sBuffer.find_last_of('\n');
        if(nLastLineBreak != std::string::npos)
        {
            std::string sComplete = sBuffer.substr(0, nLastLineBreak);
            vLines = SplitString(sComplete, '\n');

            if(nLastLineBreak != sBuffer.size()-1)
            {
                sBuffer = sBuffer.substr(nLastLineBreak+1);
            }
            else
            {
                sBuffer.clear();
            }
        }
        return true;
    }
//    else if(nRead == 0)
//    {
//        pmlLog(pml::LOG_INFO) << "Pipe read EOF" << std::endl;
//    }
//    else
//    {
//        pmlLog(pml::LOG_ERROR) << "Pipe read error: " << nRead << "=" << strerror(nRead) << std::endl;
//    }

    return false;
}



Launcher::Launcher()
: m_pid(0),
m_sPlayer(""),
m_pThread(nullptr),
m_bRun(true),
m_statusCallback(nullptr),
m_exitCallback(nullptr)
{

}

Launcher::~Launcher()
{
    if(m_pThread)
    {
        m_bRun = false;
        m_pThread->join();
    }
}

void Launcher::SetPlayer(const std::string& sPath, const std::string& sPlayer, const std::string& sConfigPath)
{
    m_sPlayer = sPath+sPlayer;
    m_sConfigPath = sConfigPath;
}

void Launcher::AddCallbacks(std::function<void(const std::string&)> statusCallback, std::function<void(int)> exitCallback)
{
    m_statusCallback = statusCallback;
    m_exitCallback = exitCallback;
}

bool Launcher::IsPlaying() const
{
    return (m_pid!=0);
}



pml::restgoose::response Launcher::LaunchPlayer(std::string sType, std::string sUid, int nLoop, bool bShuffle)
{
    pmlLog(pml::LOG_DEBUG) << "Launcher\tLaunchPlayer: ";

    int nError = pipe(m_nPipe);
    if(nError != 0)
    {
        pml::restgoose::response theResponse(500);
        theResponse.jsonData["result"] = "Could not open pipe";
        theResponse.jsonData["error_code"] = nError;
        theResponse.jsonData["error"] = strerror(nError);

        pmlLog(pml::LOG_ERROR) << "could not open pipe: " << strerror(nError) << std::endl;

        return theResponse;
    }


    m_pid = fork();
    if(m_pid < 0)
    {
        close(m_nPipe[WRITE]);
        close(m_nPipe[READ]);
        pml::restgoose::response theResponse(500);
        theResponse.jsonData["result"] = "Could not fork";
        theResponse.jsonData["error_code"] = nError;
        theResponse.jsonData["error"] = strerror(nError);

        pmlLog(pml::LOG_ERROR) << "could not fork: " << strerror(nError) << std::endl;

        return theResponse;
    }
    else if(m_pid > 0)
    {   // Parent
        close(m_nPipe[WRITE]);  //close write end

        pml::restgoose::response theResponse(200);
        theResponse.jsonData["result"] = true;

        //launch the pipe thread
        PipeThread();

        return theResponse;
    }
    else
    {   //child

        std::stringstream ssLoop;
        ssLoop << nLoop;
        std::string sLoop = ssLoop.str();

        std::string sShuffle(bShuffle ? "1" : "0");

        char* args[] = {&m_sPlayer[0], &m_sConfigPath[0], &sType[0], &sUid[0], &sLoop[0], &sShuffle[0], nullptr};


        close(m_nPipe[READ]);
        dup2(m_nPipe[WRITE],STDOUT_FILENO);

        int fderr = open("/dev/null", O_WRONLY);
        if(fderr >= 0)
        {
            dup2(fderr, STDERR_FILENO);
        }


        nError = execv(m_sPlayer.c_str(), args);

        if(nError)
        {
            std::cout << "Exec failed: " << m_sPlayer << std::endl;
            exit(-1);
        }
    }
    return pml::restgoose::response(200);
}


pml::restgoose::response Launcher::StopPlayer()
{
    pml::restgoose::response theResponse;
    if(m_pid == 0)
    {
        theResponse.nHttpCode = 409;
        theResponse.jsonData["result"] = false;
        theResponse.jsonData["reason"].append("Player not running");
        pmlLog(pml::LOG_WARN) << "- Player not running" << std::endl;
    }
    else
    {
        int nError = kill(m_pid, SIGTERM);
        if(nError != 0)
        {
            theResponse.nHttpCode = 500;
            theResponse.jsonData["result"] = "Could not send signal to player";
            theResponse.jsonData["reason"] = strerror(nError);
            pmlLog(pml::LOG_ERROR) << "- Could not send signal to player" << std::endl;
        }
        else
        {
            theResponse.jsonData["result"] = true;
            pmlLog(pml::LOG_INFO) << "- Signal sent to player" << std::endl;
        }
    }
    return theResponse;
}

pml::restgoose::response Launcher::PausePlayer()
{
    pml::restgoose::response theResponse;
    if(m_pid == 0)
    {
        theResponse.nHttpCode = 409;
        theResponse.jsonData["result"] = false;
        theResponse.jsonData["reason"].append("Player not running");
        pmlLog(pml::LOG_WARN) << "- Player not running" << std::endl;
    }
    return theResponse;
}

void Launcher::PipeThread()
{
    if(m_pThread)
    {
        m_bRun = false;
        m_pThread->join();
        m_bRun = true;
        m_pThread = nullptr;
    }
    m_pThread = std::make_unique<std::thread>([this]()
    {
        fd_set read_set;
        memset(&read_set, 0, sizeof(read_set));
        FD_SET(m_nPipe[READ], &read_set);

        timespec timeout = {5,0};

        std::string sOut, sError;
        std::vector<std::string> vOut;
        std::vector<std::string> vError;

        while(m_bRun)
        {
            FD_SET(m_nPipe[READ], &read_set);

            int nSelect = pselect(m_nPipe[READ]+1, &read_set, NULL, NULL, &timeout, NULL);
            if(nSelect == 0)
            {
                //timeout
                //no messages for 5 seconds? player must have hung. Kill it and report error
                kill(m_pid, SIGKILL);
                FD_CLR(m_nPipe[READ], &read_set);
                close(m_nPipe[READ]);
                m_nPipe[READ] = -1;
                pmlLog(pml::LOG_ERROR) << "PipeThread\tTimeout" << std::endl;
                break;
            }
            else if(nSelect > 0)
            {
                if(FD_ISSET(m_nPipe[READ], &read_set))
                {
                    if(ReadFromPipe(m_nPipe[READ], sOut, vOut))
                    {
                        if(m_statusCallback)
                        {
                            for(size_t i = 0; i < vOut.size(); i++)
                            {
                                m_statusCallback(vOut[i]);
                            }
                        }
                    }
                    else
                    {
                        FD_CLR(m_nPipe[READ], &read_set);
                        close(m_nPipe[READ]);
                        m_nPipe[READ] = -1;
                        pmlLog() << "PipeThread\tEOF or Read Error" << std::endl;
                        break;

                    }

                }
            }
            else    //error
            {
                pmlLog() << "PipeThread\tSelect Error" << std::endl;
                break;
            }
        }
        if(m_bRun)  //if we are exiting the application don't wait for the player to exit
        {
            int nStatus;
            waitpid(m_pid, &nStatus,0);
            m_pid = 0;
            if(m_exitCallback)
            {
                m_exitCallback(nStatus);
            }
        }

    });
}


