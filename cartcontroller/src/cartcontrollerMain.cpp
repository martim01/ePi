/***************************************************************
 * Name:      controllerMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Matthew Martin (martim01@outlook.com)
 * Created:   2020-06-15
 * Copyright: Matthew Martin (https://github.com/martim01)
 * License:
 **************************************************************/

#include "cartcontrollerMain.h"
#include <wx/msgdlg.h>
#include <wx/log.h>
#include "restfulclient.h"
#include "jsonutils.h"
#include "epiwriter.h"
#include <sstream>

#include <wx/display.h>
#include "dlgOptions.h"
#include "constants.h"
#include "pnlResource.h"
#include "wmlabel.h"
#include "dlgsystem.h"

const wxColour cartcontrollerDialog::CLR_PLAYING = wxColour(146,208,80);
const wxColour cartcontrollerDialog::CLR_IDLE = wxColour(141,180,226);
const wxColour cartcontrollerDialog::CLR_ERROR =  wxColour(255,0,0);
const wxColour cartcontrollerDialog::CLR_NO_FILE = wxColour(160,160,160);
const wxColour cartcontrollerDialog::CLR_CONNECTING = wxColour(255,255,0);
const wxColour cartcontrollerDialog::CLR_OKAY = wxColour(0,128,64);


const long cartcontrollerDialog::ID_TIMER_CONNECTION = wxNewId();
const long cartcontrollerDialog::ID_TIMER_HOLD = wxNewId();
const long cartcontrollerDialog::ID_TIMER_MENU = wxNewId();

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

cartcontrollerDialog::cartcontrollerDialog(wxWindow* parent,  const wxString& sIpAddress, unsigned short nPort, wxWindowID id) :
m_wsClient(),
m_rClient(this),
m_sIpAddress(sIpAddress),
m_nConnected(DISCONNECTED),
m_bCountUp(true),
m_bIgnoreUp(false),
m_bDown(false),
m_gen(m_rd()),
m_dist(500,1000)
{
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer2;
    wxGridSizer* GridSizer1;


    Create(parent, id, _("wxWidgets app"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("id"));


    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    m_ppnlStatus = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(800,40), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    m_ppnlStatus->SetBackgroundColour(CLR_ERROR);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    m_plblHostname = new wmLabel(m_ppnlStatus, wxNewId(), wxEmptyString, wxDefaultPosition, wxSize(-1, 40));
    m_plblHostname->SetBackgroundColour(CLR_ERROR);
    m_plblHostname->SetForegroundColour(*wxWHITE);
    m_plblHostname->SetFont(wxFont(20,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT));
    BoxSizer2->Add(m_plblHostname, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    m_pbtnSystem = new wmButton(this, wxNewId(), wxT("System"), wxDefaultPosition, wxSize(-1,34));
    m_pbtnSystem->SetForegroundColour(*wxWHITE);
    BoxSizer2->Add(m_pbtnSystem, 0, wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 10);

    m_ppnlStatus->SetSizer(BoxSizer2);

    BoxSizer1->Add(m_ppnlStatus, 0, wxALL|wxEXPAND, 0);
    GridSizer1 = new wxGridSizer(4, 5, 0, 0);

    m_vResourcePanels.resize(20);
    for(int i = 0; i < 20; i++)
    {
        m_vResourcePanels[i] = new pnlResource(this, wxNewId(), wxDefaultPosition, wxSize(160,110));
        GridSizer1->Add(m_vResourcePanels[i], 1, wxALL,0);
        //GridSizer1->Add(new wmLabel(this, wxNewId(), wxString::Format("%d",i )), 1, wxALL,2);
    }

    BoxSizer1->Add(GridSizer1, 1, wxALL|wxEXPAND, 0);
    SetSizer(BoxSizer1);

    int nWidth = (wxGetClientDisplayRect().GetWidth()-1);
    int nHeight = (wxGetClientDisplayRect().GetHeight()-1);
    SetSize(nWidth,nHeight);



    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);


    m_timerConnection.SetOwner(this, ID_TIMER_CONNECTION);
    m_timerStop.SetOwner(this, ID_TIMER_HOLD);


    Connect(ID_TIMER_CONNECTION,wxEVT_TIMER,(wxObjectEventFunction)&cartcontrollerDialog::OntimerConnectionTrigger);

    Connect(wxID_ANY, wxEVT_RESOURCE_PLAY, (wxObjectEventFunction)&cartcontrollerDialog::OnResourcePlay);
    Connect(wxID_ANY, wxEVT_RESOURCE_MENU, (wxObjectEventFunction)&cartcontrollerDialog::OnResourceMenu);
    Connect(wxID_ANY, wxEVT_RESOURCE_STOP, (wxObjectEventFunction)&cartcontrollerDialog::OnResourceStop);


    wxSetCursor(*wxSTANDARD_CURSOR);
    //wxSetCursor(wxNullCursor);


    Connect(wxID_ANY, wxEVT_WS_CONNECTION, (wxObjectEventFunction)&cartcontrollerDialog::OnWebsocketConnection);
    Connect(wxID_ANY, wxEVT_WS_HANDSHAKE, (wxObjectEventFunction)&cartcontrollerDialog::OnWebsocketHandshake);
    Connect(wxID_ANY, wxEVT_WS_FRAME, (wxObjectEventFunction)&cartcontrollerDialog::OnWebsocketFrame);
    Connect(wxID_ANY, wxEVT_WS_FINISHED, (wxObjectEventFunction)&cartcontrollerDialog::OnWebsocketFinished);
    Connect(wxID_ANY, wxEVT_R_REPLY, (wxObjectEventFunction)&cartcontrollerDialog::OnRestfulReply);

    Connect(m_pbtnSystem->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&cartcontrollerDialog::OnbtnSystem);


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
    if(jsValue.isArray())
    {
        UpdateResources(jsValue);
    }
    else if(jsValue["player"].isString())
    {
        UpdatePlayingStatus(jsValue);
    }
    else if(jsValue["system"].isObject())
    {
    }
}

void cartcontrollerDialog::UpdatePlayingStatus(const Json::Value& jsData)
{

    for(size_t i = 0; i < m_vResourcePanels.size(); i++)
    {
        m_vResourcePanels[i]->UpdatePlayingStatus(jsData);
    }



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
            break;
        case FILE_GET:
            ReplyAddFile(jsValue);
            break;
        case FILE_UPDATE:
            ReplyModifyFile(jsValue);
    }
}


void cartcontrollerDialog::ReplyConfig(const Json::Value& jsData)
{

    if(jsData["server"].isObject() && jsData["server"]["hostname"].isString())
    {
        m_plblHostname->SetLabel(wxString::FromUTF8(jsData["server"]["hostname"].asString().c_str()));
    }
    else
    {
        m_plblHostname->SetLabel(m_sIpAddress);
    }

}

void cartcontrollerDialog::ReplyFiles(const Json::Value& jsData)
{

    if(jsData.isArray() && jsData.size() > 0)
    {
        std::map<std::string, std::string> mFiles;
        for(size_t i = 0; i < jsData.size(); i++)
        {
            mFiles.insert(std::make_pair(jsData[i]["label"].asString(), jsData[i]["uid"].asString()));
        }

        size_t i = 0;
        for(auto pairFile : mFiles)
        {
            if(i < m_vResourcePanels.size())
            {
                m_vResourcePanels[i]->SetResource(pairFile.second, pairFile.first);
            }
            ++i;
        }
        for(; i < m_vResourcePanels.size(); i++)
        {
            m_vResourcePanels[i]->SetResource("", "");
        }
    }

   UpdateLabels();

}

void cartcontrollerDialog::UpdateLabels()
{

    wxColour clr(CLR_ERROR);
    switch(m_nConnected)
    {
        case DISCONNECTED:
            clr = CLR_ERROR;
            //m_uiStatus.SetLabel("Offline");
            break;
        case CONNECTING:
            clr = CLR_CONNECTING;
            //m_uiStatus.SetLabel("Connecting");
            break;
        case CONNECTED:
            clr = CLR_OKAY;
            break;
        default:
            clr = CLR_ERROR;
            //m_uiStatus.SetLabel("Error");
    }
    m_plblHostname->SetBackgroundColour(clr);
    m_ppnlStatus->SetBackgroundColour(clr);
}


void cartcontrollerDialog::Play(const wxString& sUid)
{
    Json::Value jsCommand;
    jsCommand["command"] = "play";
    jsCommand["type"] = "file";
    jsCommand["uid"] = sUid.ToStdString();
    jsCommand["times_to_play"] = 1;

    std::stringstream ss;
    epiWriter::Get().writeToSStream(jsCommand, ss);

    m_rClient.Patch((m_sUrl+STR_ENDPOINTS[STATUS]).ToStdString(), ss.str().data(), STATUS);
}

void cartcontrollerDialog::Stop(bool bKill)
{
    Json::Value jsCommand;
    if(!bKill)
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




void cartcontrollerDialog::OnTimerCheck(const wxTimerEvent& event)
{
    //Ask for status and info...
    m_rClient.Get((m_sUrl+STR_ENDPOINTS[CONFIG]).ToStdString(), CONFIG);
    m_rClient.Get((m_sUrl+STR_ENDPOINTS[FILES]).ToStdString(), FILES);
//    m_timerCheck.Start(10000, true);
}


void cartcontrollerDialog::OnTimerTimeout(const wxTimerEvent& event)
{
    m_nConnected = DISCONNECTED;
    m_wsClient.Stop();
    UpdateLabels();
    m_timerConnection.Start(m_dist(m_gen),true);
}


void cartcontrollerDialog::OnResourcePlay(const wxCommandEvent& event)
{
    //Stop();
    if(event.GetString().empty() == false)
    {
        Play(event.GetString());
    }
    else
    {
        Stop();
    }
}

void cartcontrollerDialog::OnResourceStop(const wxCommandEvent& event)
{
    Stop(event.GetInt());

}

void cartcontrollerDialog::OnResourceMenu(const wxCommandEvent& event)
{
    dlgOptions aDlg(this, dlgOptions::CART_FILE, m_wsClient, m_plblHostname->GetLabel(), m_sIpAddress, m_sUrl, event.GetString().ToStdString(), wxNewId(), wxPoint(0,0), wxSize(800,480));
    if(aDlg.ShowModal() == wxID_CANCEL)
    {
        EndModal(wxID_OK);
    }
    else
    {
        m_rClient.Get((m_sUrl+STR_ENDPOINTS[FILES]).ToStdString(), FILES);
    }
}


void cartcontrollerDialog::UpdateResources(Json::Value& jsResources)
{
    for(size_t i = 0; i < jsResources.size(); i++)
    {
        if(jsResources[i]["modification"].isString() && jsResources[i]["type"].isString() && jsResources[i]["uid"].isString())
        {
            if(jsResources[i]["modification"].asString() == "added")
            {
                AddResource(wxString::FromUTF8(jsResources[i]["uid"].asString().c_str()));
            }
            else if(jsResources[i]["modification"].asString() == "deleted")
            {
                DeleteResource(wxString::FromUTF8(jsResources[i]["uid"].asString().c_str()));
            }
            else if(jsResources[i]["modification"].asString() == "modified")
            {
                ModifyResource(wxString::FromUTF8(jsResources[i]["uid"].asString().c_str()));
            }
        }
    }
}

void cartcontrollerDialog::AddResource(const wxString& sUid)
{
    m_rClient.Get((m_sUrl+STR_ENDPOINTS[FILES]+"/"+sUid).ToStdString(), FILE_GET);
}

void cartcontrollerDialog::DeleteResource(const wxString& sUid)
{
    for(size_t i =0; i < m_vResourcePanels.size(); i++)
    {
        if(m_vResourcePanels[i]->GetUid() == sUid)
        {
            m_vResourcePanels[i]->SetResource("", "");
            break;
        }
    }
}

void cartcontrollerDialog::ModifyResource(const wxString& sUid)
{
    m_rClient.Get((m_sUrl+STR_ENDPOINTS[FILES]+"/"+sUid).ToStdString(), FILE_UPDATE);
}


void cartcontrollerDialog::ReplyAddFile(const Json::Value& jsData)
{
    if(jsData["uid"].isString() && jsData["label"].isString())
    {
        for(size_t i =0; i < m_vResourcePanels.size(); i++)
        {
            if(m_vResourcePanels[i]->GetUid().empty())
            {
                m_vResourcePanels[i]->SetResource(jsData["uid"].asString(),jsData["label"].asString());
                break;
            }
        }
    }
}

void cartcontrollerDialog::ReplyModifyFile(const Json::Value& jsData)
{
    if(jsData["uid"].isString() && jsData["label"].isString())
    {
        for(size_t i =0; i < m_vResourcePanels.size(); i++)
        {
            if(m_vResourcePanels[i]->GetUid().ToStdString() == jsData["uid"].asString())
            {
                m_vResourcePanels[i]->SetResource(jsData["uid"].asString(),jsData["label"].asString());
                break;
            }
        }
    }
}

void cartcontrollerDialog::OnbtnSystem(const wxCommandEvent& event)
{
    dlgOptions aDlg(this, dlgOptions::CART_SYSTEM, m_wsClient, m_plblHostname->GetLabel(), m_sIpAddress, m_sUrl, "");
    if(aDlg.ShowModal() == wxID_CANCEL)
    {
        EndModal(wxID_OK);
    }
}
