#include "wxcron.h"
#include <wx/tokenzr.h>
#include <wx/arrstr.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
using namespace std;


wxCronJob::wxCronJob(const wxString& sCron) : m_bGMT(false), m_bLast(true), m_bNext(true)
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

wxCronJob::wxCronJob() : m_bGMT(false), m_bLast(true), m_bNext(true)
{
    m_element.resize(6);
    m_element[SECONDS].vBits.resize(60);
    m_element[MINUTES].vBits.resize(60);
    m_element[HOURS].vBits.resize(24);
    m_element[DOMS].vBits.resize(35);
    m_element[MONTHS].vBits.resize(13);
    m_element[DOWS].vBits.resize(8);
}

void wxCronJob::SetString(const wxString& sCron)
{
    ParseString(sCron);
}

void wxCronJob::ParseString(const wxString& sCron)
{
    m_sCron = sCron;
    wxArrayString asCron(wxStringTokenize(sCron, wxT(" ")));

    //make sure we have a cron whatever
    for(size_t i = SECONDS; i < YEARS; i++)
    {
        if(asCron.GetCount() == i)
        {
            asCron.Add(wxT("*"));
        }
    }

    if(asCron.size() == YEARS)
    {
        asCron.Add(wxT("l"));
    }

    wxString sDebug;
    for(size_t i = 0; i < asCron.GetCount(); i++)
    {
        sDebug << asCron[i] << wxT(" ");
    }

    if(asCron.size() > YEARS)
    {
        for(size_t nElement = SECONDS; nElement < YEARS; nElement++)
        {
            if(asCron[nElement] == wxT("*") || asCron[nElement] == wxT("?"))
            {
                FillVector(m_element[nElement]);
            }
            else
            {
                GetList(asCron[nElement], m_element[nElement]);
            }
        }

        ParseYears(asCron[YEARS]);
    }
    m_bGMT = (asCron.size() > GMT && asCron[GMT] == wxT("Z"));

    #ifndef NDEBUG
    for(size_t i = 0; i < m_element.size(); i++)
    {
        wxString sLog;
        for(size_t j = 0; j < m_element[i].vBits.size(); j++)
        {
            if(i == DOMS && j == 31)
            {
                sLog << wxT("[");
            }
            sLog << m_element[i].vBits[j];
            if(i == DOMS && j == 31)
            {
                sLog << wxT("]");
            }
        }
    }
    #endif


}

void wxCronJob::FillVector(cron_element& element)
{
    for(size_t i = 0; i < element.vBits.size(); i++)
    {
        element.vBits[i] = true;
    }
    element.nFirst = 0;
}

void wxCronJob::GetList(const wxString& sElement, cron_element& element)
{
    wxArrayString asList(wxStringTokenize(sElement, wxT(",")));

    unsigned long nLower, nHigher;
    wxString sLower, sHigher;
    for(size_t i = 0; i < asList.GetCount(); i++)
    {
        sLower = (asList[i].BeforeFirst(wxT('-')));
        sHigher = (asList[i].AfterFirst(wxT('-')));
        if(sHigher.empty())
        {
            sHigher = sLower;
        }
        if(sLower.ToULong(&nLower) && sHigher.ToULong(&nHigher))
        {
            for(size_t i = min(min(nLower,nHigher), static_cast<unsigned long>(element.vBits.size())); i <= min(max(nLower,nHigher),  static_cast<unsigned long>(element.vBits.size())); i++)
            {
                element.vBits[i] = true;
            }
        }
    }
    element.nFirst = element.vBits.size();
    element.nSize = 0;
    for(size_t  i = 0; i < element.vBits.size(); i++)
    {
        if(element.vBits[i])
        {
            element.nFirst = i;
            element.nSize++;
            break;
        }
    }
}

void wxCronJob::ParseYears(const wxString& sYears)
{
    if(sYears == wxT("*") || sYears == wxT("?"))
    {
        m_listYears.push_back(make_pair(0, 9999));
    }
    else
    {
        wxArrayString asList(wxStringTokenize(sYears, wxT(",")));

        unsigned long nLower, nHigher;
        wxString sLower, sHigher;
        for(size_t i = 0; i < asList.GetCount(); i++)
        {
            sLower = (asList[i].BeforeFirst(wxT('-')));
            sHigher = (asList[i].AfterFirst(wxT('-')));
            if(sHigher.empty())
            {
                sHigher = sLower;
            }
            if(sLower.ToULong(&nLower) && sHigher.ToULong(&nHigher))
            {
                m_listYears.push_back(make_pair(min(nLower, nHigher), max(nLower,nHigher)));
            }
        }
    }
}



wxDateTime wxCronJob::GetNextScheduled(wxDateTime dt)
{
    if(m_bNext)
    {

        if(m_bGMT)
        {
            dt = dt.MakeUTC(false);
        }
        wxDateTime dtNow(wxDateTime::Now());

        if(m_listYears.size() == 1 && m_listYears.front().first == m_listYears.front().second)
        {
            dtNow = wxDateTime(1,wxDateTime::Jan,m_listYears.front().first);
            if(m_listYears.front().second < wxDateTime::Now().GetYear())
            {
                return wxDateTime(time_t(0xFFFFFFFF));
            }
        }
        if(m_element[MONTHS].nSize == 1)
        {
            dtNow.SetDay(1);
            dtNow.SetMonth(wxDateTime::Month(m_element[MONTHS].nFirst-1));
            if(m_element[DOMS].nSize == 1)
            {
                dtNow.SetDay(m_element[DOMS].nFirst);
            }
            if(wxDateTime::Month(m_element[MONTHS].nFirst-1) < wxDateTime::Now().GetMonth() ||
               (wxDateTime::Month(m_element[MONTHS].nFirst-1) == wxDateTime::Now().GetMonth() && dtNow.GetDay() < wxDateTime::Now().GetDay()))
            {
                return wxDateTime(time_t(0xFFFFFFFF));
            }
        }


        unsigned int nCount(0);
        while(!JobNow(dt) && nCount < 10000)
        {
            if(m_element[SECONDS].nFirst != dt.GetSecond())
            {
                dt += wxTimeSpan(0,0,1,0);
            }
            else if(m_element[MINUTES].nFirst != dt.GetMinute())
            {
                dt += wxTimeSpan(0,1,0,0);
            }
            else
            {
                dt += wxTimeSpan(1,0,0,0);
            }

            if((dt-dtNow).GetDays() > 1600) //if over 5 years in advand then probably wrong
            {
                return wxDateTime(time_t(0xFFFFFFFF));
            }
            nCount++;
        }

        if(nCount == 1000)
        {
            m_bNext = false;
            return wxDateTime(time_t(0xFFFFFFFF));

        }


        if(m_bGMT && dt.IsDST())
        {
            dt += wxTimeSpan(1,0,0,0);
        }
        return dt;
    }
    return wxDateTime(time_t(0xFFFFFFFF));
}

wxDateTime wxCronJob::GetLastScheduled(wxDateTime dt)
{
    if(m_bLast)
    {
        if(m_bGMT)
        {
            dt = dt.MakeUTC(false);
        }
        unsigned int nCount(0);
        while(!JobNow(dt) && nCount < 1000)
        {

            if(m_element[SECONDS].nFirst != dt.GetSecond())
            {
                dt -= wxTimeSpan(0,0,1,0);
            }
            else if(m_element[MINUTES].nFirst != dt.GetMinute())
            {
                dt -= wxTimeSpan(0,1,0,0);
            }
            else
            {
                dt -= wxTimeSpan(1,0,0,0);
            }
            ++nCount;
        }

        if(nCount == 1000)
        {
            m_bLast = false;
            return wxDateTime(time_t(0));


        }

        if(m_bGMT && dt.IsDST())
        {
            dt += wxTimeSpan(1,0,0,0);
        }
        return dt;
    }
    return wxDateTime(time_t(0));
}


bool wxCronJob::JobNow(const wxDateTime& dt)
{


    if(m_element[SECONDS].vBits[dt.GetSecond()] &&
       m_element[MINUTES].vBits[dt.GetMinute()] &&
       m_element[HOURS].vBits[dt.GetHour()] &&
       m_element[DOMS].vBits[dt.GetDay()] &&
       m_element[MONTHS].vBits[static_cast<unsigned long>(dt.GetMonth())+1] &&
       m_element[DOWS].vBits[static_cast<unsigned long>(dt.GetWeekDay())] &&
       MatchesYear(dt))
    {
        return true;
    }
    return false;
}


bool wxCronJob::MatchesYear(const wxDateTime& dt)
{
    for(list<pair<unsigned long, unsigned long> >::iterator itRange = m_listYears.begin(); itRange != m_listYears.end(); ++itRange)
    {
        if((*itRange).first <= dt.GetYear() && (*itRange).second >= dt.GetYear())
        {
            return true;
        }
    }
    return false;
}
