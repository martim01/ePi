/***************************************************************
 * Name:      controllerMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Matthew Martin (martim01@outlook.com)
 * Created:   2020-06-15
 * Copyright: Matthew Martin (https://github.com/martim01)
 * License:
 **************************************************************/

#include "controllerMain.h"
#include <wx/msgdlg.h>
#include <wx/log.h>
#include "restfulclient.h"
#include "jsonutils.h"
#include "epiwriter.h"
#include <sstream>
#include <wx/dcbuffer.h>
#include <wx/display.h>
#include "dlgOptions.h"
#include "constants.h"

const wxColour controllerDialog::CLR_PLAYING = wxColour(146,208,80);
const wxColour controllerDialog::CLR_IDLE = wxColour(141,180,226);
const wxColour controllerDialog::CLR_ERROR =  wxColour(255,0,0);
const wxColour controllerDialog::CLR_NO_FILE = wxColour(160,160,160);
const wxColour controllerDialog::CLR_CONNECTING = wxColour(255,255,0);



//(*InternalHeaders(controllerDialog)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(controllerDialog)
const long controllerDialog::ID_TIMER_CONNECTION = wxNewId();
const long controllerDialog::ID_TIMER_HOLD = wxNewId();
const long controllerDialog::ID_TIMER_MENU = wxNewId();
//*)

BEGIN_EVENT_TABLE(controllerDialog,wxDialog)
    //(*EventTable(controllerDialog)
    //*)
END_EVENT_TABLE()

controllerDialog::controllerDialog(wxWindow* parent,  const wxPoint pntLayout, unsigned int nController, const wxString& sIpAddress, unsigned short nPort, wxWindowID id) : m_wsClient(this), m_rClient(this),
m_nConnected(DISCONNECTED),
m_bPlaying(false),
m_bCountUp(true),
m_bIgnoreUp(false),
m_bDown(false)
{
    //(*Initialize(controllerDialog)
    Create(parent, id, _("wxWidgets app"), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, _T("id"));
    SetClientSize(wxSize(200,160));
    wxFont thisFont(20,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
    SetFont(thisFont);
    m_timerConnection.SetOwner(this, ID_TIMER_CONNECTION);
    m_timerStop.SetOwner(this, ID_TIMER_HOLD);
    m_timerMenu.SetOwner(this, ID_TIMER_MENU);

    Connect(ID_TIMER_CONNECTION,wxEVT_TIMER,(wxObjectEventFunction)&controllerDialog::OntimerConnectionTrigger);
    Connect(ID_TIMER_HOLD,wxEVT_TIMER,(wxObjectEventFunction)&controllerDialog::OntimerStopTrigger);
    Connect(ID_TIMER_MENU,wxEVT_TIMER,(wxObjectEventFunction)&controllerDialog::OntimerMenuTrigger);
    Connect(wxEVT_PAINT,(wxObjectEventFunction)&controllerDialog::OnPaint);
    Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&controllerDialog::OnLeftDown);
    Connect(wxEVT_LEFT_UP,(wxObjectEventFunction)&controllerDialog::OnLeftUp);
    //*)


    int nWidth = wxDisplay().GetGeometry().GetWidth()/pntLayout.x;
    int nHeight = wxDisplay().GetGeometry().GetHeight()/pntLayout.y;

    SetSize(nWidth,nHeight);
    int nColumn = nController%pntLayout.x;
    int nRow = nController/pntLayout.x;

    SetPosition(wxPoint(nColumn*nWidth, nRow*nHeight));


    m_uiName.SetRect(0,0,GetClientRect().GetWidth(),GetClientRect().GetHeight()/2);
    m_uiStatus.SetRect(0,m_uiName.GetBottom(),m_uiName.GetWidth(),m_uiName.GetHeight()+1);

    m_uiName.SetGradient(0);
    m_uiStatus.SetGradient(0);
    m_uiName.SetForegroundColour(*wxBLACK);
    m_uiStatus.SetForegroundColour(*wxBLACK);

    Connect(wxID_ANY, wxEVT_WS_CONNECTION, (wxObjectEventFunction)&controllerDialog::OnWebsocketConnection);
    Connect(wxID_ANY, wxEVT_WS_HANDSHAKE, (wxObjectEventFunction)&controllerDialog::OnWebsocketHandshake);
    Connect(wxID_ANY, wxEVT_WS_FRAME, (wxObjectEventFunction)&controllerDialog::OnWebsocketFrame);
    Connect(wxID_ANY, wxEVT_WS_FINISHED, (wxObjectEventFunction)&controllerDialog::OnWebsocketFinished);
    Connect(wxID_ANY, wxEVT_R_REPLY, (wxObjectEventFunction)&controllerDialog::OnRestfulReply);

    SetBackgroundColour(wxColour(50,50,80));
    UpdateLabels();


    m_sWSEndpoint.Printf("ws://%s:%u", sIpAddress.c_str(), nPort);
    m_sUrl.Printf("http://%s:%u/x-epi/", sIpAddress.c_str(), nPort);

    m_rClient.Get((m_sUrl+STR_ENDPOINTS[CONFIG]).ToStdString(), CONFIG);
    m_rClient.Get((m_sUrl+STR_ENDPOINTS[FILES]).ToStdString(), FILES);

    m_wsClient.Connect(std::string(m_sWSEndpoint.mb_str()));
}

controllerDialog::~controllerDialog()
{
    //(*Destroy(controllerDialog)
    //*)
}

void controllerDialog::OnQuit(wxCommandEvent& event)
{
    Close();
}

void controllerDialog::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}


void controllerDialog::OnWebsocketConnection(const wxCommandEvent& event)
{
    wxLogDebug("Connection=%d", event.GetInt());
    if(event.GetInt() != 0)
    {
        m_nConnected = DISCONNECTED;
        UpdateLabels();
        m_timerConnection.Start(100,true);
    }
    else
    {
        m_nConnected = CONNECTING;
        UpdateLabels();
    }
}

void controllerDialog::OnWebsocketHandshake(const wxCommandEvent& event)
{
    if(event.GetInt() == 101)
    {
        m_nConnected = CONNECTED;
        UpdateLabels();
        m_uiStatus.SetLabel("Connected");
    }
    else
    {
        m_nConnected = DISCONNECTED;
        UpdateLabels();
        m_uiStatus.SetLabel(wxString::Format("HTTP: %d", event.GetInt()));

    }
}

void controllerDialog::OnWebsocketFrame(const wxCommandEvent& event)
{
    Json::Value jsValue(ConvertToJson(event.GetString().ToStdString()));
    if(jsValue["player"].isString())
    {
        UpdatePlayingStatus(jsValue);
    }
}

void controllerDialog::UpdatePlayingStatus(const Json::Value& jsData)
{
    if(jsData["player"].asString() == "Running")
    {
        m_bPlaying = true;
        if(jsData["status"].isObject() && jsData["status"]["playing"].isObject())
        {
           m_tsLength = wxTimeSpan(0,0,0, jsData["status"]["playing"]["length"].asInt());
           m_tsPosition = wxTimeSpan(0,0,0, jsData["status"]["playing"]["time"].asInt());
        }
    }
    else
    {
        m_bPlaying = false;
        m_tsLength = wxTimeSpan(0);
        m_tsPosition = wxTimeSpan(0);
    }
    UpdateLabels();
}

void controllerDialog::OnWebsocketFinished(const wxCommandEvent& event)
{
    wxLogDebug("Finished");

    m_nConnected = DISCONNECTED;
    UpdateLabels();
    m_uiStatus.SetLabel("Offline");


    m_timerConnection.Start(100,true);
}



void controllerDialog::OntimerConnectionTrigger(wxTimerEvent& event)
{
    m_wsClient.Connect(std::string(m_sWSEndpoint.mb_str()));
}


void controllerDialog::OnRestfulReply(const wxCommandEvent& event)
{
    Json::Value jsValue(ConvertToJson(event.GetString().ToStdString()));
    switch(event.GetInt())
    {
        case CONFIG:
            ReplyConfig(jsValue);
            break;
        case FILES:
            ReplyFiles(jsValue);
    }
}


void controllerDialog::ReplyConfig(const Json::Value& jsData)
{
    if(jsData["server"].isObject() && jsData["server"]["hostname"].isString())
    {
        m_uiName.SetLabel(wxString::FromUTF8(jsData["server"]["hostname"].asString().c_str()));
    }
    else
    {
        m_uiName.SetLabel("????");
    }

}

void controllerDialog::ReplyFiles(const Json::Value& jsData)
{
    wxLogDebug("ReplyFiles");
    if(jsData.isArray() && jsData.size() > 0)
    {
        m_sDefaultFileLabel = wxString::FromUTF8(jsData[0]["label"].asString().c_str());
        m_sDefaultFileUid = jsData[0]["uid"].asString();
   }
   else
   {
       m_sDefaultFileLabel = "";
       m_sDefaultFileUid = "";
   }
   UpdateLabels();
}

void controllerDialog::UpdateLabels()
{
    wxColour clr(CLR_ERROR);
    switch(m_nConnected)
    {
        case DISCONNECTED:
            clr = CLR_ERROR;
            m_uiStatus.SetLabel("Offline");
            break;
        case CONNECTING:
            clr = CLR_CONNECTING;
            m_uiStatus.SetLabel("Connecting");
            break;
        case CONNECTED:
            if(m_sDefaultFileUid.empty())
            {
                clr = CLR_NO_FILE;
                m_uiStatus.SetLabel("No File");
            }
            else if(m_bPlaying == false)
            {
                clr = CLR_IDLE;
                m_uiStatus.SetLabel(m_sDefaultFileLabel);
            }
            else
            {
                clr = CLR_PLAYING;
                if(m_bCountUp)
                {
                    m_uiStatus.SetLabel(m_tsPosition.Format("%H:%M:%S"));
                }
                else
                {
                    m_uiStatus.SetLabel((m_tsLength-m_tsPosition).Format("-%H:%M:%S"));
                }
            }
            break;
        default:
            clr = CLR_ERROR;
            m_uiStatus.SetLabel("Error");
    }
    if(!m_bDown)
    {
        m_uiName.SetBackgroundColour(clr);
        m_uiStatus.SetBackgroundColour(clr);
    }
    Refresh();
    Update();
}

void controllerDialog::OnLeftDown(wxMouseEvent& event)
{
    m_bDown = true;
    m_uiName.SetBackgroundColour(wxColour(255,128,0));
    m_uiStatus.SetBackgroundColour(wxColour(255,128,0));
    Refresh();
    if(m_bPlaying)
    {
        m_timerStop.Start(2000,true);
    }
    else
    {
        m_timerMenu.Start(4000,true);
    }
}

void controllerDialog::OnLeftUp(wxMouseEvent& event)
{
    m_bDown = false;
    UpdateLabels();
    m_timerMenu.Stop();
    m_timerStop.Stop();
    if(m_bIgnoreUp)
    {
        m_bIgnoreUp = false;
    }
    else
    {
        if(m_sDefaultFileUid.empty() == false)
        {
            if(m_bPlaying == false)
            {
                Play();
            }
            else
            {
                m_bCountUp = !m_bCountUp;
            }
        }
        else
        {
            wxLogDebug("%d %s", m_bPlaying, m_sDefaultFileLabel.c_str());
        }
    }
}


void controllerDialog::Play()
{
    Json::Value jsCommand;
    jsCommand["command"] = "play";
    jsCommand["type"] = "file";
    jsCommand["uid"] = m_sDefaultFileUid;
    jsCommand["times_to_play"] = -1;

    std::stringstream ss;
    epiWriter::Get().writeToSStream(jsCommand, ss);

    m_rClient.Patch((m_sUrl+STR_ENDPOINTS[STATUS]).ToStdString(), ss.str().data(), STATUS);
}

void controllerDialog::Stop()
{
    Json::Value jsCommand;
    jsCommand["command"] = "stop";

    std::stringstream ss;
    epiWriter::Get().writeToSStream(jsCommand, ss);

    m_rClient.Patch((m_sUrl+STR_ENDPOINTS[STATUS]).ToStdString(), ss.str().data(), STATUS);
}

void controllerDialog::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    wxFont fnt(GetFont());
    fnt.SetPointSize(20);
    dc.SetFont(fnt);
    m_uiName.Draw(dc, uiRect::BORDER_NONE);
    if(m_bPlaying)
    {
        fnt.SetPointSize(16);
    }
    else
    {
        fnt.SetPointSize(12);
    }

    m_uiStatus.Draw(dc, uiRect::BORDER_NONE);
}

void controllerDialog::OntimerStopTrigger(wxTimerEvent& event)
{
    Stop();
    m_bIgnoreUp = true;
    m_bDown = false;
    UpdateLabels();
}

void controllerDialog::OntimerMenuTrigger(wxTimerEvent& event)
{
    m_bIgnoreUp = true;
    m_bDown = false;
    UpdateLabels();

    dlgOptions aDlg(this, m_uiName.GetLabel(), m_sUrl, m_sDefaultFileUid, wxNewId(), wxPoint(0,0), wxSize(800,480));
    aDlg.ShowModal();

    m_rClient.Get((m_sUrl+STR_ENDPOINTS[FILES]).ToStdString(), FILES);
}
