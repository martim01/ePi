#pragma once
#include <map>
#include "inimanager.h"
#include <string>
#include <sys/select.h>
#include <vector>

class iniSection;
class Launcher
{
    public:
        Launcher(iniManager& ini);
        bool Run();

        struct controller
        {
            controller(const iniSection* pS, int nP) : pSection(pS), nPid(nP){}
            const iniSection* pSection;
            std::string sBuffer;
            std::vector<std::string> vLines;
            int nPid;
        };


    private:
        void LaunchAll();
        bool Launch(const iniSection* pSection);
        void Loop();

        void CheckReadSet();
        void ResetMaxFd();

        bool HandleData(const controller& aController);

        void RestartAll();

        iniManager& m_ini;
        std::string m_sRows;
        std::string m_sColumns;
        fd_set m_read;
        fd_set m_master;
        int m_nMaxFd;



        std::map<int, controller> m_mControllers;

};

