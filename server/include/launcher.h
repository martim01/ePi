#pragma once
#include "json/json.h"
#include "response.h"
#include <functional>
#include <memory>
#include <thread>
#include <atomic>

class Launcher
{
    public:
        Launcher();
        ~Launcher();
        void SetPlayer(const std::string& sPath, const std::string& sPlayer, const std::string& sConfigPath);

        void AddCallbacks(std::function<void(const std::string&)> statusCallback, std::function<void(int)> m_exitCallback);

        pml::restgoose::response LaunchPlayer(std::string sType, std::string sUid, int nLoop=0, bool bShuffle=false);

        pml::restgoose::response PausePlayer();
        pml::restgoose::response StopPlayer();

        bool IsPlaying() const;

    private:

        void PipeThread();

        pid_t m_pid;
        int m_nPipe[2];
        enum {READ=0, WRITE};

        int m_nExitCode;

        std::string m_sPlayer;
        std::string m_sConfigPath;
        std::unique_ptr<std::thread> m_pThread;
        std::atomic<bool> m_bRun;

        std::function<void(const std::string&)> m_statusCallback;
        std::function<void(int)> m_exitCallback;
};

