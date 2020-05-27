#include "launcher.h"
#include "utils.h"
#include "resource.h"
#include <spawn.h>
#include <unistd.h>
#include <string>


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

    return false;
}



Launcher::Launcher(ResourceManager& manager)  : m_manager(manager), m_pid(0)
{

}


response Launcher::Play(const Json::Value& jsData)
{

    //check if player is running
    if(m_pid != 0)
    {
        response theResponse(409);
        theResponse.jsonData["result"] = "Player already running";
        return theResponse;
    }

    //if not check the data is valid
    if(jsData["type"].isString() == false)
    {
        response theResponse(400);
        theResponse.jsonData["result"] = "Type not set";
        return theResponse;
    }
    else if(jsData["uid"].isString() == false)
    {
        response theResponse(400);
        theResponse.jsonData["result"] = "uid not set";
        return theResponse;
    }
    else if(CmpNoCase(jsData["type"].asString(), "file"))
    {
        return PlayFile(jsData);
    }
    else if(CmpNoCase(jsData["type"].asString(), "playlist"))
    {
        return PlayPlaylist(jsData);
    }
    else if(CmpNoCase(jsData["type"].asString(), "schedule"))
    {
        return PlaySchedule(jsData);
    }
    else
    {
        response theResponse(400);
        theResponse.jsonData["result"] = "Type not valid";
        return theResponse;
    }
}

response Launcher::PlayFile(const Json::Value& jsData)
{
    if(m_manager.FindFile(jsData["uid"].asString()) == m_manager.GetFilesEnd())
    {
        response theResponse(404);
        theResponse.jsonData["result"] = "file not found";
        return theResponse;
    }
    else
    {
        return LaunchPlayer("file", jsData["uid"]);
    }

}

response Launcher::PlaySchedule(const Json::Value& jsData)
{
    auto itPlaylist = m_manager.FindPlaylist(jsData["uid"].asString());
    if(itPlaylist == m_manager.GetPlaylistsEnd())
    {
        response theResponse(404);
        theResponse.jsonData["result"] = "playlist not found";
        return theResponse;
    }
    else
    {
        return LaunchPlayer("playlist", itPlaylist->second->GetJson());
    }
}

response Launcher::PlayPlaylist(const Json::Value& jsData)
{
    auto itSchedule = m_manager.FindSchedule(jsData["uid"].asString());
    if(itSchedule == m_manager.GetSchedulesEnd())
    {
        response theResponse(404);
        theResponse.jsonData["result"] = "schedule not found";
        return theResponse;
    }
    else
    {
        return LaunchPlayer("schedule", itSchedule->second->GetJson());
    }

}

response Launcher::Pause(const Json::Value& jsData)
{
    if(m_pid == 0)
    {
        response theResponse(409);
        theResponse.jsonData["result"] = "Player not running";
        return theResponse;
    }

    // @todo(martim01) send a signal to tell child to pause
}

response Launcher::Stop(const Json::Value& jsData)
{
    if(m_pid == 0)
    {
        response theResponse(409);
        theResponse.jsonData["result"] = "Player not running";
        return theResponse;
    }
    // @todo(martim01) send a signal to tell the child to stop
}


response Launcher::LaunchPlayer(std::string sType, const Json::Value& jsData)
{

    posix_spawn_file_actions_t action;

    int nError = pipe(m_cout_pipe);
    if(nError != 0)
    {
        response theResponse(500);
        theResponse.jsonData["result"] = "Could not open cout pipe";
        theResponse.jsonData["error_code"] = nError;
        theResponse.jsonData["error"] = strerror(nError);
        return theResponse;
    }

    nError = pipe(m_cerr_pipe);
    if(nError != 0)
    {
        close(m_cout_pipe[0]);
        close(m_cout_pipe[1]);
        response theResponse(500);
        theResponse.jsonData["result"] = "Could not open cerr pipe";
        theResponse.jsonData["error_code"] = nError;
        theResponse.jsonData["error"] = strerror(nError);
        return theResponse;
    }

    posix_spawn_file_actions_init(&action);
    posix_spawn_file_actions_addclose(&action, m_cout_pipe[0]);
    posix_spawn_file_actions_addclose(&action, m_cerr_pipe[0]);
    posix_spawn_file_actions_adddup2(&action, m_cout_pipe[1],1);
    posix_spawn_file_actions_adddup2(&action, m_cerr_pipe[1],2);
    posix_spawn_file_actions_addclose(&action, m_cout_pipe[1]);
    posix_spawn_file_actions_addclose(&action, m_cerr_pipe[1]);

    std::stringstream ssData;
    ssData << jsData;
    std::string sData = ssData.str();

    char* args[] = {&sType[0], &sData[0], nullptr};

    //Launc the application
    nError = posix_spawn(&m_pid, m_sPlayer.c_str(), &action, NULL, &args[0], NULL);
    if(nError)
    {
        response theResponse(500);
        theResponse.jsonData["result"] = "Could not launch player";
        theResponse.jsonData["error_code"] = nError;
        theResponse.jsonData["error"] = strerror(nError);
        return theResponse;
    }

    close(m_cout_pipe[1]);
    close(m_cerr_pipe[1]);

    //launch the pipe thread
    PipeThread();

    response theResponse(200);
    theResponse.jsonData["result"] = "Success";

    return theResponse;
}



void Launcher::PipeThread()
{
    std::thread th([this]()
    {
        fd_set read_set;
        memset(&read_set, 0, sizeof(read_set));
        FD_SET(m_cout_pipe[0], &read_set);
        FD_SET(m_cerr_pipe[0], &read_set);

        int nMaxFd = std::max(m_cout_pipe[0], m_cerr_pipe[0]);
        timespec timeout = {5,0};

        std::string sOut, sError;
        std::vector<std::string> vOut;
        std::vector<std::string> vError;

        int nSelect = pselect(nMaxFd+1, &read_set, NULL, NULL, &timeout, NULL);
        if(nSelect == 0)
        {
            //timeout
            // @todo(martim01) no messages for 5 seconds? player must have hung. Kill it and report error
        }
        else if(nSelect > 0)
        {
            if(FD_ISSET(m_cout_pipe[0], &read_set))
            {
                if(ReadFromPipe(m_cout_pipe[0], sOut, vOut))
                {
                    // @todo(martim01) Turn in to JSON and sent to server
                }
                else
                {
                    FD_CLR(m_cout_pipe[0], &read_set);
                    close(m_cout_pipe[0]);
                    m_cout_pipe[0] = -1;

                    //have to assume player has closed...
                    // @todo(martim01) exit the thread and let everyone know player has exited
                }

            }
            else if(FD_ISSET(m_cerr_pipe[0], &read_set))
            {
                if(ReadFromPipe(m_cerr_pipe[0], sError, vError))
                {
                    // @todo(martim01) Turn in to JSON and send to server
                }
                else
                {
                    FD_CLR(m_cerr_pipe[0], &read_set);
                    close(m_cerr_pipe[0]);
                    m_cerr_pipe[0] = -1;

                    //have to assume player has closed...
                    // @todo(martim01) exit the thread and let everyone know player has exited
                }
            }
        }
        else    //error
        {
            //probably means player has closed...
            // @todo(martim01) exit the thread and let everyone know player has exited
        }
    });
    th.detach();
}


