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

        Json::Value GetNtpStatus();

        void ExtractTicks(const std::string& sLine);

        bool ConvertToDouble(const std::string& sValue, double& d);
        bool ConvertToLongLong(const std::string& sValue, long long& n);

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

        //ntpstat stuff
        struct  ntp
        {               /* RFC-1305 NTP control message format */
            unsigned char byte1;  /* Version Number: bits 3 - 5; Mode: bits 0 - 2; */
            unsigned char byte2;  /* Response: bit 7;
                                     Error: bit 6;
                                     More: bit 5;
                                     Op code: bits 0 - 4 */
            unsigned short sequence;
            unsigned char  status1;  /* LI and clock source */
            unsigned char  status2;  /* count and code */
            unsigned short AssocID;
            unsigned short Offset;
            unsigned short Count;
            char payload[468];
            char authenticator[96];
        };

        static const int NTP_PORT=123;
        static const char B1VAL=0x16;   /* VN = 2, Mode = 6 */
        static const char B2VAL=2;
        static const char RMASK=0x80;  /* bit mask for the response bit in Status Byte2 */
        static const char EMASK=0x40;  /* bit mask for the error bit in Status Byte2 */
        static const char MMASK=0x20;  /* bit mask for the more bit in Status Byte2 */
        static const int PAYLOADSIZE=468; /* size in bytes of the message payload string */
        static const std::string clksrcname[10];
        static const std::string DISP;
        static const std::string DELAY;
        static const std::string STRATUM;
        static const std::string POLL;
        static const std::string REFID;
};
