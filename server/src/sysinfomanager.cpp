#include "sysinfomanager.h"
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <fstream>
#include "utils.h"
#include "version.h"
#include <algorithm>
#include "log.h"



SysInfoManager::SysInfoManager() :
 m_sPath("/"),
 m_startTime(std::chrono::high_resolution_clock::now())
{
    m_ntp.Start();
}

SysInfoManager::~SysInfoManager()
{
    m_ntp.Stop();
}

Json::Value SysInfoManager::GetSysInfo()
{
    Json::Value jsInfo;
    struct sysinfo info;
    int nError = sysinfo(&info);
    if(nError !=0)
    {
        jsInfo["error_code"] = nError;
        jsInfo["error"] = strerror(nError);
    }
    else
    {
        Json::Value jsRam;
        jsRam["total"] = static_cast<Json::UInt64>(info.totalram);
        jsRam["free"] = static_cast<Json::UInt64>(info.freeram);
        jsRam["shared"] = static_cast<Json::UInt64>(info.sharedram);
        jsRam["buffered"] = static_cast<Json::UInt64>(info.bufferram);

        Json::Value jsSwap;
        jsSwap["total"] = static_cast<Json::UInt64>(info.totalswap);
        jsSwap["free"] = static_cast<Json::UInt64>(info.freeswap);

        Json::Value jsHigh;
        jsHigh["total"] = static_cast<Json::UInt64>(info.totalhigh);
        jsHigh["free"] = static_cast<Json::UInt64>(info.freehigh);

        float fload = 1.f/(1 << SI_LOAD_SHIFT);
        fload *=100/get_nprocs();
        Json::Value jsLoads;
        jsLoads["1"] = (info.loads[0]*fload);
        jsLoads["5"] = (info.loads[1]*fload);
        jsLoads["15"] = (info.loads[2]*fload);

        jsInfo["uptime"] = static_cast<Json::UInt64>(info.uptime);
        jsInfo["procs"] = static_cast<Json::UInt64>(info.procs);

        jsInfo["ram"] = jsRam;
        jsInfo["swap"] = jsSwap;
        jsInfo["high"] = jsHigh;
        jsInfo["loads"] = jsLoads;

    }
    return jsInfo;
}


Json::Value SysInfoManager::GetDiskInfo()
{
    Json::Value jsInfo;

    struct statvfs info;
    int nError = statvfs(m_sPath.c_str(), &info);
    if(nError !=0)
    {
        jsInfo["error_code"] = nError;
        jsInfo["error"] = strerror(nError);
    }
    else
    {
        jsInfo["bytes"]["available"] = static_cast<Json::UInt64>(info.f_bavail)*static_cast<Json::UInt64>(info.f_bsize);
        jsInfo["bytes"]["free"] =      static_cast<Json::UInt64>(info.f_bfree)*static_cast<Json::UInt64>(info.f_bsize);
        jsInfo["bytes"]["total"] =     static_cast<Json::UInt64>(info.f_blocks)*static_cast<Json::UInt64>(info.f_frsize);
        jsInfo["bytes"]["blocks_total"] =     static_cast<Json::UInt64>(info.f_blocks);
        jsInfo["bytes"]["frsize"] =     static_cast<Json::UInt64>(info.f_frsize);
        jsInfo["bytes"]["bsize"] =     static_cast<Json::UInt64>(info.f_bsize);
        jsInfo["bytes"]["blocks_free"] =     static_cast<Json::UInt64>(info.f_bfree);

        jsInfo["inodes"]["available"] = static_cast<Json::UInt64>(info.f_favail);
        jsInfo["inodes"]["free"] = static_cast<Json::UInt64>(info.f_ffree);
        jsInfo["inodes"]["total"] = static_cast<Json::UInt64>(info.f_files);
    }
    return jsInfo;
}


Json::Value SysInfoManager::GetInfo()
{
    Json::Value jsInfo;
    jsInfo["system"] = GetSysInfo();
    jsInfo["disk"] = GetDiskInfo();
    jsInfo["cpu"] = GetCpuInfo();
    jsInfo["application"] = GetApplicationInfo();
    jsInfo["temperature"] = GetTemperature();
    jsInfo["ntp"] = m_ntp.GetStatus();
    return jsInfo;
}

Json::Value SysInfoManager::GetCpuInfo()
{
    Json::Value jsInfo;

    std::ifstream ifs;
    ifs.open("/proc/stat");
    if(ifs.is_open() == false)
    {
        jsInfo["error"] = "Could not open /proc/stat";
    }
    else
    {
        ifs.clear();
        std::string sLine;
        while(ifs.eof() == false)
        {
            getline(ifs,sLine,'\n');

            if(sLine.substr(0,3) == "cpu")
            {
                ExtractTicks(sLine);
            }
        }

        for(auto pairCpu : m_mCpu)
        {
            jsInfo[pairCpu.first] = static_cast<int>(pairCpu.second.dUsage*100.0);
        }
    }
    return jsInfo;
}


void SysInfoManager::ExtractTicks(const std::string& sLine)
{
    std::vector<std::string> vSplit(SplitString(sLine, ' '));
    if(vSplit.size() >= 5)
    {
        unsigned long nTicks[4];
        unsigned long nDifference[4];
        std::map<std::string, cpu>::iterator itCpu = m_mCpu.insert(std::make_pair(vSplit[0], cpu())).first;

        for(int i = 0; i < 4; i++)
        {
            try
            {
                nTicks[i] = std::stoul(vSplit[i+1]);
            }
            catch(const std::invalid_argument& e)
            {
                nTicks[i] = 0;
            }

            nDifference[i] = nTicks[i]-itCpu->second.nTicks[i];
            itCpu->second.nTicks[i] = nTicks[i];
        }

        itCpu->second.dUsage = static_cast<double>(nDifference[USER]+nDifference[SYSTEM]+nDifference[NICE])/static_cast<double>(nDifference[USER]+nDifference[SYSTEM]+nDifference[NICE]+nDifference[IDLE]);

    }
}


Json::Value SysInfoManager::GetApplicationInfo()
{
    Json::Value jsInfo;

    //@todo(martim01) Get version number of this app and also the player
    std::stringstream ssVersion;
    ssVersion << version::MAJOR << "." << version::MINOR << "." << version::PATCH << "." << version::BUILD;
    jsInfo["version"] = ssVersion.str();
    jsInfo["date"] = ConvertTimeToIsoString(std::time_t(version::DATE));
    jsInfo["start_time"] = ConvertTimeToIsoString(m_startTime);


    std::chrono::time_point<std::chrono::high_resolution_clock> tp(std::chrono::high_resolution_clock::now());

    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()-m_startTime.time_since_epoch());

    jsInfo["up_time"] = uptime.count();
    return jsInfo;
}

Json::Value SysInfoManager::GetTemperature()
{
    Json::Value jsInfo;
    std::ifstream ifs;
    ifs.open("/sys/class/thermal/thermal_zone0/temp");
    if(ifs.is_open() == false)
    {
        jsInfo["error"] = "Could not open /sys/class/thermal/thermal_zone0/temp";
    }
    else
    {
        ifs.clear();
        std::string sLine;
        while(ifs.eof() == false)
        {
            getline(ifs,sLine,'\n');
            try
            {
                double dTemp = std::stod(sLine);
                dTemp /= 1000.0;
                jsInfo["cpu"] = dTemp;
            }
            catch(const std::invalid_argument& e)
            {
                jsInfo["error"] = "Temp in invalid format";
            }
        }
    }
    return jsInfo;
}


