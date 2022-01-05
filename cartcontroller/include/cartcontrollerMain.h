/***************************************************************
 * Name:      controllerMain.h
 * Purpose:   Defines Application Frame
 * Author:    Matthew Martin (martim01@outlook.com)
 * Created:   2020-06-15
 * Copyright: Matthew Martin (https://github.com/martim01)
 * License:
 **************************************************************/

#ifndef CONTROLLERMAIN_H
#define CONTROLLERMAIN_H

#include "uirect.h"
#include <random>
#include "wxwebsocketclient.h"
//#include "restfulclient.h"
#include "json/json.h"
#include <wx/datetime.h>
#include <wx/panel.h>
#include <vector>
#include <map>


//(*Headers(controllerDialog)
#include <wx/dialog.h>
#include <wx/timer.h>
//*)

class pnlResource;
class wmLabel;
class wmButton;

class cartcontrollerDialog: public wxDialog
{
    public:

        cartcontrollerDialog(wxWindow* parent, const wxString& sIpAddress, unsigned short nPort, wxWindowID id = -1);
        virtual ~cartcontrollerDialog();




    private:

        //(*Handlers(controllerDialog)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        //*)

        void OntimerConnectionTrigger(wxTimerEvent& event);
        void OntimerHoldTrigger(wxTimerEvent& event);




        //(*Identifiers(controllerDialog)
        //*)

        static const long ID_TIMER_CONNECTION;
        static const long ID_TIMER_HOLD;
        static const long ID_TIMER_MENU;
        static const long ID_PANEL1;

        //(*Declarations(controllerDialog)
        //*)
        wxTimer m_timerConnection;

        wxTimer m_timerStop;
        wxTimer m_timerCheck;
        wxPanel* m_ppnlStatus;
        wmLabel* m_plblHostname;
        wmButton* m_pbtnSystem;
        wmButton* m_pbtnPrev;
        wmButton* m_pbtnNext;
        wmLabel* m_plblPage;

        void OnWebsocketConnection(const wxCommandEvent& event);
        void OnWebsocketFrame(const wxCommandEvent& event);
        void OnTimerTimeout(const wxTimerEvent& event);

        void OnResourcePlay(const wxCommandEvent& event);
        void OnResourceMenu(const wxCommandEvent& event);
        void OnResourceStop(const wxCommandEvent& event);

        void OnbtnSystem(const wxCommandEvent& event);
        void OnbtnPrevious(const wxCommandEvent& event);
        void OnbtnNext(const wxCommandEvent& event);

        void OnTimerCheck(const wxTimerEvent& event);
        void UpdateLabels();

        void ReplyConfig(const Json::Value& jsData);
        void ReplyFiles(const Json::Value& jsData);
        void UpdatePlayingStatus(const Json::Value& jsData);

        void Play(const wxString& sUid);
        void Stop(bool bKill=false);

        void UpdateResources(Json::Value& jsResources);
        void AddResource(const wxString& sUid);
        void DeleteResource(const wxString& sUid);
        void ModifyResource(const wxString& sUid);
        void ReplyAddFile(const Json::Value& jsData);
        void ReplyModifyFile(const Json::Value& jsData);

        void UpdatePanel();

        uiRect m_uiName;
        uiRect m_uiStatus;


        wxWebSocketClient m_wsClient;
        //RestfulClient m_rClient;

        wxString m_sWSEndpoint;
        wxString m_sUrl;
        wxString m_sIpAddress;

        std::string m_sDefaultFileUid;
        wxString m_sDefaultFileLabel;

        wxTimeSpan m_tsLength;
        wxTimeSpan m_tsPosition;

        enum {DISCONNECTED, CONNECTING, CONNECTED};
        int m_nConnected;

        bool m_bCountUp;

        bool m_bIgnoreUp;
        bool m_bDown;

        wxTimer m_timerTimeout;

        size_t m_nPageOffset;

        std::random_device m_rd;
        std::mt19937 m_gen;
        std::uniform_int_distribution<> m_dist;

        std::vector<pnlResource*> m_vResourcePanels;

        std::map<std::string, std::string> m_mFiles;

        DECLARE_EVENT_TABLE()

        static const wxColour CLR_PLAYING;
        static const wxColour CLR_IDLE;
        static const wxColour CLR_CONNECTING;
        static const wxColour CLR_ERROR;
        static const wxColour CLR_NO_FILE;
        static const wxColour CLR_OKAY;


};

#endif // CONTROLLERMAIN_H
