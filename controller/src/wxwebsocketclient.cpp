#include "wxwebsocketclient.h"
#include "log.h"

using namespace std::placeholders;

wxDEFINE_EVENT(wxEVT_WS_CONNECTION, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_WS_MESSAGE, wxCommandEvent);

wxWebSocketClient::wxWebSocketClient() :
    m_client(std::bind(&wxWebSocketClient::ConnectionCallback, this, _1, _2), std::bind(&wxWebSocketClient::MessageCallback, this, _1,_2))
{

}

void wxWebSocketClient::AddHandler(wxEvtHandler* pHandler)
{
    m_setHandlers.insert(pHandler);
}

void wxWebSocketClient::RemoveHandler(wxEvtHandler* pHandler)
{
    m_setHandlers.erase(pHandler);
}


void wxWebSocketClient::Run()
{
    m_client.Run();
}

void wxWebSocketClient::Stop()
{
    m_client.Stop();
}

bool wxWebSocketClient::Connect(const endpoint& theEndpoint)
{
    return m_client.Connect(theEndpoint);
}

bool wxWebSocketClient::ConnectionCallback(const endpoint& theEndpoint, bool bConnected)
{
    int nId = wxNewId();
    m_mId.insert({theEndpoint, nId});
    for(auto pHandler : m_setHandlers)
    {
        auto pEvent = new wxCommandEvent(wxEVT_WS_CONNECTION);
        pEvent->SetString(theEndpoint.Get());
        pEvent->SetInt(nId);
        pEvent->SetExtraLong(bConnected);
        wxQueueEvent(pHandler, pEvent);
    }
    return true;
}

bool wxWebSocketClient::MessageCallback(const endpoint& theEndpoint, const std::string& sMessage)
{

    auto itId = m_mId.find(theEndpoint);
    if(itId != m_mId.end())
    {
        for(auto pHandler : m_setHandlers)
        {
            auto pEvent = new wxCommandEvent(wxEVT_WS_MESSAGE);
            pEvent->SetString(sMessage);
            pEvent->SetId(itId->second);
            wxQueueEvent(pHandler, pEvent);
        }
    }
    return true;
}
