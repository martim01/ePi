#pragma once
#include <map>
#include <string>
#include "json/json.h"
#include <chrono>

class SysInfoManager
{
    public:
        static SysInfoManager& Get();

        Json::Value GetInfo();

    private:

        SysInfoManager();


        Json::Value GetSysInfo();
        Json::Value GetDiskInfo(const std::string& sPath);
        Json::Value GetCpuInfo();
        Json::Value GetApplicationInfo();

        void ExtractTicks(const std::string& sLine);

        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;

        struct cpu
        {
            cpu(): dUsage(0)
            {
                for(int i = 0; i < 4; i++)
                    nTicks[i] = 0;
            }
            double dUsage;
            unsigned long nTicks[4];
        };

        std::map<std::string, cpu> m_mCpu;

        enum {USER,SYSTEM,NICE,IDLE};

};
