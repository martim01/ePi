#pragma once
#include "websocketclient.h"
#include <wx/event.h>
#include <set>

namespace Json
{
    class StreamWriter;
}

class wxWebSocketClient
{
    public:

        static wxWebSocketClient& Get();
        void Run();
        bool Connect(const endpoint& theEndpoint, wxEvtHandler* pHandler);
        void CloseConnection(const endpoint& theEndpoint);
        void Stop();

        void SendAuthentication(const endpoint& theEndpoint, const wxString& sUser, const wxString& sPassword);

    private:

        wxWebSocketClient();
        ~wxWebSocketClient();

        bool ConnectionCallback(const endpoint& theEndpoint, bool bConnected);
        bool MessageCallback(const endpoint& theEndpoint, const std::string& sMessage);
        pml::restgoose::WebSocketClient m_client;

        std::map<endpoint, wxEvtHandler*> m_mHandlers;

        std::unique_ptr<Json::StreamWriter> m_pWriter;

};

wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_CONNECTION, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_MESSAGE, wxCommandEvent);
