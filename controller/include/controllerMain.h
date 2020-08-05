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
#include "websocketclient.h"
#include "restfulclient.h"
#include "json/json.h"
#include <wx/datetime.h>
//(*Headers(controllerDialog)
#include <wx/dialog.h>
#include <wx/timer.h>
//*)
#include "uirect.h"
#include <random>

class controllerDialog: public wxDialog
{
    public:

        controllerDialog(wxWindow* parent, const wxPoint pntLayout, unsigned int nController, const wxString& sIpAddress, unsigned short nPort, wxWindowID id = -1);
        virtual ~controllerDialog();




    private:

        //(*Handlers(controllerDialog)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OntimerConnectionTrigger(wxTimerEvent& event);
        void OnLeftDown(wxMouseEvent& event);
        void OnLeftUp(wxMouseEvent& event);
        void OnPaint(wxPaintEvent& event);
        void OntimerHoldTrigger(wxTimerEvent& event);
        void OntimerStopTrigger(wxTimerEvent& event);
        void OntimerMenuTrigger(wxTimerEvent& event);
        //*)

        //(*Identifiers(controllerDialog)
        static const long ID_TIMER_CONNECTION;
        static const long ID_TIMER_HOLD;
        static const long ID_TIMER_MENU;
        //*)

        //(*Declarations(controllerDialog)
        wxTimer m_timerConnection;
        wxTimer m_timerMenu;
        wxTimer m_timerStop;
        //*)
        wxTimer m_timerCheck;

        void OnWebsocketConnection(const wxCommandEvent& event);
        void OnWebsocketHandshake(const wxCommandEvent& event);
        void OnWebsocketFrame(const wxCommandEvent& event);
        void OnWebsocketFinished(const wxCommandEvent& event);
        void OnRestfulReply(const wxCommandEvent& event);
        void OnTimerTimeout(const wxTimerEvent& event);

        void OnTimerCheck(const wxTimerEvent& event);
        void UpdateLabels();

        void ReplyConfig(const Json::Value& jsData);
        void ReplyFiles(const Json::Value& jsData);
        void UpdatePlayingStatus(const Json::Value& jsData);

        void Play();
        void Stop();

        uiRect m_uiName;
        uiRect m_uiStatus;


        WebSocketClient m_wsClient;
        RestfulClient m_rClient;

        wxString m_sWSEndpoint;
        wxString m_sUrl;
        wxString m_sIpAddress;

        std::string m_sDefaultFileUid;
        wxString m_sDefaultFileLabel;

        wxTimeSpan m_tsLength;
        wxTimeSpan m_tsPosition;

        enum {DISCONNECTED, CONNECTING, CONNECTED};
        int m_nConnected;
        enum {STOPPED, PLAYING, ORPHANED};
        int m_nPlaying;
        bool m_bCountUp;

        bool m_bIgnoreUp;
        bool m_bDown;

        wxTimer m_timerTimeout;

        std::random_device m_rd;
        std::mt19937 m_gen;
        std::uniform_int_distribution<> m_dist;

        DECLARE_EVENT_TABLE()

        static const wxColour CLR_PLAYING;
        static const wxColour CLR_IDLE;
        static const wxColour CLR_CONNECTING;
        static const wxColour CLR_ERROR;
        static const wxColour CLR_NO_FILE;


};

#endif // CONTROLLERMAIN_H
