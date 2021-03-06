#include "restfulclient.h"
#include <thread>
#include <wx/log.h>
#include "mongoose.h"
#include <iostream>

struct mg_connection *mg_connect_http_opt(
    struct mg_mgr *mgr, MG_CB(mg_event_handler_t ev_handler, void *user_data),
    struct mg_connect_opts opts, const char *url, const char *extra_headers,
    const char *post_data, const char* method)
{
  struct mg_str user = MG_NULL_STR, null_str = MG_NULL_STR;
  struct mg_str host = MG_NULL_STR, path = MG_NULL_STR;
  struct mbuf auth;
  struct mg_connection *nc = mg_connect_http_base(mgr, MG_CB(ev_handler, user_data), opts, "http", NULL, "https", NULL, url, &path, &user, &host);

  if (nc == NULL)
  {
    return NULL;
  }

  mbuf_init(&auth, 0);
  if (user.len > 0)
  {
    mg_basic_auth_header(user, null_str, &auth);
  }

  if (post_data == NULL) post_data = "";
  if (extra_headers == NULL) extra_headers = "";
  if (path.len == 0) path = mg_mk_str("/");
  if (host.len == 0) host = mg_mk_str("");

  mg_printf(nc,
            "%s %.*s HTTP/1.1\r\nHost: %.*s\r\nContent-Length: %" SIZE_T_FMT
            "\r\n%.*s%s\r\n%s",
            method, (int) path.len, path.p,
            (int) (path.p - host.p), host.p, strlen(post_data), (int) auth.len,
            (auth.buf == NULL ? "" : auth.buf), extra_headers, post_data);

  mbuf_free(&auth);
  return nc;
}

struct mg_connection *mg_connect_http(
    struct mg_mgr *mgr, MG_CB(mg_event_handler_t ev_handler, void *user_data),
    const char *url, const char *extra_headers, const char *post_data, const char* method)
{
  struct mg_connect_opts opts;
  memset(&opts, 0, sizeof(opts));
  return mg_connect_http_opt(mgr, MG_CB(ev_handler, user_data), opts, url,
                             extra_headers, post_data, method);
}


const std::string RestfulClient::METHOD[5] = {"GET", "POST", "PATCH", "PUT", "DELETE"};

wxDEFINE_EVENT(wxEVT_R_CONNECTION, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_R_REPLY, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_R_FINISHED, wxCommandEvent);

static void restful_handler(mg_connection* pConnection, int nEvent, void* pData)
{
    RestfulClient* pThread = reinterpret_cast<RestfulClient*>(pConnection->user_data);
    pThread->HandleEvent(pConnection, nEvent, pData);
}

RestfulClient::RestfulClient(wxEvtHandler* pHandler) : m_pHandler(pHandler),
m_bLoop(false),
m_bRunning(false),
m_pThread(nullptr)
{
    m_pManager = new mg_mgr;

    mg_mgr_init(m_pManager, nullptr);

    Connect(wxID_ANY, wxEVT_R_CONNECTION, (wxObjectEventFunction)&RestfulClient::OnConnectionEvent);
    Connect(wxID_ANY, wxEVT_R_FINISHED, (wxObjectEventFunction)&RestfulClient::OnFinishedEvent);

    m_timerTask.SetOwner(this, wxNewId());
    Connect(m_timerTask.GetId(), wxEVT_TIMER, (wxObjectEventFunction)&RestfulClient::OnTimerTask);
}

RestfulClient::~RestfulClient()
{
    m_bLoop = false;
    if(m_pThread)
    {
        m_pThread->join();
    }
    mg_mgr_free(m_pManager);


}

bool RestfulClient::Get(const std::string& sEndpoint, int nUserId)
{
    m_qTasks.push({sEndpoint, nullptr, METHOD_GET, nUserId});

    if(m_qTasks.size() == 1)
    {
        DoNextTask();
    }
    return true;
}

bool RestfulClient::Patch(const std::string& sEndpoint, const char* pPostData, int nUserId)
{
    m_qTasks.push({sEndpoint, pPostData, METHOD_PATCH, nUserId});

    if(m_qTasks.size() == 1)
    {
        DoNextTask();
    }
    return true;
}

bool RestfulClient::Post(const std::string& sEndpoint, const char* pPostData, int nUserId)
{
    m_qTasks.push({sEndpoint, pPostData, METHOD_POST, nUserId});

    if(m_qTasks.size() == 1)
    {
        DoNextTask();
    }
    return true;
}

bool RestfulClient::Put(const std::string& sEndpoint, const char* pPostData, int nUserId)
{
    m_qTasks.push({sEndpoint, pPostData, METHOD_PUT, nUserId});

    if(m_qTasks.size() == 1)
    {
        DoNextTask();
    }
    return true;
}

bool RestfulClient::Delete(const std::string& sEndpoint,int nUserId)
{
    m_qTasks.push({sEndpoint, nullptr, METHOD_DELETE, nUserId});

    if(m_qTasks.size() == 1)
    {
        DoNextTask();
    }
    return true;
}


void RestfulClient::DoNextTask()
{
    if(!m_bRunning)
    {
        if(m_pThread)   //thread has finished but is waiting for us to join and let it exit
        {
            m_pThread->join();
            m_pThread = nullptr;
        }

        mg_connection* pConnection = mg_connect_http(m_pManager, restful_handler, m_qTasks.front().sEndpoint.c_str(), nullptr,m_qTasks.front().pPostData, METHOD[m_qTasks.front().nMethod].c_str());
        if(pConnection != nullptr)
        {
            m_bRunning = true;
            mg_set_protocol_http_websocket(pConnection);
            pConnection->user_data = reinterpret_cast<void*>(this);
            m_bLoop = true;

            m_pThread = std::make_unique<std::thread>([this](){
                while(m_bLoop)
                {
                    mg_mgr_poll(m_pManager, 100);
                }
                m_bRunning = false;

            });
        }
    }
}

void RestfulClient::HandleEvent(mg_connection *pConnection, int nEvent, void* pData)
{
    std::cout << "RestfulClient::HandleEvent: " << nEvent << std::endl;
    switch(nEvent)
    {
        case MG_EV_CONNECT:
            ConnectionEvent(*reinterpret_cast<int*>(pData));
            break;
        case MG_EV_RECV:
            std::cout << std::string(pConnection->recv_mbuf.buf, pConnection->recv_mbuf.len) << std::endl;
            break;
        case MG_EV_HTTP_REPLY:
            ReplyEvent(reinterpret_cast<http_message*>(pData));
            break;
        case MG_EV_CLOSE:
            CloseEvent();
            break;
    }
}


void RestfulClient::ConnectionEvent(int nStatus)
{

    if(m_pHandler)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_R_CONNECTION);
        pEvent->SetInt(nStatus);
        wxQueueEvent(this, pEvent);
    }
}

void RestfulClient::ReplyEvent(http_message* pMessage)
{

    std::cout << "RestfulClient::ReplyEvent" << std::endl;
    if(m_pHandler)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_R_REPLY);
        pEvent->SetString(wxString::FromUTF8(pMessage->body.p, pMessage->body.len));
        pEvent->SetInt(m_qTasks.front().nId);
        wxQueueEvent(m_pHandler, pEvent);
    }
}

void RestfulClient::CloseEvent()
{
    if(m_pHandler)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_R_FINISHED);
        wxQueueEvent(this, pEvent);
    }
    m_bLoop = false;
}

void RestfulClient::OnConnectionEvent(wxCommandEvent& event)
{
    if(event.GetInt() != 0)
    {
        wxLogDebug("Connection failed");
    }
}

void RestfulClient::OnFinishedEvent(wxCommandEvent& event)
{
    m_qTasks.pop();
    m_timerTask.Start(250, true);
}


void RestfulClient::OnTimerTask(const wxTimerEvent& event)
{
    if(m_qTasks.empty() == false)
    {
        DoNextTask();
    }
}
