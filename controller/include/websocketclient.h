#pragma once
#include <wx/event.h>
#include <atomic>
#include <set>
#include <mutex>
#include <thread>
#include <memory>

struct mg_mgr;
struct mg_connection;
struct http_message;
struct websocket_message;

class WebSocketClient
{
    public:
        WebSocketClient();
        ~WebSocketClient();
        bool Connect(const std::string& sEndpoint);

        /** Handles an event
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void HandleEvent(mg_connection *pConnection, int nEvent, void* pData);

        void AddHandler(wxEvtHandler* pHandler);
        void RemoveHandler(wxEvtHandler* pHandler);

        void Stop();

    private:

        void ConnectionEvent(int nStatus);
        void HandshakeDone(http_message* pMessage);
        void FrameReceived(websocket_message* pMessage);
        void CloseEvent();

        std::set<wxEvtHandler*> m_setHandlers;

        mg_mgr* m_pManager;
        mg_connection* m_pConnection;
        std::atomic<bool> m_bLoop;
        std::mutex m_mutex;
        std::unique_ptr<std::thread> m_pThread;
};


wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_CONNECTION, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_HANDSHAKE, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_FRAME, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_WS_FINISHED, wxCommandEvent);
