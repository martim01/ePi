#include "wxhttpclientmanager.h"
#include "httpclient.h"
#include "log.h"


using namespace std::placeholders;


wxDEFINE_EVENT(wxEVT_R_PROGRESS, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_R_REPLY, wxCommandEvent);


wxHttpClientManager::wxHttpClientManager(wxEvtHandler* pHandler) : m_pHandler(pHandler)
{

}

void wxHttpClientManager::Progress(unsigned long nSent, unsigned long nTotal)
{
    if(m_pHandler)
    {
        auto pEvent = new wxCommandEvent(wxEVT_R_PROGRESS);
        pEvent->SetInt(nSent);
        pEvent->SetExtraLong(nTotal);
        wxQueueEvent(m_pHandler, pEvent);
    }
}

void wxHttpClientManager::Reply(const pml::restgoose::clientResponse& resp, unsigned int nRunId)
{
    pmlLog(pml::LOG_TRACE) << "wxHttpClientManager::Reply " << nRunId << " code: " << resp.nCode;
    auto itId = m_mIds.find(nRunId);
    if(itId != m_mIds.end())
    {
        if(m_pHandler)
        {
            auto pEvent = new wxCommandEvent(wxEVT_R_REPLY);
            pEvent->SetInt(resp.nCode);
            pEvent->SetExtraLong(itId->second);
            pEvent->SetString(resp.data.Get());
            wxQueueEvent(m_pHandler, pEvent);
        }
        m_mIds.erase(nRunId);
        m_mClients.erase(nRunId);
    }
    pmlLog(pml::LOG_TRACE) << "wxHttpClientManager::Reply Done";
}

void wxHttpClientManager::Run(std::unique_ptr<pml::restgoose::HttpClient> pClient, unsigned int nId)
{
    int nInternalId = wxNewId();
    auto itClient = m_mClients.insert(std::make_pair(nInternalId, std::move(pClient))).first;
    m_mIds.insert({nInternalId, nId});

    itClient->second->SetUploadProgressCallback(std::bind(&wxHttpClientManager::Progress, this, _1,_2));
    itClient->second->Run(std::bind(&wxHttpClientManager::Reply, this, _1, _2), nInternalId);
}

void wxHttpClientManager::Cancel(unsigned int nId)
{
    for(auto pairId : m_mIds)
    {
        if(pairId.second == nId)
        {
            auto itClient = m_mClients.find(pairId.first);
            if(itClient != m_mClients.end())
            {
                itClient->second->Cancel();
                m_mClients.erase(pairId.first);
            }
            m_mIds.erase(pairId.first);
            break;
        }
    }
}
