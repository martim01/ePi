#pragma once
#include "json/json.h"
#include "response.h"
#include "resourcemanager.h"

class Launcher
{
    public:
        Launcher(ResourceManager& manager);

        response Play(const Json::Value& jsData);
        response Pause(const Json::Value& jsData);
        response Stop(const Json::Value& jsData);


    private:
        response PlayFile(const Json::Value& jsData);
        response PlaySchedule(const Json::Value& jsData);
        response PlayPlaylist(const Json::Value& jsData);

        response LaunchPlayer(std::string sType, const Json::Value& jsData);

        void PipeThread();


        ResourceManager& m_manager;

        pid_t m_pid;
        int m_cout_pipe[2];
        int m_cerr_pipe[2];
        int m_nExitCode;

        std::string m_sPlayer;

        ;
};

