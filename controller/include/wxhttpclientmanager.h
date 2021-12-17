#pragma once
#include <wx/event.h>
#include "response.h"

namespace pml
{
    namespace restgoose
    {
        class HttpClient;
    }
}

class wxHttpClientManager
{
    public:
        wxHttpClientManager(wxEvtHandler* pHandler);

        void Run(std::unique_ptr<pml::restgoose::HttpClient> pClient, unsigned int nId);
        void Cancel(unsigned int nId);

    private:
        void Progress(unsigned long nSent, unsigned long nTotal);
        void Reply(const pml::restgoose::clientResponse& resp, unsigned int nRunId);

        wxEvtHandler* m_pHandler;
        std::map<unsigned int, std::unique_ptr<pml::restgoose::HttpClient>> m_mClients;
        std::map<unsigned int, unsigned int> m_mIds;
};

wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_R_PROGRESS, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_R_REPLY, wxCommandEvent);
