#pragma once
#include "json/json.h"
#include "response.h"
#include <functional>

class Launcher
{
    public:
        Launcher();

        void AddCallbacks(std::function<void(const std::string&)> statusCallback, std::function<void(int)> m_exitCallback);

        response LaunchPlayer(std::string sType, const Json::Value& jsData, int nLoop=0, bool bShuffle=false);

        response PausePlayer();
        response StopPlayer();

        bool IsPlaying() const;

    private:

        void PipeThread();

        pid_t m_pid;
        int m_nPipe[2];
        enum {READ=0, WRITE};

        int m_nExitCode;

        std::string m_sPlayer;

        std::function<void(const std::string&)> m_statusCallback;
        std::function<void(int)> m_exitCallback;
};

