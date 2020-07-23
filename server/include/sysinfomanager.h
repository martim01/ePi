#pragma once
#include <map>
#include <string>
#include "json/json.h"
#include <chrono>

class SysInfoManager
{
    public:
        SysInfoManager();
        void SetDiskPath(const std::string& sPath)
        {
            m_sPath = sPath;
        }
        Json::Value GetInfo();

    private:

        std::string m_sPath;


        Json::Value GetSysInfo();
        Json::Value GetDiskInfo();
        Json::Value GetCpuInfo();
        Json::Value GetApplicationInfo();
        Json::Value GetTemperature();

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
