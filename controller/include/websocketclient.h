#pragma once
#include <wx/event.h>
#include <atomic>


struct mg_mgr;
struct mg_connection;
struct http_message;
struct websocket_message;

class WebSocketClient
{
    public:
        WebSocketClient(wxEvtHandler* pHandler);
        bool Connect(const std::string& sEndpoint);

        /** Handles an event
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void HandleEvent(mg_connection *pConnection, int nEvent, void* pData);

    private:

        void ConnectionEvent(int nStatus);
        void HandshakeDone(http_message* pMessage);
        void FrameReceived(websocket_message* pMessage);
        void CloseEvent();

        wxEvtHandler* m_pHandler;
        mg_mgr* m_pManager;
        mg_connection* m_pConnection;
        std::atomic<bool> m_bLoop;
};


wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_CONNECTION, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_HANDSHAKE, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_FRAME, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_FINISHED, wxCommandEvent);
