#pragma once
#include "websocketclient.h"
#include <wx/event.h>
#include <set>

class wxWebSocketClient
{
    public:
        wxWebSocketClient();

        void AddHandler(wxEvtHandler* pHandler);
        void RemoveHandler(wxEvtHandler* pHandler);

        void Run();
        bool Connect(const endpoint& theEndpoint);

        void Stop();



    private:
        bool ConnectionCallback(const endpoint& theEndpoint, bool bConnected);
        bool MessageCallback(const endpoint& theEndpoint, const std::string& sMessage);
        pml::restgoose::WebSocketClient m_client;

        std::set<wxEvtHandler*> m_setHandlers;

        std::map<endpoint, int> m_mId;

};

wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_CONNECTION, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_MESSAGE, wxCommandEvent);
