#pragma once
#include <vector>
#include <list>
#include <string>
#include <chrono>
#include <tuple>


using range = std::tuple<unsigned long, unsigned long, bool>;

class CronJob
{
    public:
        CronJob(const std::string& sCron);

        CronJob();
        bool SetString(const std::string& sCron);

        bool JobNow(const std::chrono::time_point<std::chrono::system_clock>& tp);
        bool JobNow(const std::tm& now);

        enum {SECONDS = 0, MINUTES, HOURS, DOMS, MONTHS, DOWS, YEARS, GMT};

    private:

        struct cron_element
        {
            std::vector<bool> vBits;

        };

        bool ParseString(const std::string& sCron);
        bool ParseYears(const std::string& sYears);

        void FillVector(cron_element& element);
        bool GetList(const std::string& sElement, cron_element& element);

        bool MatchesYear(unsigned long nYear);

        range GetRange(const std::string& sRange);


        void OutputElements(const std::vector<bool>& vBits, size_t nSelected);

        std::vector<cron_element> m_vElement;
        std::list<range> m_listYears;
        bool m_bGMT;

        std::string m_sCron;
        bool m_bLast;
        bool m_bNext;

        enum {MIN=0,MAX,VALID};

};
