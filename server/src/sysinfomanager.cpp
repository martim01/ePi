#include "sysinfomanager.h"
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <fstream>
#include "utils.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <error.h>
#include <algorithm>
#include "log.h"

const std::string SysInfoManager::clksrcname[10] = {  /* Refer RFC-1305, Appendix B, Section 2.2.1 */
    "unspecified",    /* 0 */
    "atomic clock",   /* 1 */
    "VLF radio",      /* 2 */
    "HF radio",       /* 3 */
    "UHF radio",      /* 4 */
    "local net",      /* 5 */
    "NTP server",     /* 6 */
    "UDP/TIME",       /* 7 */
    "wristwatch",     /* 8 */
    "modem"};         /* 9 */

const std::string SysInfoManager::DISP = "rootdisp=";
const std::string SysInfoManager::DELAY = "rootdelay=";
const std::string SysInfoManager::STRATUM = "stratum=";
const std::string SysInfoManager::POLL = "tc=";
const std::string SysInfoManager::REFID = "refid=";


SysInfoManager::SysInfoManager() :
 m_sPath("/"),
 m_startTime(std::chrono::high_resolution_clock::now())
{

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
    jsInfo["ntp"] = GetNtpStatus();
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


Json::Value SysInfoManager::GetNtpStatus()
{


    Json::Value jsValue;

    timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    fd_set fds;
    FD_ZERO(&fds);

    struct sockaddr_in sock;
    struct in_addr address;

    inet_aton("127.0.0.1", &address);
    sock.sin_family = AF_INET;
    sock.sin_addr = address;
    sock.sin_port = htons(NTP_PORT);

    /*----------------------------------------------------------------*/
    /* Compose the command message */
    ntp ntpmsg;
    memset ( &ntpmsg, 0, sizeof(ntpmsg));
    ntpmsg.byte1 = B1VAL;
    ntpmsg.byte2 = B2VAL;  ntpmsg.sequence=htons(1);
    /*---------------------------------------------------------------------*/
    /* Send the command message */
    int sd;        /* file descriptor for socket */

    if ((sd = socket (PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        jsValue["error"] = "unable to open socket";
        return jsValue;
    }

    if (connect(sd, (struct sockaddr *)&sock, sizeof(sock)) < 0)
    {
        jsValue["error"] = "unable to connect to socket";
        return jsValue;
    }

    FD_SET(sd, &fds);

    if (send(sd, &ntpmsg, sizeof(ntpmsg), 0) < 0)
    {
        jsValue["error"] = "unable to send command to NTP port";
        return jsValue;
    }
    /*----------------------------------------------------------------------*/
    /* Receive the reply message */
    int n = select (sd+1, &fds, nullptr, nullptr , &tv);

    if (n == 0)
    {
        jsValue["error"] = "timeout from ntp";
        return jsValue;
    }

    if (n == -1)
    {
        jsValue["error"] = "error on select";
        return jsValue;
    }

    if ((n = recv (sd, &ntpmsg, sizeof(ntpmsg), 0)) < 0)
    {
        jsValue["error"] = "Unable to talk to NTP daemon. Is it running?";
        return jsValue;
    }

    /*----------------------------------------------------------------------*/
    /* Interpret the received NTP control message */
    /* For the reply message to be valid, the first byte should be as sent,
     and the second byte should be the same, with the response bit set */

    unsigned char byte1ok = ((ntpmsg.byte1&0x3F) == B1VAL);
    unsigned char byte2ok = ((ntpmsg.byte2 & ~MMASK) == (B2VAL|RMASK));
    if (!(byte1ok && byte2ok))
    {
        jsValue["error"] = "return data appears to be invalid based on status word";
        return jsValue;
    }

    if (!(ntpmsg.byte2 | EMASK))
    {
        fprintf (stderr,"status byte2 is %02x\n", ntpmsg.byte2 );
        jsValue["error"] = "error bit is set in reply";
        return jsValue;
    }

    if (!(ntpmsg.byte2 | MMASK))
    {
        jsValue["error"] = "More bit unexpected in reply";
    }

    /* if the leap indicator (LI), which is the two most significant bits
     in status byte1, are both one, then the clock is not synchronised. */
    if ((ntpmsg.status1 >> 6) == 3)
    {
        jsValue["synchronised"] = false;

        /* look at the system event code and see if indicates system restart */
        if ((ntpmsg.status2 & 0x0F) == 1)
        {
            jsValue["restarting"] = true;
        }
        else
        {
            jsValue["restarting"] = false;
        }
    }
    else
    {
        jsValue["synchronised"] = true;

        unsigned int clksrc = (ntpmsg.status1 & 0x3F);
        if (clksrc < 10)
        {
            jsValue["source"] = clksrcname[clksrc];
        }
        else
        {
            jsValue["source"] = "Unknown";
        }



//        strncpy(buff, ntpmsg.payload, sizeof(buff));
//        if ((newstr = strstr (buff, STRATUM)))
//        {
//            newstr += sizeof(STRATUM) - 1;
//            dispstr = strtok(newstr,",");
//
//            if ((strlen (dispstr) == 0) || (strlen (dispstr) > 2))
//            {
//                printf (", stratum unreadable\n");
//            }
//            else
//            {
//                printf(" at stratum %s \n",dispstr);
//            }
//        }
//        else
//        {
//            rc=1;
//            printf (", stratum unknown\n");
//        }
//
//        strncpy(buff, ntpmsg.payload, sizeof(buff));
//        if ((dispstr = strstr (buff, DISP)) && (delaystr = strstr (buff, DELAY)))
//        {
//            dispstr += sizeof(DISP) - 1;
//            dispstr = strtok(dispstr,",");
//            delaystr += sizeof(DELAY) - 1;
//            delaystr = strtok(delaystr,",");
//
//            /* Check the resultant string is of a reasonable length */
//            if ((strlen (dispstr) == 0) || (strlen (dispstr) > 10) ||
//                (strlen (delaystr) == 0) || (strlen (delaystr) > 10))
//            {
//                printf ("accuracy unreadable\n");
//            }
//            else
//            {
//                printf("   time correct to within %.0f ms\n", atof(dispstr) + atof(delaystr) / 2.0);
//            }
//        }
//        else
//        {
//            rc=1;
//            printf ("accuracy unknown\n");
//        }
//    }
//
//
//    strncpy(buff, ntpmsg.payload, sizeof(buff));
//    if ((newstr = strstr (buff, POLL)))
//    {
//        newstr += sizeof(POLL) - 1;
//        dispstr = strtok(newstr,",");
//
//        /* Check the resultant string is of a reasonable length */
//        if ((strlen (dispstr) == 0) || (strlen (dispstr) > 2))
//        {
//            printf ("poll interval unreadable\n");
//        }
//        else
//        {
//            printf("   polling server every %d s\n",1 << atoi(dispstr));
//        }
//    }
//    else
//    {
//        rc=1;
//        printf ("poll interval unknown\n");
//    }
    }
    std::string sMessage = ntpmsg.payload;
    std::replace(sMessage.begin(), sMessage.end(), '\n', ' ');

    std::vector<std::string> vSplit = SplitString(sMessage, ',');

    for(size_t i = 0; i < vSplit.size(); i++)
    {

        std::vector<std::string> vKeyValue = SplitString(trim(vSplit[i]), '=');
        if(vKeyValue.size() == 2)
        {
            trim(vKeyValue[0]);
            trim(vKeyValue[1]);
            long long nValue;
            double dValue;
            if(ConvertToLongLong(vKeyValue[1], nValue))
            {
                jsValue[vKeyValue[0]] = nValue;
            }
            else if(ConvertToDouble(vKeyValue[1], dValue))
            {
                jsValue[vKeyValue[0]] = dValue;
            }
            else
            {
                jsValue[vKeyValue[0]] = vKeyValue[1];
            }
        }
    }

    return jsValue;
}

bool SysInfoManager::ConvertToDouble(const std::string& sValue, double& d)
{
    try
    {
        size_t nPos;
        d = std::stod(sValue, &nPos);
        if(nPos == sValue.size())
        {
            return true;
        }
    }
    catch(std::invalid_argument& e)
    {
    }
    return false;
}
bool SysInfoManager::ConvertToLongLong(const std::string& sValue, long long& n)
{
    try
    {
        size_t nPos;
        n = std::stoll(sValue, &nPos);
        if(nPos == sValue.size())
        {
            return true;
        }
    }
    catch(std::invalid_argument& e)
    {
    }
    return false;
}
