#include "dlgsystem.h"
#include "restfulclient.h"
#include "constants.h"
#include "jsonutils.h"
#include "version.h"
#include <wx/log.h>
#include "dlgEdit.h"
#include <sstream>
#include "json/json.h"
#include "epiwriter.h"
#include <wx/msgdlg.h>
#include "dlgUsb.h"
#include <map>
#include "dlgUpload.h"
#include "dlgError.h"
#include <sys/capability.h>
#include "dlgInfo.h"
//(*InternalHeaders(dlgSystem)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(dlgSystem)
const long dlgSystem::ID_STATICTEXT30 = wxNewId();
const long dlgSystem::ID_STATICTEXT1 = wxNewId();
const long dlgSystem::ID_STATICTEXT2 = wxNewId();
const long dlgSystem::ID_STATICTEXT3 = wxNewId();
const long dlgSystem::ID_STATICTEXT4 = wxNewId();
const long dlgSystem::ID_STATICTEXT5 = wxNewId();
const long dlgSystem::ID_STATICTEXT6 = wxNewId();
const long dlgSystem::ID_BUTTON_UPDATE = wxNewId();
const long dlgSystem::ID_STATICTEXT7 = wxNewId();
const long dlgSystem::ID_STATICTEXT8 = wxNewId();
const long dlgSystem::ID_STATICTEXT9 = wxNewId();
const long dlgSystem::ID_STATICTEXT10 = wxNewId();
const long dlgSystem::ID_STATICLINE1 = wxNewId();
const long dlgSystem::ID_STATICTEXT11 = wxNewId();
const long dlgSystem::ID_STATICTEXT12 = wxNewId();
const long dlgSystem::ID_STATICTEXT13 = wxNewId();
const long dlgSystem::ID_STATICTEXT17 = wxNewId();
const long dlgSystem::ID_STATICTEXT14 = wxNewId();
const long dlgSystem::ID_STATICTEXT18 = wxNewId();
const long dlgSystem::ID_STATICTEXT15 = wxNewId();
const long dlgSystem::ID_STATICTEXT19 = wxNewId();
const long dlgSystem::ID_STATICTEXT20 = wxNewId();
const long dlgSystem::ID_STATICLINE2 = wxNewId();
const long dlgSystem::ID_BUTTON_REPLACE = wxNewId();
const long dlgSystem::ID_BUTTON_DELETE = wxNewId();
const long dlgSystem::ID_STATICTEXT16 = wxNewId();
const long dlgSystem::ID_STATICTEXT25 = wxNewId();
const long dlgSystem::ID_BUTTON_UPDATE_EPISERVER = wxNewId();
const long dlgSystem::ID_STATICTEXT21 = wxNewId();
const long dlgSystem::ID_STATICTEXT26 = wxNewId();
const long dlgSystem::ID_BUTTON_UPDATE_PLAYER3 = wxNewId();
const long dlgSystem::ID_STATICTEXT22 = wxNewId();
const long dlgSystem::ID_STATICTEXT27 = wxNewId();
const long dlgSystem::ID_BUTTON_UPDATE_PLAYER67 = wxNewId();
const long dlgSystem::ID_STATICTEXT23 = wxNewId();
const long dlgSystem::ID_STATICTEXT28 = wxNewId();
const long dlgSystem::ID_BUTTON_UPDATE_CONTROLLER = wxNewId();
const long dlgSystem::ID_STATICTEXT24 = wxNewId();
const long dlgSystem::ID_STATICTEXT29 = wxNewId();
const long dlgSystem::ID_BUTTON_UPDATE_LAUNCHER = wxNewId();
const long dlgSystem::ID_BUTTON_RESTART_SERVER = wxNewId();
const long dlgSystem::ID_BUTTON_RESTART_OS = wxNewId();
const long dlgSystem::ID_BUTTON_SHUTDOWN_OS = wxNewId();
const long dlgSystem::ID_BUTTON_SSH = wxNewId();
const long dlgSystem::ID_BUTTON_BACK = wxNewId();
const long dlgSystem::ID_PROGRESSDIALOG1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(dlgSystem,wxDialog)
	//(*EventTable(dlgSystem)
	//*)
END_EVENT_TABLE()

dlgSystem::dlgSystem(wxWindow* parent, WebSocketClient& wsClient, const wxString& sHostname, const wxString& sIpAddress, const wxString& sUrl, wxWindowID id,const wxPoint& pos,const wxSize& size) :
    m_wsClient(wsClient),
    m_sIpAddress(sIpAddress),
    m_sUrl(sUrl),
    m_client(this)

{

	wxBoxSizer* BoxSizer0;
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer3;
	wxGridBagSizer* GridBagSizer1;
	wxGridBagSizer* GridBagSizer2;
	wxGridBagSizer* GridBagSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer3;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetMinSize(wxSize(800,480));
	SetBackgroundColour(wxColour(255,255,255));
	wxFont thisFont(12,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	SetFont(thisFont);

	BoxSizer0 = new wxBoxSizer(wxVERTICAL);
	m_pstHostname = new wmLabel(this, ID_STATICTEXT30, _("Label"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT30"));
	m_pstHostname->SetMinSize(wxSize(800,40));
	m_pstHostname->SetForegroundColour(wxColour(255,255,255));
	m_pstHostname->SetBackgroundColour(wxColour(0,128,64));
	wxFont m_pstHostnameFont(24,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	m_pstHostname->SetFont(m_pstHostnameFont);
	BoxSizer0->Add(m_pstHostname, 0, wxALL|wxEXPAND, 2);

    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);

	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, wxEmptyString);
	GridBagSizer3 = new wxGridBagSizer(0, 0);
	StaticText9 = new wmLabel(this, ID_STATICTEXT16, _("episerver:"), wxDefaultPosition, wxSize(100,-1), 0, _T("ID_STATICTEXT16"));
	StaticText9->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText9, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionEpiServer = new wmLabel(this, ID_STATICTEXT25, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT25"));
	m_pstVersionEpiServer->SetMinSize(wxSize(100,-1));
	m_pstVersionEpiServer->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionEpiServer->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionEpiServer, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnUpdateEpiServer = new wmButton(this, ID_BUTTON_UPDATE_EPISERVER, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_EPISERVER"));
	GridBagSizer3->Add(m_pbtnUpdateEpiServer, wxGBPosition(0, 2), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText13 = new wmLabel(this, ID_STATICTEXT21, _("player3:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT21"));
	StaticText13->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText13, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionPlayer3 = new wmLabel(this, ID_STATICTEXT26, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT26"));
	m_pstVersionPlayer3->SetMinSize(wxSize(100,-1));
	m_pstVersionPlayer3->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionPlayer3->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionPlayer3, wxGBPosition(1, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnUpdatePlayer3 = new wmButton(this, ID_BUTTON_UPDATE_PLAYER3, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_PLAYER3"));
	GridBagSizer3->Add(m_pbtnUpdatePlayer3, wxGBPosition(1, 2), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText14 = new wmLabel(this, ID_STATICTEXT22, _("player67:"), wxDefaultPosition, wxSize(100,-1), 0, _T("ID_STATICTEXT22"));
	StaticText14->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText14, wxGBPosition(1, 3), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionPlayer67 = new wmLabel(this, ID_STATICTEXT27, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT27"));
	m_pstVersionPlayer67->SetMinSize(wxSize(100,-1));
	m_pstVersionPlayer67->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionPlayer67->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionPlayer67, wxGBPosition(1, 4), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnUpdatePlayer67 = new wmButton(this, ID_BUTTON_UPDATE_PLAYER67, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_PLAYER67"));
	GridBagSizer3->Add(m_pbtnUpdatePlayer67, wxGBPosition(1, 5), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText15 = new wmLabel(this, ID_STATICTEXT23, _("controller:"), wxDefaultPosition, wxSize(100,-1), 0, _T("ID_STATICTEXT23"));
	StaticText15->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText15, wxGBPosition(2, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionController = new wmLabel(this, ID_STATICTEXT28, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT28"));
	m_pstVersionController->SetMinSize(wxSize(100,-1));
	m_pstVersionController->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionController->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionController, wxGBPosition(2, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnUpdateController = new wmButton(this, ID_BUTTON_UPDATE_CONTROLLER, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_CONTROLLER"));
	GridBagSizer3->Add(m_pbtnUpdateController, wxGBPosition(2, 2), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText16 = new wmLabel(this, ID_STATICTEXT24, _("launcher:"), wxDefaultPosition, wxSize(100,-1), 0, _T("ID_STATICTEXT24"));
	StaticText16->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText16, wxGBPosition(2, 3), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionLauncher = new wmLabel(this, ID_STATICTEXT29, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT29"));
	m_pstVersionLauncher->SetMinSize(wxSize(100,-1));
	m_pstVersionLauncher->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionLauncher->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionLauncher, wxGBPosition(2, 4), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);

	m_pbtnInfo = new wmButton(this, wxNewId(), _("Server Info"), wxDefaultPosition, wxSize(150,-1), 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_LAUNCHER"));
	GridBagSizer3->Add(m_pbtnInfo, wxGBPosition(0, 3), wxGBSpan(1,2), wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);

	StaticBoxSizer2->Add(GridBagSizer3, 1, wxALL|wxEXPAND, 0);
	BoxSizer1->Add(StaticBoxSizer2, 1, wxBOTTOM|wxLEFT|wxEXPAND, 5);


	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, wxEmptyString);
	m_pbtnRestartServer = new wmButton(this, ID_BUTTON_RESTART_SERVER, _("Restart\nServer"), wxDefaultPosition, wxSize(100,40), wmButton::STYLE_HOLD, wxDefaultValidator, _T("ID_BUTTON_RESTART_SERVER"));
	m_pbtnRestartServer->SetForegroundColour(wxColour(255,255,255));
	m_pbtnRestartServer->SetBackgroundColour(wxColour(128,0,0));
	StaticBoxSizer3->Add(m_pbtnRestartServer, 1, wxALL|wxEXPAND,2);
	m_pbtnRestartOS = new wmButton(this, ID_BUTTON_RESTART_OS, _("Restart\nServer OS"), wxDefaultPosition, wxDefaultSize, wmButton::STYLE_HOLD, wxDefaultValidator, _T("ID_BUTTON_RESTART_OS"));
	m_pbtnRestartOS->SetForegroundColour(wxColour(255,255,255));
	m_pbtnRestartOS->SetBackgroundColour(wxColour(128,0,0));
	StaticBoxSizer3->Add(m_pbtnRestartOS, 1, wxALL|wxEXPAND,2);
	m_pbtnShutdownOS = new wmButton(this, ID_BUTTON_SHUTDOWN_OS, _("Shutdown\nServer OS"), wxDefaultPosition, wxDefaultSize, wmButton::STYLE_HOLD, wxDefaultValidator, _T("ID_BUTTON_SHUTDOWN_OS"));
	m_pbtnShutdownOS->SetForegroundColour(wxColour(255,255,255));
	m_pbtnShutdownOS->SetBackgroundColour(wxColour(128,0,0));
	StaticBoxSizer3->Add(m_pbtnShutdownOS, 1, wxALL|wxEXPAND,2);

	m_pbtnStopController = new wmButton(this, wxNewId(), _("Restart\nController"), wxDefaultPosition, wxDefaultSize, wmButton::STYLE_HOLD, wxDefaultValidator, _T("ID_BUTTON_SHUTDOWN_OS"));
	m_pbtnStopController->SetForegroundColour(wxColour(255,255,255));
	m_pbtnStopController->SetBackgroundColour(wxColour(128,0,0));
	StaticBoxSizer3->Add(m_pbtnStopController, 1, wxALL|wxEXPAND,2);


	StaticBoxSizer3->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);



	m_pbtnSSH = new wmButton(this, ID_BUTTON_SSH, _("SSH"), wxDefaultPosition, wxDefaultSize, wmButton::STYLE_HOLD, wxDefaultValidator, _T("ID_BUTTON_SSH"));
	m_pbtnSSH->SetForegroundColour(wxColour(255,255,255));
	m_pbtnSSH->SetBackgroundColour(wxColour(68,1,158));
	StaticBoxSizer3->Add(m_pbtnSSH, 1, wxALL|wxEXPAND,2);
	StaticBoxSizer3->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	m_pbtnBack = new wmButton(this, ID_BUTTON_BACK, _("Back"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BACK"));
	m_pbtnBack->SetForegroundColour(wxColour(255,255,255));
	m_pbtnBack->SetBackgroundColour(wxColour(0,128,0));
	StaticBoxSizer3->Add(m_pbtnBack, 1, wxALL|wxEXPAND,2);
	BoxSizer1->Add(StaticBoxSizer3, 0, wxBOTTOM|wxRIGHT|wxEXPAND, 5);
	BoxSizer0->Add(BoxSizer1, 1, wxALL|wxEXPAND, 0);

	SetSizer(BoxSizer0);
	BoxSizer0->Fit(this);
	BoxSizer0->SetSizeHints(this);

	Connect(ID_BUTTON_UPDATE_EPISERVER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnUpdateEpiServerClick);
	Connect(ID_BUTTON_UPDATE_PLAYER3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnUpdatePlayer3Click);
	Connect(ID_BUTTON_UPDATE_PLAYER67,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnUpdatePlayer67Click);
	Connect(ID_BUTTON_UPDATE_CONTROLLER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnUpdateControllerClick);
	Connect(ID_BUTTON_RESTART_SERVER,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgSystem::OnbtnRestartServerClick);
	Connect(ID_BUTTON_RESTART_OS,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgSystem::OnbtnRestartOSClick);
	Connect(ID_BUTTON_SHUTDOWN_OS,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgSystem::OnbtnShutdownOSClick);
	Connect(m_pbtnStopController->GetId(),wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgSystem::OnbtnShutdownControllerClick);
	Connect(ID_BUTTON_SSH,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgSystem::OnbtnSSHClick);
	Connect(ID_BUTTON_BACK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnBackClick);
	Connect(m_pbtnInfo->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnInfoClick);


	m_pbtnInfo->SetBackgroundColour(wxColour(0,128,128));


    m_pbtnUpdateController->SetBackgroundColour(wxColour(0,30,100));
    m_pbtnUpdateEpiServer->SetBackgroundColour(wxColour(0,30,100));
    m_pbtnUpdatePlayer3->SetBackgroundColour(wxColour(0,30,100));
    m_pbtnUpdatePlayer67->SetBackgroundColour(wxColour(0,30,100));

    m_pbtnUpdateController->SetColourDisabled(wxColour(140,140,140));
    m_pbtnUpdateEpiServer->SetColourDisabled(wxColour(140,140,140));
    m_pbtnUpdatePlayer3->SetColourDisabled(wxColour(140,140,140));
    m_pbtnUpdatePlayer67->SetColourDisabled(wxColour(140,140,140));

    m_pbtnRestartServer->SetColourDisabled(wxColour(140,140,140));
    m_pbtnRestartOS->SetColourDisabled(wxColour(140,140,140));
    m_pbtnShutdownOS->SetColourDisabled(wxColour(140,140,140));

	m_pstHostname->SetTextAlign(wxALIGN_CENTER);
    StaticText9->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
    StaticText13->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
    StaticText14->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
    StaticText15->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
    StaticText16->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);



    m_pstVersionController->SetBorderState(uiRect::BORDER_FLAT);
    m_pstVersionEpiServer->SetBorderState(uiRect::BORDER_FLAT);
    m_pstVersionLauncher->SetBorderState(uiRect::BORDER_FLAT);
    m_pstVersionPlayer3->SetBorderState(uiRect::BORDER_FLAT);
    m_pstVersionPlayer67->SetBorderState(uiRect::BORDER_FLAT);

    ShowConnectedButtons(false);

	/*
	Connect(ID_BUTTON_UPDATE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnFileUpdateClick);
	Connect(ID_BUTTON_REPLACE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnReplaceClick);
	Connect(ID_BUTTON_DELETE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnDeleteClick);
	Connect(ID_BUTTON_UPDATE_EPISERVER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnUpdateEpiServerClick);
	Connect(ID_BUTTON_UPDATE_PLAYER3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnUpdatePlayer3Click);
	Connect(ID_BUTTON_UPDATE_PLAYER67,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnUpdatePlayer67Click);
	Connect(ID_BUTTON_UPDATE_CONTROLLER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnUpdateControllerClick);
	Connect(ID_BUTTON_UPDATE_LAUNCHER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnUpdateLauncherClick);
	Connect(ID_BUTTON_RESTART_SERVER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnRestartServerClick);
	Connect(ID_BUTTON_RESTART_OS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnRestartOSClick);
	Connect(ID_BUTTON_SHUTDOWN_OS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnShutdownOSClick);
	Connect(ID_BUTTON_SSH,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnSSHClick);
	Connect(ID_BUTTON_BACK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgSystem::OnbtnBackClick);
	*/


	Center();
	Move(wxPoint(0,0));
	SetSize(800,480);


	m_pstHostname->SetLabel(sHostname);
	Connect(wxID_ANY, wxEVT_R_REPLY, (wxObjectEventFunction)&dlgSystem::OnRestfulReply);

	m_client.Get((m_sUrl+STR_ENDPOINTS[UPDATE]).ToStdString(), UPDATE);

}

dlgSystem::~dlgSystem()
{
	//(*Destroy(dlgSystem)
	//*)
}



void dlgSystem::OnbtnUpdateEpiServerClick(wxCommandEvent& event)
{
    UpdateApp("episerver");

}

void dlgSystem::UpdateApp(const wxString& sApp)
{
    dlgUpload aDlg(this, m_pstHostname->GetLabel(), m_sIpAddress);
    aDlg.PutApp(sApp);
    m_client.Get((m_sUrl+STR_ENDPOINTS[UPDATE]).ToStdString(), UPDATE);

}


void dlgSystem::OnbtnUpdatePlayer3Click(wxCommandEvent& event)
{
    UpdateApp("player3");
}

void dlgSystem::OnbtnUpdateControllerClick(wxCommandEvent& event)
{
    dlgUsb aDlg(this, m_pstHostname->GetLabel(), "cartcontroller");
    if(aDlg.ShowModal() == wxID_OK)
    {
        UsbChecker::UnmountDevice();
        if(UsbChecker::MountDevice(aDlg.m_sSelectedDevice) == 0)
        {
            //get the current directory
            char buffer[256];
            int nBytes = readlink("/proc/self/exe", buffer, 256);
            if(nBytes > 0)
            {
                buffer[nBytes] = '\0';
                //rename the controller app
                int nResult = rename(buffer, "/tmp/cartcontroller.old");

                std::cout << buffer << std::endl;

                //copy the new app in place of the controller app
                std::ifstream source(aDlg.m_sSelectedFile, std::ios::binary);
                std::ofstream dest(buffer, std::ios::binary);
                if(source && dest)
                {
                    dest << source.rdbuf();
                    source.close();
                    dest.close();

                    //set the x
                    chmod(buffer, 0755);


                    //tell launcher to restart all
                    //@todo restart
                    std::cout << "command:restart_all" << std::endl;

                }
                else
                {
                    Json::Value jsError;
                    jsError["reason"] = Json::Value(Json::arrayValue);
                    if(!source)
                    {
                        jsError["reason"].append("Couldn't open source file to read");
                    }
                    if(!dest)
                    {
                        jsError["reason"].append("Couldn't open destination file to write");
                    }
                    jsError["reason"].append(aDlg.m_sSelectedFile.ToStdString());
                    jsError["reason"].append(buffer);

                    ShowError("Can't update controller!", jsError);
                }
            }
            else
            {
                Json::Value jsError;
                jsError["reason"].append("Couldn't find running app!");
                jsError["reason"].append(strerror(errno));
                ShowError("Can't update controller!", jsError);
            }
            UsbChecker::UnmountDevice();
        }
        else
        {
            Json::Value jsError;
            jsError["reason"].append("Couldn't mount device!");
            jsError["reason"].append(aDlg.m_sSelectedDevice.ToStdString());
            ShowError("Can't update controller!", jsError);
        }
    }
}

void dlgSystem::OnbtnUpdatePlayer67Click(wxCommandEvent& event)
{
    UpdateApp("player67");
}


void dlgSystem::OnbtnRestartServerClick(wxCommandEvent& event)
{
    std::string sEndpoint = (m_sUrl+STR_ENDPOINTS[POWER]).ToStdString();
    std::string sCommand = "{ \"command\": \"restart server\"}";
    m_client.Put(sEndpoint, sCommand.c_str(), POWER);
}

void dlgSystem::OnbtnRestartOSClick(wxCommandEvent& event)
{
    std::string sEndpoint = (m_sUrl+STR_ENDPOINTS[POWER]).ToStdString();
    std::string sCommand = "{ \"command\": \"restart os\"}";
    m_client.Put(sEndpoint, sCommand.c_str(), POWER);
}

void dlgSystem::OnbtnShutdownOSClick(wxCommandEvent& event)
{
   std::string sEndpoint = (m_sUrl+STR_ENDPOINTS[POWER]).ToStdString();
    std::string sCommand = "{ \"command\": \"shutdown\"}";
    m_client.Put(sEndpoint, sCommand.c_str(), POWER);
}

void dlgSystem::OnbtnShutdownControllerClick(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void dlgSystem::OnbtnSSHClick(wxCommandEvent& event)
{

    wxExecute(wxString::Format("xterm -maximized -e 'ssh %s'", m_sIpAddress.Before(':').c_str()));
}

void dlgSystem::OnbtnBackClick(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}


void dlgSystem::OnRestfulReply(const wxCommandEvent &event)
{
    ShowConnectedButtons(true);
    Json::Value jsValue(ConvertToJson(event.GetString().ToStdString()));

    switch(event.GetInt())
    {
        case UPDATE:
            VersionReply(jsValue);
            break;
        case POWER:
            PowerReply(jsValue);

    }
}

void dlgSystem::ShowStringValue(const Json::Value& jsData, const std::string& sKey, wmLabel* pLabel)
{
    if(jsData[sKey].isString())
    {
        pLabel->SetLabel(wxString::FromUTF8(jsData[sKey].asString().c_str()));
    }
    else
    {
        pLabel->SetLabel(wxEmptyString);
    }
}

void dlgSystem::ShowIntValue(const Json::Value& jsData, const std::string& sKey, wmLabel* pLabel)
{
    if(jsData[sKey].isInt())
    {
        pLabel->SetLabel(wxString::Format("%d", jsData[sKey].asInt()));
    }
    else
    {
        pLabel->SetLabel(wxEmptyString);
    }
}


void dlgSystem::VersionReply(const Json::Value& jsData)
{
    if(jsData["server"].isObject())
    {
        ShowStringValue(jsData["server"], "version", m_pstVersionEpiServer);
    }
    else
    {
        m_pstVersionEpiServer->SetLabel(wxEmptyString);
    }
    if(jsData["player3"].isObject())
    {
        ShowStringValue(jsData["player3"], "version", m_pstVersionPlayer3);
    }
    else
    {
        m_pstVersionPlayer3->SetLabel(wxEmptyString);
    }
    if(jsData["player67"].isObject())
    {
        ShowStringValue(jsData["player67"], "version", m_pstVersionPlayer67);
    }
    else
    {
        m_pstVersionPlayer67->SetLabel(wxEmptyString);
    }

    m_pstVersionController->SetLabel(wxString::Format("%d.%d.%d.%d", version::MAJOR, version::MINOR, version::PATCH, version::BUILD));
    //todo(@martim01) launcher
}

void dlgSystem::ShowError(wxString sMessage, const Json::Value& jsData)
{
    dlgError aDlg(this, sMessage, jsData);
    aDlg.ShowModal();
}


void dlgSystem::PowerReply(const Json::Value& jsData)
{
    if(jsData["result"].isBool() && jsData["result"] == false)
    {
        ShowError("Power command failed!", jsData);
    }
}


void dlgSystem::OnbtnInfoClick(const wxCommandEvent& event)
{
    dlgInfo aDlg(this, m_wsClient, m_pstHostname->GetLabel());
    aDlg.ShowModal();

}


void dlgSystem::ShowConnectedButtons(bool bConnected)
{
    m_pbtnUpdateController->Enable(bConnected);
    m_pbtnUpdateEpiServer->Enable(bConnected);
    m_pbtnUpdatePlayer3->Enable(bConnected);
    m_pbtnUpdatePlayer67->Enable(bConnected);

    m_pbtnRestartServer->Enable(bConnected);
    m_pbtnRestartOS->Enable(bConnected);
    m_pbtnShutdownOS->Enable(bConnected);

}

