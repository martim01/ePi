#pragma once
#include "json/json.h"
#include <mutex>
#include <atomic>

class NtpStatus
{
    public:
        NtpStatus();
        void Start();
        void Stop() { m_bRun = false; }
        const Json::Value& GetStatus();


    private:
        void Thread();

        bool Connect();
        bool Send();
        bool Receive();

        bool ConvertToDouble(const std::string& sValue, double& d);
        bool ConvertToLongLong(const std::string& sValue, long long& n);

        std::mutex m_mutex;
        std::atomic<bool> m_bRun;
        Json::Value m_jsStatus;

        int m_sd;

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

        void InterpretMessage(const ntp& ntpmsg);

        static const int NTP_PORT=123;
        static const char B1VAL=0x16;   /* VN = 2, Mode = 6 */
        static const char B2VAL=2;
        static const char RMASK=0x80;  /* bit mask for the response bit in Status Byte2 */
        static const char EMASK=0x40;  /* bit mask for the error bit in Status Byte2 */
        static const char MMASK=0x20;  /* bit mask for the more bit in Status Byte2 */
        static const int PAYLOADSIZE=468; /* size in bytes of the message payload string */
        static const std::string clksrcname[10];
};
