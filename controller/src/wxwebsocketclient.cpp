#include "wxwebsocketclient.h"
#include "log.h"
#include "json/json.h"

using namespace std::placeholders;

wxDEFINE_EVENT(wxEVT_WS_CONNECTION, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_WS_MESSAGE, wxCommandEvent);


wxWebSocketClient& wxWebSocketClient::Get()
{
    static wxWebSocketClient ws;
    return ws;
}

wxWebSocketClient::wxWebSocketClient() :
    m_client(std::bind(&wxWebSocketClient::ConnectionCallback, this, _1, _2), std::bind(&wxWebSocketClient::MessageCallback, this, _1,_2))
{
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "";
    m_pWriter = std::unique_ptr<Json::StreamWriter>(builder.newStreamWriter());
}

wxWebSocketClient::~wxWebSocketClient()
{
    Stop();
}

void wxWebSocketClient::Run()
{
    m_client.Run();
}

void wxWebSocketClient::Stop()
{
    m_client.Stop();
}

bool wxWebSocketClient::Connect(const endpoint& theEndpoint, wxEvtHandler* pHandler)
{
    if(m_client.Connect(theEndpoint))
    {
        m_mHandlers[theEndpoint].insert(pHandler);
        return true;
    }
    return false;
}

void wxWebSocketClient::AddHandler(const endpoint& theEndpoint, wxEvtHandler* pHandler)
{
    m_mHandlers[theEndpoint].insert(pHandler);
}

void wxWebSocketClient::RemoveHandler(const endpoint& theEndpoint, wxEvtHandler* pHandler)
{
    auto itHandlers = m_mHandlers.find(theEndpoint);
    if(itHandlers != m_mHandlers.end())
    {
        itHandlers->second.erase(pHandler);
    }
}

void wxWebSocketClient::RemoveHandler(wxEvtHandler* pHandler)
{
    for(auto& pairHandlers : m_mHandlers)
    {
        pairHandlers.second.erase(pHandler);
    }
}

bool wxWebSocketClient::ConnectionCallback(const endpoint& theEndpoint, bool bConnected)
{
    //pmlLog() << "wxWebSocketClient\tConnectionCallback: " << bConnected;

    auto itHandlers = m_mHandlers.find(theEndpoint);
    if(itHandlers != m_mHandlers.end())
    {
        for(auto pHandler : itHandlers->second)
        {
            auto pEvent = new wxCommandEvent(wxEVT_WS_CONNECTION);
            pEvent->SetString(theEndpoint.Get());
            pEvent->SetExtraLong(bConnected);
            wxQueueEvent(pHandler, pEvent);
        }
    }
    return true;
}

bool wxWebSocketClient::MessageCallback(const endpoint& theEndpoint, const std::string& sMessage)
{
    auto itHandlers = m_mHandlers.find(theEndpoint);
    if(itHandlers != m_mHandlers.end())
    {
        for(auto pHandler : itHandlers->second)
        {
            auto pEvent = new wxCommandEvent(wxEVT_WS_MESSAGE);
            pEvent->SetString(sMessage);
            wxQueueEvent(pHandler, pEvent);
        }
    }
    return true;
}


void wxWebSocketClient::SendAuthentication(const endpoint& theEndpoint, const wxString& sUser, const wxString& sPassword)
{
    Json::Value jsData;
    jsData["action"] = "_authentication";
    jsData["user"] = sUser.ToStdString();
    jsData["password"] = sPassword.ToStdString();

    std::stringstream ss;
    m_pWriter->write(jsData, &ss);

    m_client.Send(theEndpoint, ss.str());
}



void wxWebSocketClient::CloseConnection(const endpoint& theEndpoint)
{
    m_client.CloseConnection(theEndpoint);
}
