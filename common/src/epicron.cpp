#include "epicron.h"
#include "utils.h"
#include "log.h"

CronJob::CronJob(const std::string& sCron) : m_bGMT(false), m_bLast(true), m_bNext(true)
{
    m_element.resize(6);
    m_element[SECONDS].vBits.resize(60);
    m_element[MINUTES].vBits.resize(60);
    m_element[HOURS].vBits.resize(24);
    m_element[DOMS].vBits.resize(32);
    m_element[MONTHS].vBits.resize(13); //one greater as month string is 1-12
    m_element[DOWS].vBits.resize(7); //dows is 0=Sunday, 6 = Saturday


    ParseString(sCron);

}

CronJob::CronJob() : m_bGMT(false), m_bLast(true), m_bNext(true)
{
    m_element.resize(6);
    m_element[SECONDS].vBits.resize(60);
    m_element[MINUTES].vBits.resize(60);
    m_element[HOURS].vBits.resize(24);
    m_element[DOMS].vBits.resize(35);
    m_element[MONTHS].vBits.resize(13);
    m_element[DOWS].vBits.resize(8);
}

bool CronJob::SetString(const std::string& sCron)
{
    return ParseString(sCron);
}

bool CronJob::ParseString(const std::string& sCron)
{
    m_sCron = sCron;
    std::vector<std::string> vCron(SplitString(sCron, ' '));


    //make sure we have a cron whatever
    for(size_t i = SECONDS; i < YEARS; i++)
    {
        if(vCron.size() == i)
        {
            vCron.push_back("*");
        }
    }


    if(vCron.size() > YEARS)
    {
        for(size_t nElement = SECONDS; nElement < YEARS; nElement++)
        {
            if(vCron[nElement] == "*" || vCron[nElement] == "?")
            {
                FillVector(m_element[nElement]);
            }
            else
            {
                if(GetList(vCron[nElement], m_element[nElement]) == false)
                {
                    pml::Log::Get(pml::Log::LOG_ERROR) << nElement << ": " << vCron[nElement] << " is incorrect";
                    return false;
                }

            }
        }

        if(!ParseYears(vCron[YEARS]))
        {
            pml::Log::Get(pml::Log::LOG_ERROR) << "YEAR: " << vCron[YEARS] << " is incorrect";
            return false;
        }
    }
    m_bGMT = false;//(vCron.size() > GMT && vCron[GMT] == "Z");

    #ifndef NDEBUG
    for(size_t i = 0; i < m_element.size(); i++)
    {

        for(size_t j = 0; j < m_element[i].vBits.size(); j++)
        {
            if(i == DOMS && j == 31)
            {
                pml::Log::Get(pml::Log::LOG_DEBUG) << "[";
            }
            pml::Log::Get(pml::Log::LOG_DEBUG) << m_element[i].vBits[j];
            if(i == DOMS && j == 31)
            {
                pml::Log::Get(pml::Log::LOG_DEBUG) << "]";
            }
        }
    }
    pml::Log::Get(pml::Log::LOG_DEBUG) << std::endl;
    #endif

    return true;

}

void CronJob::FillVector(cron_element& element)
{
    for(size_t i = 0; i < element.vBits.size(); i++)
    {
        element.vBits[i] = true;
    }
    element.nFirst = 0;
}


range CronJob::GetRange(const std::string& sRange)
{
    unsigned long nFirst(0), nSecond(0);
    std::vector<std::string> vRange(SplitString(sRange,'-'));
    bool bOk(true);
    if(vRange.size() > 0)
    {
        try
        {
            nFirst = std::stoul(vRange[0]);
            if(vRange.size() > 1)
            {
                nSecond = std::stoul(vRange[1]);
            }
            else
            {
                nSecond = nFirst;
            }
        }
        catch(const std::invalid_argument& e)
        {
            bOk = false;
        }
    }
    return range(std::min(nFirst, nSecond), std::max(nFirst, nSecond), bOk);
}

bool CronJob::GetList(const std::string& sElement, cron_element& element)
{
    std::vector<std::string> vList(SplitString(sElement, ','));

    for(size_t i = 0; i < vList.size(); i++)
    {
        range rnge(GetRange(vList[i]));
        if(std::get<VALID>(rnge) == false)
        {
            return false;
        }

        for(size_t i = std::min(std::get<MIN>(rnge), static_cast<unsigned long>(element.vBits.size()));
            i <= std::min(std::get<MAX>(rnge),  static_cast<unsigned long>(element.vBits.size())); i++)
        {
            element.vBits[i] = true;
        }
    }

    return true;
}

bool CronJob::ParseYears(const std::string& sYears)
{
    if(sYears == "*" || sYears == "?")
    {
        m_listYears.push_back(range(0,9999,true));
    }
    else
    {
        std::vector<std::string> vList(SplitString(sYears, ','));

        for(size_t i = 0; i < vList.size(); i++)
        {
            range rnge(GetRange(vList[i]));
            if(std::get<VALID>(rnge) == false)
            {
                return false;
            }

            m_listYears.push_back(rnge);
        }
    }
    return true;
}


bool CronJob::JobNow(const std::chrono::time_point<std::chrono::system_clock>& tp)
{
    std::time_t t  = std::chrono::system_clock::to_time_t(tp);
    std::tm* pDate = std::localtime(&t);

    return JobNow(*pDate);
}

void CronJob::OutputElements(const std::vector<bool>& vBits, size_t nSelected)
{
    for(size_t i = 0; i < vBits.size(); i++)
    {
        if(i == nSelected)
        {
            std::cout << "[";
        }
        std::cout << vBits[i];
        if(i == nSelected)
        {
            std::cout << "]";
        }
    }
    std::cout << std::endl;
}

bool CronJob::JobNow(const std::tm& now)
{
    #ifdef TEST
    OutputElements(m_element[SECONDS].vBits, now.tm_sec);
    OutputElements(m_element[MINUTES].vBits, now.tm_min);
    OutputElements(m_element[HOURS].vBits, now.tm_hour);
    OutputElements(m_element[DOMS].vBits, now.tm_mday);
    OutputElements(m_element[MONTHS].vBits, now.tm_mon+1);
    OutputElements(m_element[DOWS].vBits, now.tm_wday);
    #endif


    if(m_element[SECONDS].vBits[now.tm_sec] &&
       m_element[MINUTES].vBits[now.tm_min] &&
       m_element[HOURS].vBits[now.tm_hour] &&
       m_element[DOMS].vBits[now.tm_mday] &&
       m_element[MONTHS].vBits[now.tm_mon+1] &&
       m_element[DOWS].vBits[now.tm_wday] &&
       MatchesYear(now.tm_year+900))
    {
        return true;
    }
    return false;
}


bool CronJob::MatchesYear(unsigned long nYear)
{
    for(auto itRange = m_listYears.begin(); itRange != m_listYears.end(); ++itRange)
    {
        if(std::get<MIN>(*itRange) <= nYear && std::get<MAX>(*itRange) >= nYear)
        {
            return true;
        }
    }
    return false;
}
