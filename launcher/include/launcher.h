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
            controller(const iniSection* pS) : pSection(pS){}
            const iniSection* pSection;
            std::string sBuffer;
            std::vector<std::string> vLines;
        };


    private:

        bool Launch(const iniSection* pSection);
        void Loop();

        void CheckReadSet();
        void ResetMaxFd();

        iniManager& m_ini;
        std::string m_sRows;
        std::string m_sColumns;
        fd_set m_read;
        fd_set m_master;
        int m_nMaxFd;



        std::map<int, controller> m_mControllers;

};

