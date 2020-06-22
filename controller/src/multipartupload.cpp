#include "multipartupload.h"
#include <thread>
#include <wx/log.h>
#include "mongoose.h"

static void upload_handler(mg_connection* pConnection, int nEvent, void* pData)
{
    MultipartUpload* pThread = reinterpret_cast<MultipartUpload*>(pConnection->user_data);
    pThread->HandleEvent(pConnection, nEvent, pData);
}

MultipartUpload::MultipartUpload(wxEvtHandler* pHandler) : m_pHandler(pHandler),
m_bLoop(false)
{
    m_timerTask.SetOwner(this, wxNewId());
    Connect(m_timerTask.GetId(), wxEVT_TIMER, (wxObjectEventFunction)&MultipartUpload::OnTimerTask);
}


bool MultipartUpload::Upload(const std::string& sEndpoint, const std::map<dataType,std::string>& mData, int nId)
{
    m_qTasks.push({sEndpoint, mData, nId});

    if(m_qTasks.size() == 1)
    {
        DoNextTask();
    }
    return true;
}


void MultipartUpload::DoNextTask()
{
    wxLogDebug("DoNextTask = '%s'", wxString::FromUTF8(m_qTasks.front().sEndpoint.c_str()).c_str());

    m_pManager = new mg_mgr;

    mg_mgr_init(m_pManager, nullptr);
    mg_connection* pConnection = mg_connect(m_pManager, m_qTasks.front().sEndpoint.c_str(), upload_handler);
    if(pConnection != nullptr)
    {
        pConnection->user_data = reinterpret_cast<void*>(this);
        m_bLoop = true;

        std::thread th([this](){
            while(m_bLoop)
            {
                mg_mgr_poll(m_pManager, 100);
            }
            mg_mgr_free(m_pManager);
        });
        th.detach();
    }
}

void MultipartUpload::HandleEvent(mg_connection *pConnection, int nEvent, void* pData)
{
    wxLogDebug("MultipartUpload::Event: #%d", nEvent);
    switch(nEvent)
    {
        case MG_EV_CONNECT:
            ConnectionEvent(*reinterpret_cast<int*>(pData));
            break;
        case MG_EV_HTTP_REPLY:
            ReplyEvent(reinterpret_cast<http_message*>(pData));
            break;
        case MG_EV_CLOSE:
            CloseEvent();
            break;
    }
}

void MultipartUpload::ConnectionEvent(int nStatus)
{
    if(nStatus != 0)
    {
        //@todo(martim01) failed to connect
    }
    else
    {

    }
}


void MultipartUpload::OnTimerTask(const wxTimerEvent& event)
{
    if(m_qTasks.empty() == false)
    {
        DoNextTask();
    }
}


