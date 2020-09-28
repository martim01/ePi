#pragma once
#include <wx/event.h>
#include <atomic>
#include <queue>
#include <string>
#include <wx/timer.h>

struct mg_mgr;
struct mg_connection;
struct http_message;

class RestfulClient : public wxEvtHandler
{
    public:
        RestfulClient(wxEvtHandler* pHandler);
        ~RestfulClient();
        bool Get(const std::string& sEndpoint, int nUserId);
        bool Post(const std::string& sEndpoint, const char* pPostData, int nUserId);
        bool Patch(const std::string& sEndpoint, const char* pPostData, int nUserId);
        bool Put(const std::string& sEndpoint, const char* pPostData, int nUserId);
        bool Delete(const std::string& sEndpoint, int nUserId);

        /** Handles an event
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void HandleEvent(mg_connection *pConnection, int nEvent, void* pData);

        enum {CONFIG, FILES, PLAYLISTS, SCHEDULES, STATUS, INFO, POWER, UPDATE, OUTPUTS};

    private:

        void OnConnectionEvent(wxCommandEvent& event);
        void OnFinishedEvent(wxCommandEvent& event);
        void DoNextTask();

        void OnTimerTask(const wxTimerEvent& event);

        void ConnectionEvent(int nStatus);
        void ReplyEvent(http_message* pMessage);
        void CloseEvent();

        wxEvtHandler* m_pHandler;
        mg_mgr* m_pManager;

        std::atomic<bool> m_bLoop;

        static const std::string METHOD[5];
        enum {METHOD_GET, METHOD_POST, METHOD_PATCH, METHOD_PUT, METHOD_DELETE};

        struct task
        {
            std::string sEndpoint;
            const char* pPostData;
            int nMethod;
            int nId;
        };

        std::queue<task> m_qTasks;
        wxTimer m_timerTask;

        bool m_bRunning;

};


wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_R_CONNECTION, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_R_REPLY, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_R_FINISHED, wxCommandEvent);
