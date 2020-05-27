#include "sysinfomanager.h"
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <fstream>
#include "utils.h"

SysInfoManager::SysInfoManager() :
 m_startTime(std::chrono::high_resolution_clock::now())
{

}

SysInfoManager& SysInfoManager::Get()
{
    static SysInfoManager sys;
    return sys;
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

        Json::Value jsLoads;
        jsLoads["1"] = static_cast<Json::UInt64>(info.loads[0]);
        jsLoads["5"] = static_cast<Json::UInt64>(info.loads[1]);
        jsLoads["15"] = static_cast<Json::UInt64>(info.loads[2]);

        jsInfo["uptime"] = static_cast<Json::UInt64>(info.uptime);
        jsInfo["procs"] = static_cast<Json::UInt64>(info.procs);

        jsInfo["ram"] = jsRam;
        jsInfo["swap"] = jsSwap;
        jsInfo["high"] = jsHigh;
        jsInfo["loads"] = jsLoads;

    }
    return jsInfo;
}


Json::Value SysInfoManager::GetDiskInfo(const std::string& sPath)
{
    Json::Value jsInfo;

    struct statvfs info;
    int nError = statvfs(sPath.c_str(), &info);
    if(nError !=0)
    {
        jsInfo["error_code"] = nError;
        jsInfo["error"] = strerror(nError);
    }
    else
    {
        jsInfo["bytes"]["available"] = static_cast<Json::UInt64>(info.f_bavail*info.f_bsize);
        jsInfo["bytes"]["free"] =      static_cast<Json::UInt64>(info.f_bfree*info.f_bsize);
        jsInfo["bytes"]["total"] =     static_cast<Json::UInt64>(info.f_blocks * info.f_bsize);

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
    jsInfo["disk"] = GetDiskInfo("/var/ePi");   // @todo(martim01) get the path from the inimanager
    jsInfo["cpu"] = GetCpuInfo();
    jsInfo["application"] = GetApplicationInfo();

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
    jsInfo["version"] = "?";
    jsInfo["start_time"] = ConvertTimeToIsoString(m_startTime);

    std::chrono::time_point<std::chrono::high_resolution_clock> tp(std::chrono::high_resolution_clock::now());

    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()-m_startTime.time_since_epoch());

    jsInfo["up_time"] = uptime.count();
    return jsInfo;
}
