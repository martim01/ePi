#include "websocketclient.h"
#include <thread>
#include <wx/log.h>
#include "mongoose.h"

wxDEFINE_EVENT(wxEVT_WS_CONNECTION, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_WS_HANDSHAKE, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_WS_FRAME, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_WS_FINISHED, wxCommandEvent);

static void ev_handler(mg_connection* pConnection, int nEvent, void* pData)
{
    WebSocketClient* pThread = reinterpret_cast<WebSocketClient*>(pConnection->user_data);
    pThread->HandleEvent(pConnection, nEvent, pData);
}

WebSocketClient::WebSocketClient(wxEvtHandler* pHandler) : m_pHandler(pHandler),
m_pConnection(nullptr),
m_bLoop(false)
{

}


bool WebSocketClient::Connect(const std::string& sEndpoint)
{
    if(m_bLoop)
        return false;

    m_pManager = new mg_mgr;

    mg_mgr_init(m_pManager, nullptr);
    m_pConnection = mg_connect_ws(m_pManager, ev_handler, sEndpoint.c_str(), nullptr, nullptr);
    if(m_pConnection == nullptr)
    {
        return false;
    }

    m_pConnection->user_data = reinterpret_cast<void*>(this);
    m_bLoop = true;


    std::thread th([this](){
        while(m_bLoop)
        {
            mg_mgr_poll(m_pManager, 100);
        }
        mg_mgr_free(m_pManager);
    });
    th.detach();

    return true;
}

void WebSocketClient::HandleEvent(mg_connection *pConnection, int nEvent, void* pData)
{
    switch(nEvent)
    {
        case MG_EV_CONNECT:
            ConnectionEvent(*reinterpret_cast<int*>(pData));
            break;
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
            HandshakeDone(reinterpret_cast<http_message*>(pData));
            break;
        case MG_EV_WEBSOCKET_FRAME:
            FrameReceived(reinterpret_cast<websocket_message*>(pData));
            break;
        case MG_EV_HTTP_REPLY:
            {
                http_message* pMessage = reinterpret_cast<http_message*>(pData);
                wxLogDebug(wxString::FromUTF8(pMessage->message.p, pMessage->message.len));
            }
            break;
        case MG_EV_CLOSE:
            CloseEvent();
            break;
    }
}


void WebSocketClient::ConnectionEvent(int nStatus)
{
    if(m_pHandler)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_WS_CONNECTION);
        pEvent->SetInt(nStatus);
        wxQueueEvent(m_pHandler, pEvent);
    }
}

void WebSocketClient::HandshakeDone(http_message* pMessage)
{
    if(m_pHandler)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_WS_HANDSHAKE);
        pEvent->SetInt(pMessage->resp_code);
        wxQueueEvent(m_pHandler, pEvent);
    }
}

void WebSocketClient::FrameReceived(websocket_message* pMessage)
{
    if(m_pHandler)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_WS_FRAME);
        pEvent->SetString(wxString::FromUTF8(reinterpret_cast<char*>(pMessage->data), pMessage->size));
        wxQueueEvent(m_pHandler, pEvent);
    }
}

void WebSocketClient::CloseEvent()
{
    if(m_pHandler)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_WS_FINISHED);
        wxQueueEvent(m_pHandler, pEvent);
    }
    m_bLoop = false;
}
