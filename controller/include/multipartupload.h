#pragma once
#include <wx/event.h>
#include <atomic>
#include <queue>
#include <string>
#include <wx/timer.h>

struct mg_mgr;
struct mg_connection;
struct http_message;

using dataType = std::pair<std::string, bool>;

class MultipartUpload : public wxEvtHandler
{
    public:
        MultipartUpload(wxEvtHandler* pHandler);
        bool Upload(const std::string& sEndpoint, const std::map<dataType,std::string>& mData, int nId);

        /** Handles an event
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void HandleEvent(mg_connection *pConnection, int nEvent, void* pData);


    private:

        void ConnectionEvent(int nStatus);

        wxEvtHandler* m_pHandler;
        mg_mgr* m_pManager;

        std::atomic<bool> m_bLoop;

        struct task
        {
            std::string sEndpoint;
            std::map<dataType,std::string> mData;
        };

        std::queue<task> m_qTasks;
        wxTimer m_timerTask;

};


