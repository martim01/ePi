#pragma once

#include <wx/string.h>
#include <wx/datetime.h>
#include <vector>
#include <list>
#include <wx/arrstr.h>

class wxCronJob
{
    public:
        wxCronJob(const wxString& sCron);

        wxCronJob();
        void SetString(const wxString& sCron);



        wxDateTime GetNextScheduled(wxDateTime dt);
        wxDateTime GetLastScheduled(wxDateTime dt);

        bool JobNow(const wxDateTime& dt);

        static const unsigned char SECONDS = 0;
        static const unsigned char MINUTES = 1;
        static const unsigned char HOURS = 2;
        static const unsigned char DOMS = 3;
        static const unsigned char MONTHS = 4;
        static const unsigned char DOWS = 5;
        static const unsigned char YEARS = 6;
        static const unsigned char GMT = 7;

    private:

        struct cron_element
        {
            std::vector<bool> vBits;
            size_t nFirst;
            size_t nSize;

        };

        void ParseString(const wxString& sCron);
        void ParseYears(const wxString& sYears);

        void FillVector(cron_element& element);
        void GetList(const wxString& sElement, cron_element& element);

        bool MatchesYear(const wxDateTime& dt);



        std::vector<cron_element> m_element;
        std::list<std::pair<unsigned long, unsigned long> > m_listYears;
        bool m_bGMT;

        wxString m_sCron;
        bool m_bLast;
        bool m_bNext;


};
