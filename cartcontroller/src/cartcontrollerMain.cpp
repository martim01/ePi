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
#include "pnlResource.h"

const wxColour cartcontrollerDialog::CLR_PLAYING = wxColour(146,208,80);
const wxColour cartcontrollerDialog::CLR_IDLE = wxColour(141,180,226);
const wxColour cartcontrollerDialog::CLR_ERROR =  wxColour(255,0,0);
const wxColour cartcontrollerDialog::CLR_NO_FILE = wxColour(160,160,160);
const wxColour cartcontrollerDialog::CLR_CONNECTING = wxColour(255,255,0);



//(*InternalHeaders(cartcontrollerDialog)
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

//(*IdInit(cartcontrollerDialog)
const long cartcontrollerDialog::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(cartcontrollerDialog,wxDialog)
    //(*EventTable(cartcontrollerDialog)
    //*)
END_EVENT_TABLE()

cartcontrollerDialog::cartcontrollerDialog(wxWindow* parent,  const wxPoint pntLayout, unsigned int nController, const wxString& sIpAddress, unsigned short nPort, wxWindowID id) :
m_wsClient(),
m_rClient(this),
m_sIpAddress(sIpAddress),
m_nConnected(DISCONNECTED),
m_nPlaying(STOPPED),
m_bCountUp(true),
m_bIgnoreUp(false),
m_bDown(false),
m_gen(m_rd()),
m_dist(500,1000)
{
    //(*Initialize(cartcontrollerDialog)
    wxBoxSizer* BoxSizer1;
    wxGridSizer* GridSizer1;

    Create(parent, id, _("wxWidgets app"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    m_ppnlStatus = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer1->Add(m_ppnlStatus, 0, wxALL|wxEXPAND, 0);
    GridSizer1 = new wxGridSizer(6, 8, 0, 0);
    BoxSizer1->Add(GridSizer1, 1, wxALL|wxEXPAND, 0);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    //*)

    wxFont thisFont(20,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
    SetFont(thisFont);

    for(size_t i = 0; i < 48; i++)
    {
        GridSizer1->Add(new pnlResource(this), 1, wxEXPAND, 1);
    }




    m_timerConnection.SetOwner(this, ID_TIMER_CONNECTION);
    m_timerStop.SetOwner(this, ID_TIMER_HOLD);
    m_timerMenu.SetOwner(this, ID_TIMER_MENU);

    Connect(ID_TIMER_CONNECTION,wxEVT_TIMER,(wxObjectEventFunction)&cartcontrollerDialog::OntimerConnectionTrigger);
    Connect(ID_TIMER_HOLD,wxEVT_TIMER,(wxObjectEventFunction)&cartcontrollerDialog::OntimerStopTrigger);
    Connect(ID_TIMER_MENU,wxEVT_TIMER,(wxObjectEventFunction)&cartcontrollerDialog::OntimerMenuTrigger);

    int nWidth = (wxGetClientDisplayRect().GetWidth()-1);
    int nHeight = (wxGetClientDisplayRect().GetHeight()-1);

    SetSize(nWidth,nHeight);

    wxSetCursor(*wxSTANDARD_CURSOR);
    //wxSetCursor(wxNullCursor);


    Connect(wxID_ANY, wxEVT_WS_CONNECTION, (wxObjectEventFunction)&cartcontrollerDialog::OnWebsocketConnection);
    Connect(wxID_ANY, wxEVT_WS_HANDSHAKE, (wxObjectEventFunction)&cartcontrollerDialog::OnWebsocketHandshake);
    Connect(wxID_ANY, wxEVT_WS_FRAME, (wxObjectEventFunction)&cartcontrollerDialog::OnWebsocketFrame);
    Connect(wxID_ANY, wxEVT_WS_FINISHED, (wxObjectEventFunction)&cartcontrollerDialog::OnWebsocketFinished);
    Connect(wxID_ANY, wxEVT_R_REPLY, (wxObjectEventFunction)&cartcontrollerDialog::OnRestfulReply);



    m_timerCheck.SetOwner(this, wxNewId());
    Connect(m_timerCheck.GetId(), wxEVT_TIMER, (wxObjectEventFunction)&cartcontrollerDialog::OnTimerCheck);

    SetBackgroundColour(wxColour(255,255,255));
    UpdateLabels();

    m_sIpAddress.Printf("%s:%u", sIpAddress.c_str(), nPort);

    m_sWSEndpoint.Printf("ws://%s", m_sIpAddress.c_str());
    m_sUrl.Printf("http://%s/x-epi/", m_sIpAddress.c_str());

    m_wsClient.AddHandler(this);
    m_wsClient.Connect(std::string(m_sWSEndpoint.mb_str()));

    m_timerTimeout.SetOwner(this, wxNewId());
    Connect(m_timerTimeout.GetId(), wxEVT_TIMER, (wxObjectEventFunction)&cartcontrollerDialog::OnTimerTimeout);
    m_timerTimeout.Start(5000, true);
}

cartcontrollerDialog::~cartcontrollerDialog()
{
    //(*Destroy(cartcontrollerDialog)
    //*)
}

void cartcontrollerDialog::OnQuit(wxCommandEvent& event)
{
    Close();
}

void cartcontrollerDialog::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}


void cartcontrollerDialog::OnWebsocketConnection(const wxCommandEvent& event)
{
    if(event.GetInt() != 0)
    {
        m_nConnected = DISCONNECTED;
        UpdateLabels();
        m_timerConnection.Start(m_dist(m_gen),true);
    }
    else
    {
        m_nConnected = CONNECTING;
        m_timerCheck.Start(100,true);

        UpdateLabels();
    }
}

void cartcontrollerDialog::OnWebsocketHandshake(const wxCommandEvent& event)
{
    if(event.GetInt() == 101)
    {
        m_nConnected = CONNECTED;
        UpdateLabels();
        //m_uiStatus.SetLabel("Connected");
    }
    else
    {
        m_nConnected = DISCONNECTED;
        UpdateLabels();
        //m_uiStatus.SetLabel(wxString::Format("HTTP: %d", event.GetInt()));

    }
}

void cartcontrollerDialog::OnWebsocketFrame(const wxCommandEvent& event)
{
    m_timerTimeout.Stop();
    m_timerTimeout.Start(5000, true);

    Json::Value jsValue(ConvertToJson(event.GetString().ToStdString()));
    if(jsValue["player"].isString())
    {
        UpdatePlayingStatus(jsValue);
    }
}

void cartcontrollerDialog::UpdatePlayingStatus(const Json::Value& jsData)
{
    /*
    if(jsData["player"].asString() == "Running")
    {
        m_nPlaying = PLAYING;
        if(jsData["status"].isObject() && jsData["status"]["playing"].isObject())
        {
           m_tsLength = wxTimeSpan(0,0,0, jsData["status"]["playing"]["length"].asInt());
           m_tsPosition = wxTimeSpan(0,0,0, jsData["status"]["playing"]["time"].asInt());
        }
    }
    else if(jsData["player"].asString() == "Orphaned")
    {
        m_nPlaying = ORPHANED;
        m_tsLength = wxTimeSpan(0);
        m_tsPosition = wxTimeSpan(0);
    }
    else
    {
        m_nPlaying = STOPPED;
        m_tsLength = wxTimeSpan(0);
        m_tsPosition = wxTimeSpan(0);
    }
    UpdateLabels();
    */
}

void cartcontrollerDialog::OnWebsocketFinished(const wxCommandEvent& event)
{
    m_nConnected = DISCONNECTED;
    UpdateLabels();
    //m_uiStatus.SetLabel("Offline");


    m_timerConnection.Start(m_dist(m_gen),true);
}



void cartcontrollerDialog::OntimerConnectionTrigger(wxTimerEvent& event)
{
    m_wsClient.Connect(std::string(m_sWSEndpoint.mb_str()));
}


void cartcontrollerDialog::OnRestfulReply(const wxCommandEvent& event)
{
    wxLogDebug("OnRestfulReply");
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


void cartcontrollerDialog::ReplyConfig(const Json::Value& jsData)
{
    /*
    if(jsData["server"].isObject() && jsData["server"]["hostname"].isString())
    {
        m_uiName.SetLabel(wxString::FromUTF8(jsData["server"]["hostname"].asString().c_str()));
    }
    else
    {
        m_uiName.SetLabel("????");
    }
    */
}

void cartcontrollerDialog::ReplyFiles(const Json::Value& jsData)
{
    /*
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
   */
}

void cartcontrollerDialog::UpdateLabels()
{
    /*
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
            else if(m_nPlaying == STOPPED)
            {
                clr = CLR_IDLE;
                m_uiStatus.SetLabel(m_sDefaultFileLabel);
            }
            else if(m_nPlaying  == ORPHANED)
            {
                clr = CLR_PLAYING;
                m_uiStatus.SetLabel("??:??:??");
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
    */
}
/*
void cartcontrollerDialog::OnLeftDown(wxMouseEvent& event)
{
    m_bDown = true;
    m_uiName.SetBackgroundColour(wxColour(255,128,0));
    m_uiStatus.SetBackgroundColour(wxColour(255,128,0));
    Refresh();
    if(m_nPlaying != STOPPED)
    {
        m_timerStop.Start(2000,true);
    }
    else
    {
        m_timerMenu.Start(4000,true);
    }
}

void cartcontrollerDialog::OnLeftUp(wxMouseEvent& event)
{
    m_timerMenu.Stop();
    m_timerStop.Stop();
    if(m_bIgnoreUp)
    {
        m_bIgnoreUp = false;
    }
    else if(m_bDown)
    {
        if(m_sDefaultFileUid.empty() == false)
        {
            if(m_nPlaying == STOPPED)
            {
                Play();
            }
            else
            {
                m_bCountUp = !m_bCountUp;
            }
        }
    }
    m_bDown = false;
    UpdateLabels();
}
*/

void cartcontrollerDialog::Play()
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

void cartcontrollerDialog::Stop()
{

    Json::Value jsCommand;
    if(m_nPlaying == PLAYING)
    {
        jsCommand["command"] = "stop";
    }
    else
    {
        jsCommand["command"] = "kill";
    }

    std::stringstream ss;
    epiWriter::Get().writeToSStream(jsCommand, ss);

    m_rClient.Patch((m_sUrl+STR_ENDPOINTS[STATUS]).ToStdString(), ss.str().data(), STATUS);
}

void cartcontrollerDialog::OntimerStopTrigger(wxTimerEvent& event)
{
    Stop();
    m_bIgnoreUp = true;
    m_bDown = false;
    UpdateLabels();
}

void cartcontrollerDialog::OntimerMenuTrigger(wxTimerEvent& event)
{
    m_bIgnoreUp = true;
    m_bDown = false;
    UpdateLabels();

    dlgOptions aDlg(this, m_wsClient, m_uiName.GetLabel(), m_sIpAddress, m_sUrl, m_sDefaultFileUid, wxNewId(), wxPoint(0,0), wxSize(800,480));
    if(aDlg.ShowModal() == wxID_CANCEL)
    {
        EndModal(wxID_OK);
    }
    else
    {
        m_rClient.Get((m_sUrl+STR_ENDPOINTS[FILES]).ToStdString(), FILES);
    }
}


void cartcontrollerDialog::OnTimerCheck(const wxTimerEvent& event)
{
    wxLogDebug("OnTimerCheck");
    //Ask for status and info...
    m_rClient.Get((m_sUrl+STR_ENDPOINTS[CONFIG]).ToStdString(), CONFIG);
    m_rClient.Get((m_sUrl+STR_ENDPOINTS[FILES]).ToStdString(), FILES);
    m_timerCheck.Start(10000, true);
}


void cartcontrollerDialog::OnTimerTimeout(const wxTimerEvent& event)
{
    m_nConnected = DISCONNECTED;
    m_wsClient.Stop();
    UpdateLabels();
    m_timerConnection.Start(m_dist(m_gen),true);
}
