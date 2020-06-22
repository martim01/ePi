#pragma once
#include <wx/event.h>
#include <atomic>
#include <queue>
#include <string>
#include <wx/timer.h>
#include "namedtype.h"
#include <map>
#include <fstream>


struct mg_mgr;
struct mg_connection;
struct http_message;

using muFileName = NamedType<std::string, struct muFileNameParameter>;
using muFilePath = NamedType<std::string, struct muFilePathParameter>;
using muFile = std::pair<muFileName, muFilePath>;


class MultipartUpload : public wxEvtHandler
{
    public:
        MultipartUpload(wxEvtHandler* pHandler);
        bool Post(const std::string& sIpAddress, const std::string& sEndpoint,  const std::map<std::string,std::string>& mData, const std::map<std::string, muFile> mFiles, int nId);
        bool Put(const std::string& sIpAddress, const std::string& sEndpoint,  const std::map<std::string,std::string>& mData, const std::map<std::string, muFile> mFiles, int nId);

        void Cancel();
        /** Handles an event
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void HandleEvent(mg_connection *pConnection, int nEvent, void* pData);


    private:

        static const std::string UPLOAD[3];
        enum {UPLOAD_POST, UPLOAD_PATCH, UPLOAD_PUT};

        void ConnectionEvent(mg_connection* pConnection, int nStatus);
        void OnTimerTask(const wxTimerEvent& event);

        std::string GetData();
        std::map<std::string, std::string> GetFileBody();
        size_t GetTotalFileLength();

        void DoNextTask();

        void InitSendFile(mg_connection* pConnection);
        void SendSomeData(mg_connection* pConnection);

        void DataReceived(const wxString& sData);

        void OnFinishedEvent(const wxCommandEvent& event);

        std::ifstream m_ifs;
        double m_dLength;
        double m_dRead;
        int m_nProgress;


        std::map<std::string, std::string> m_mFileBody;
        std::map<std::string, std::string>::iterator m_itFile;


        wxEvtHandler* m_pHandler;
        mg_mgr* m_pManager;

        std::atomic<bool> m_bLoop;

        struct task
        {
            task(const std::string& sH, const std::string& sE, int nM, const std::map<std::string,std::string>& mD, const std::map<std::string,muFile>& mF, int nI) :
                sHost(sH), sEndpoint(sE), nMethod(nM), mData(mD), mFiles(mF), nId(nI){}
            std::string sHost;
            std::string sEndpoint;
            int nMethod;
            std::map<std::string,std::string> mData;
            std::map<std::string,muFile> mFiles;
            int nId;
        };

        bool m_bMore;


        std::queue<task> m_qTasks;
        wxTimer m_timerTask;

        static const std::string BOUNDARY;
        static const std::string CRLF;

};

wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_R_PROGRESS, wxCommandEvent);
