#include "dlgOptions.h"
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
//#include <sys/capability.h>
#include "dlgInfo.h"
#include "response.h"
#include <fstream>
#include "wxwebsocketclient.h"
//(*InternalHeaders(dlgOptions)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(dlgOptions)
const long dlgOptions::ID_STATICTEXT30 = wxNewId();
const long dlgOptions::ID_STATICTEXT1 = wxNewId();
const long dlgOptions::ID_STATICTEXT2 = wxNewId();
const long dlgOptions::ID_STATICTEXT3 = wxNewId();
const long dlgOptions::ID_STATICTEXT4 = wxNewId();
const long dlgOptions::ID_STATICTEXT5 = wxNewId();
const long dlgOptions::ID_STATICTEXT6 = wxNewId();
const long dlgOptions::ID_BUTTON_UPDATE = wxNewId();
const long dlgOptions::ID_STATICTEXT7 = wxNewId();
const long dlgOptions::ID_STATICTEXT8 = wxNewId();
const long dlgOptions::ID_STATICTEXT9 = wxNewId();
const long dlgOptions::ID_STATICTEXT10 = wxNewId();
const long dlgOptions::ID_STATICLINE1 = wxNewId();
const long dlgOptions::ID_STATICTEXT11 = wxNewId();
const long dlgOptions::ID_STATICTEXT12 = wxNewId();
const long dlgOptions::ID_STATICTEXT13 = wxNewId();
const long dlgOptions::ID_STATICTEXT17 = wxNewId();
const long dlgOptions::ID_STATICTEXT14 = wxNewId();
const long dlgOptions::ID_STATICTEXT18 = wxNewId();
const long dlgOptions::ID_STATICTEXT15 = wxNewId();
const long dlgOptions::ID_STATICTEXT19 = wxNewId();
const long dlgOptions::ID_STATICTEXT20 = wxNewId();
const long dlgOptions::ID_STATICLINE2 = wxNewId();
const long dlgOptions::ID_BUTTON_REPLACE = wxNewId();
const long dlgOptions::ID_BUTTON_DELETE = wxNewId();
const long dlgOptions::ID_STATICTEXT16 = wxNewId();
const long dlgOptions::ID_STATICTEXT25 = wxNewId();
const long dlgOptions::ID_BUTTON_UPDATE_EPISERVER = wxNewId();
const long dlgOptions::ID_STATICTEXT21 = wxNewId();
const long dlgOptions::ID_STATICTEXT26 = wxNewId();
const long dlgOptions::ID_BUTTON_UPDATE_PLAYER3 = wxNewId();
const long dlgOptions::ID_STATICTEXT22 = wxNewId();
const long dlgOptions::ID_STATICTEXT27 = wxNewId();
const long dlgOptions::ID_BUTTON_UPDATE_PLAYER67 = wxNewId();
const long dlgOptions::ID_STATICTEXT23 = wxNewId();
const long dlgOptions::ID_STATICTEXT28 = wxNewId();
const long dlgOptions::ID_BUTTON_UPDATE_CONTROLLER = wxNewId();
const long dlgOptions::ID_STATICTEXT24 = wxNewId();
const long dlgOptions::ID_STATICTEXT29 = wxNewId();
const long dlgOptions::ID_BUTTON_UPDATE_LAUNCHER = wxNewId();
const long dlgOptions::ID_BUTTON_RESTART_SERVER = wxNewId();
const long dlgOptions::ID_BUTTON_RESTART_OS = wxNewId();
const long dlgOptions::ID_BUTTON_SHUTDOWN_OS = wxNewId();
const long dlgOptions::ID_BUTTON_SSH = wxNewId();
const long dlgOptions::ID_BUTTON_BACK = wxNewId();
const long dlgOptions::ID_PROGRESSDIALOG1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(dlgOptions,wxDialog)
	//(*EventTable(dlgOptions)
	//*)
END_EVENT_TABLE()

dlgOptions::dlgOptions(wxWindow* parent, int nType, wxWebSocketClient& wsClient, const wxString& sHostname, const wxString& sIpAddress, const wxString& sUrl, const std::string& sUid, wxWindowID id,const wxPoint& pos,const wxSize& size) :
    m_nType(nType),
    m_wsClient(wsClient),
    m_sIpAddress(sIpAddress),
    m_sUrl(sUrl),
    m_sUid(sUid)
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
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	if(m_nType != CART_SYSTEM)
	{
        StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, wxEmptyString);
        GridBagSizer1 = new wxGridBagSizer(0, 0);
        StaticText1 = new wmLabel(this, ID_STATICTEXT1, _("UID:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
        StaticText1->SetForegroundColour(wxColour(0,150,100));
        GridBagSizer1->Add(StaticText1, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
        m_pstUid = new wmLabel(this, ID_STATICTEXT2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT2"));
        m_pstUid->SetMinSize(wxSize(400,-1));
        m_pstUid->SetForegroundColour(*wxBLACK);
        m_pstUid->SetBackgroundColour(wxColour(255,255,255));
        GridBagSizer1->Add(m_pstUid, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
        StaticText2 = new wmLabel(this, ID_STATICTEXT3, _("Label:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
        StaticText2->SetForegroundColour(wxColour(0,150,100));
        GridBagSizer1->Add(StaticText2, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
        m_pstLabel = new wmLabel(this, ID_STATICTEXT4, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT4"));
        m_pstLabel->SetForegroundColour(wxColour(0,0,0));
        m_pstLabel->SetBackgroundColour(wxColour(255,255,255));
        GridBagSizer1->Add(m_pstLabel, wxGBPosition(1, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
        StaticText3 = new wmLabel(this, ID_STATICTEXT5, _("Description:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
        StaticText3->SetForegroundColour(wxColour(0,150,100));
        StaticText3->SetMinSize(wxSize(80,-1));
        GridBagSizer1->Add(StaticText3, wxGBPosition(2, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
        m_pstDescription = new wmLabel(this, ID_STATICTEXT6, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT6"));
        m_pstDescription->SetForegroundColour(wxColour(0,0,0));
        m_pstDescription->SetBackgroundColour(wxColour(255,255,255));
        GridBagSizer1->Add(m_pstDescription, wxGBPosition(2, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
        m_pbtnUpdate = new wmButton(this, ID_BUTTON_UPDATE, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE"));
        GridBagSizer1->Add(m_pbtnUpdate, wxGBPosition(1, 2), wxGBSpan(2, 1), wxALL, 2);
        StaticText5 = new wmLabel(this, ID_STATICTEXT7, _("Created:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
        StaticText5->SetForegroundColour(wxColour(0,150,100));
        GridBagSizer1->Add(StaticText5, wxGBPosition(3, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
        m_pstCreated = new wmLabel(this, ID_STATICTEXT8, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT8"));
        m_pstCreated->SetForegroundColour(wxColour(0,0,0));
        m_pstCreated->SetBackgroundColour(wxColour(255,255,255));
        GridBagSizer1->Add(m_pstCreated, wxGBPosition(3, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
        StaticText7 = new wmLabel(this, ID_STATICTEXT9, _("Modified:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
        StaticText7->SetForegroundColour(wxColour(0, 150,100));
        GridBagSizer1->Add(StaticText7, wxGBPosition(4, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
        m_pstModified = new wmLabel(this, ID_STATICTEXT10, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT10"));
        m_pstModified->SetForegroundColour(wxColour(0,0,0));
        m_pstModified->SetBackgroundColour(wxColour(255,255,255));
        GridBagSizer1->Add(m_pstModified, wxGBPosition(4, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
        StaticBoxSizer1->Add(GridBagSizer1, 1, wxALL|wxEXPAND, 2);
        GridBagSizer2 = new wxGridBagSizer(0, 0);
        StaticText4 = new wmLabel(this, ID_STATICTEXT11, _("Channels:"), wxDefaultPosition, wxSize(100,-1), 0, _T("ID_STATICTEXT11"));
        StaticText4->SetForegroundColour(wxColour(0,164,164));
        GridBagSizer2->Add(StaticText4, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
        m_pstChannels = new wmLabel(this, ID_STATICTEXT12, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
        m_pstChannels->SetMinSize(wxSize(50,-1));
        m_pstChannels->SetForegroundColour(*wxBLACK);
        m_pstChannels->SetBackgroundColour(*wxWHITE);
        GridBagSizer2->Add(m_pstChannels, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
        StaticText10 = new wmLabel(this, ID_STATICTEXT13, _("Sample Rate:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
        StaticText10->SetForegroundColour(wxColour(0,164,164));
        StaticText10->SetMinSize(wxSize(100,-1));
        GridBagSizer2->Add(StaticText10, wxGBPosition(0, 2), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
        m_pstSampleRate = new wmLabel(this, ID_STATICTEXT17, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
        m_pstSampleRate->SetMinSize(wxSize(50,-1));
        m_pstSampleRate->SetForegroundColour(*wxBLACK);
        m_pstSampleRate->SetBackgroundColour(*wxWHITE);
        GridBagSizer2->Add(m_pstSampleRate, wxGBPosition(0, 3), wxDefaultSpan, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
        StaticText11 = new wmLabel(this, ID_STATICTEXT14, _("Format:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
        StaticText11->SetForegroundColour(wxColour(0,164,164));
        GridBagSizer2->Add(StaticText11, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
        m_pstType = new wmLabel(this, ID_STATICTEXT18, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT18"));
        m_pstType->SetMinSize(wxSize(50,-1));
        m_pstType->SetForegroundColour(*wxBLACK);
        m_pstType->SetBackgroundColour(*wxWHITE);
        GridBagSizer2->Add(m_pstType, wxGBPosition(1, 1), wxGBSpan(1,1), wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
        StaticText12 = new wmLabel(this, ID_STATICTEXT15, _("Length:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
        StaticText12->SetForegroundColour(wxColour(0,164,164));
        GridBagSizer2->Add(StaticText12, wxGBPosition(0, 4), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
        m_pstSubType = new wmLabel(this, ID_STATICTEXT19, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT19"));
        m_pstSubType->SetMinSize(wxSize(150,-1));
        m_pstSubType->SetForegroundColour(*wxBLACK);
        m_pstSubType->SetBackgroundColour(*wxWHITE);
        GridBagSizer2->Add(m_pstSubType, wxGBPosition(1, 2), wxGBSpan(1, 2), wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
        m_pstLength = new wmLabel(this, ID_STATICTEXT20, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT20"));
        m_pstLength->SetMinSize(wxSize(100,-1));
        m_pstLength->SetForegroundColour(*wxBLACK);
        m_pstLength->SetBackgroundColour(*wxWHITE);

        GridBagSizer2->Add(m_pstLength, wxGBPosition(0, 5), wxDefaultSpan, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
        StaticBoxSizer1->Add(GridBagSizer2, 0, wxALL|wxEXPAND, 2);
        BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
        m_pbtnReplace = new wmButton(this, ID_BUTTON_REPLACE, _("Replace"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_REPLACE"));
        BoxSizer2->Add(m_pbtnReplace, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
        BoxSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
        m_pbtnDelete = new wmButton(this, ID_BUTTON_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, wmButton::STYLE_HOLD, wxDefaultValidator, _T("ID_BUTTON_DELETE"));
        BoxSizer2->Add(m_pbtnDelete, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
        StaticBoxSizer1->Add(BoxSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0);

        GridBagSizer1->AddGrowableCol(1);
        GridBagSizer1->AddGrowableRow(2);

        if(nType == CART_FILE)
        {
            BoxSizer3->Add(StaticBoxSizer1, 1, wxLEFT|wxBOTTOM|wxEXPAND, 5);
        }
        else
        {
            BoxSizer1->Add(StaticBoxSizer1, 1, wxLEFT|wxEXPAND, 5);
        }
	}

    StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, wxEmptyString);

    if(nType == CONTROLLER || nType == CART_SYSTEM)
	{

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
        m_pbtnUpdateLauncher = new wmButton(this, ID_BUTTON_UPDATE_LAUNCHER, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_LAUNCHER"));
        m_pbtnUpdateLauncher->Hide();
        GridBagSizer3->Add(m_pbtnUpdateLauncher, wxGBPosition(2, 5), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);

        m_pbtnInfo = new wmButton(this, wxNewId(), _("Server Info"), wxDefaultPosition, wxSize(150,-1), 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_LAUNCHER"));
        GridBagSizer3->Add(m_pbtnInfo, wxGBPosition(0, 3), wxGBSpan(1,2), wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);

        StaticBoxSizer2->Add(GridBagSizer3, 1, wxALL|wxEXPAND, 0);
        BoxSizer1->Add(StaticBoxSizer2, 0, wxBOTTOM|wxLEFT|wxEXPAND, 5);
        BoxSizer3->Add(BoxSizer1, 1, wxALL|wxEXPAND, 0);
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

        if(m_nType == CONTROLLER)
        {
            m_pbtnRestartAll = new wmButton(this, wxNewId(), _("Restart\nAll"), wxDefaultPosition, wxDefaultSize, wmButton::STYLE_HOLD, wxDefaultValidator, _T("ID_BUTTON_SHUTDOWN_OS"));
            m_pbtnRestartAll->SetForegroundColour(wxColour(255,255,255));
            m_pbtnRestartAll->SetBackgroundColour(wxColour(128,0,0));
            StaticBoxSizer3->Add(m_pbtnRestartAll, 1, wxALL|wxEXPAND,2);
        }
	}

    StaticBoxSizer3->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);


	if(m_nType != CART_FILE)
	{
        m_pbtnSSH = new wmButton(this, ID_BUTTON_SSH, _("SSH"), wxDefaultPosition, wxDefaultSize, wmButton::STYLE_HOLD, wxDefaultValidator, _T("ID_BUTTON_SSH"));
        m_pbtnSSH->SetForegroundColour(wxColour(255,255,255));
        m_pbtnSSH->SetBackgroundColour(wxColour(68,1,158));
        StaticBoxSizer3->Add(m_pbtnSSH, 1, wxALL|wxEXPAND,2);
        StaticBoxSizer3->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    }

	m_pbtnBack = new wmButton(this, ID_BUTTON_BACK, _("Back"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BACK"));
	m_pbtnBack->SetForegroundColour(wxColour(255,255,255));
	m_pbtnBack->SetBackgroundColour(wxColour(0,128,0));
	StaticBoxSizer3->Add(m_pbtnBack, 0, wxALL|wxEXPAND,2);
	BoxSizer3->Add(StaticBoxSizer3, 0, wxBOTTOM|wxRIGHT|wxEXPAND, 5);
	BoxSizer0->Add(BoxSizer3, 1, wxALL|wxEXPAND, 0);
	SetSizer(BoxSizer0);
	BoxSizer0->Fit(this);
	BoxSizer0->SetSizeHints(this);

	Connect(ID_BUTTON_BACK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnBackClick);

	if(m_nType != CART_SYSTEM)
	{
        Connect(ID_BUTTON_UPDATE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnFileUpdateClick);
        Connect(ID_BUTTON_REPLACE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnReplaceClick);
        Connect(ID_BUTTON_DELETE,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgOptions::OnbtnDeleteClick);
    }

	if(m_nType != CART_FILE)
	{
        Connect(ID_BUTTON_UPDATE_EPISERVER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdateEpiServerClick);
        Connect(ID_BUTTON_UPDATE_PLAYER3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdatePlayer3Click);
        Connect(ID_BUTTON_UPDATE_PLAYER67,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdatePlayer67Click);
        Connect(ID_BUTTON_UPDATE_CONTROLLER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdateControllerClick);
        Connect(ID_BUTTON_UPDATE_LAUNCHER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdateLauncherClick);
        Connect(ID_BUTTON_RESTART_SERVER,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgOptions::OnbtnRestartServerClick);
        Connect(ID_BUTTON_RESTART_OS,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgOptions::OnbtnRestartOSClick);
        Connect(ID_BUTTON_SHUTDOWN_OS,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgOptions::OnbtnShutdownOSClick);
        Connect(m_pbtnStopController->GetId(),wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgOptions::OnbtnShutdownControllerClick);
        Connect(ID_BUTTON_SSH,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgOptions::OnbtnSSHClick);
        Connect(m_pbtnInfo->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnInfoClick);

        if(m_nType == CONTROLLER)
        {
            Connect(m_pbtnRestartAll->GetId(),wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgOptions::OnbtnRestartAllClick);
        }
        m_pbtnUpdateController->SetBackgroundColour(wxColour(0,30,100));
	    m_pbtnUpdateEpiServer->SetBackgroundColour(wxColour(0,30,100));
	    m_pbtnUpdateLauncher->SetBackgroundColour(wxColour(0,30,100));
	    m_pbtnUpdatePlayer3->SetBackgroundColour(wxColour(0,30,100));
	    m_pbtnUpdatePlayer67->SetBackgroundColour(wxColour(0,30,100));
	    m_pbtnUpdateController->SetColourDisabled(wxColour(140,140,140));
	    m_pbtnUpdateEpiServer->SetColourDisabled(wxColour(140,140,140));
	    m_pbtnUpdatePlayer3->SetColourDisabled(wxColour(140,140,140));
	    m_pbtnUpdatePlayer67->SetColourDisabled(wxColour(140,140,140));
        m_pbtnRestartServer->SetColourDisabled(wxColour(140,140,140));
        m_pbtnRestartOS->SetColourDisabled(wxColour(140,140,140));
        m_pbtnShutdownOS->SetColourDisabled(wxColour(140,140,140));

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

	}
    m_pstHostname->SetTextAlign(wxALIGN_CENTER);

    if(m_nType != CART_SYSTEM)
	{
        m_pbtnUpdate->SetBackgroundColour(wxColour(128,128,0));
        m_pbtnReplace->SetBackgroundColour(wxColour(0,80,0));
        m_pbtnDelete->SetBackgroundColour(wxColour(0,80,0));
        m_pbtnUpdate->SetColourDisabled(wxColour(140,140,140));
        m_pbtnReplace->SetColourDisabled(wxColour(140,140,140));
        m_pbtnDelete->SetColourDisabled(wxColour(140,140,140));
        StaticText1->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
        StaticText2->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
        StaticText3->SetTextAlign(wxALIGN_TOP|wxALIGN_RIGHT);
        StaticText4->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
        StaticText5->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
        StaticText7->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
        StaticText10->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
        StaticText11->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);
        StaticText12->SetTextAlign(wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT);


        m_pstUid->SetTextAlign(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        m_pstCreated->SetTextAlign(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        m_pstLabel->SetTextAlign(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        m_pstDescription->SetTextAlign(wxALIGN_LEFT | wxALIGN_TOP);
        m_pstModified->SetTextAlign(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

        m_pstUid->SetBorderState(uiRect::BORDER_FLAT);
        m_pstCreated->SetBorderState(uiRect::BORDER_FLAT);
        m_pstLabel->SetBorderState(uiRect::BORDER_FLAT);
        m_pstDescription->SetBorderState(uiRect::BORDER_FLAT);
        m_pstModified->SetBorderState(uiRect::BORDER_FLAT);
        m_pstChannels->SetBorderState(uiRect::BORDER_FLAT);
        m_pstSampleRate->SetBorderState(uiRect::BORDER_FLAT);
        m_pstLength->SetBorderState(uiRect::BORDER_FLAT);
        m_pstType->SetBorderState(uiRect::BORDER_FLAT);
        m_pstSubType->SetBorderState(uiRect::BORDER_FLAT);

    }


	Center();
	Move(wxPoint(0,0));
	SetSize(800,480);


	m_pstHostname->SetLabel(sHostname);

	if(m_nType != CART_SYSTEM)
	{
        GetFileDetails();
    }

	if(m_nType != CART_FILE)
	{
        pml::restgoose::HttpClient client(pml::restgoose::GET, endpoint((m_sUrl+STR_ENDPOINTS[UPDATE]).ToStdString()));
        auto resp = client.Run();
        if(resp.nCode > 100)
        {
            ShowConnectedButtons(true);
            VersionReply(ConvertToJson(resp.data.Get()));
        }
    }

}

dlgOptions::~dlgOptions()
{
	//(*Destroy(dlgOptions)
	//*)
}


void dlgOptions::OnbtnFileUpdateClick(wxCommandEvent& event)
{
    dlgEdit aDlg(this, m_pstHostname->GetLabel(), m_pstLabel->GetLabel(), m_pstDescription->GetLabel());
    if(aDlg.ShowModal() == wxID_OK)
    {
        Json::Value jsData;
        jsData["label"] = aDlg.m_pedtLabel->GetValue().ToStdString();
        jsData["description"] = aDlg.m_pedtDescription->GetValue().ToStdString();
        std::stringstream ssData;
        epiWriter::Get().writeToSStream(jsData, ssData);

        std::string sEndpoint((m_sUrl+STR_ENDPOINTS[FILES]+"/").ToStdString());
        sEndpoint += m_sUid;

        pml::restgoose::HttpClient client(pml::restgoose::PATCH, endpoint(sEndpoint), textData(ssData.str()));
        FileUpdateReply(ConvertToJson(client.Run().data.Get()));
    }
}


void dlgOptions::OnbtnReplaceClick(wxCommandEvent& event)
{
    dlgUpload aDlg(this, m_pstHostname->GetLabel(), m_sIpAddress);
    if(m_pbtnReplace->GetLabel() == "Upload")
    {
        if(aDlg.PostAudioFile() == wxID_OK)
        {
            if(aDlg.m_jsReply.isMember("success") == false || aDlg.m_jsReply["success"].asBool() == false)
            {
                ShowError("Failed to upload file", aDlg.m_jsReply);
            }
            else if(aDlg.m_jsReply["uid"].isString())
            {
                m_sUid = aDlg.m_jsReply["uid"].asString();
                GetFileDetails();
            }
            else
            {
                ShowError("Uploaded but odd response", aDlg.m_jsReply);
            }
        }
    }
    else
    {
        if(aDlg.PutAudioFile(wxString::FromUTF8(m_sUid.c_str())) == wxID_OK)
        {
            FileUpdateReply(aDlg.m_jsReply);
        }

    }
}

void dlgOptions::OnbtnDeleteClick(wxCommandEvent& event)
{
    {
        std::string sEndpoint((m_sUrl+STR_ENDPOINTS[FILES]+"/").ToStdString());
        sEndpoint += m_sUid;

        pml::restgoose::HttpClient client(pml::restgoose::HTTP_DELETE, endpoint(sEndpoint));
        FileDeleteReply(ConvertToJson(client.Run().data.Get()));
    }
}

void dlgOptions::OnbtnUpdateEpiServerClick(wxCommandEvent& event)
{
    UpdateApp("episerver");

}

void dlgOptions::UpdateApp(const wxString& sApp)
{
    dlgUpload aDlg(this, m_pstHostname->GetLabel(), m_sIpAddress);
    aDlg.PutApp(sApp);

    pml::restgoose::HttpClient client(pml::restgoose::GET, endpoint((m_sUrl+STR_ENDPOINTS[UPDATE]).ToStdString()));
    VersionReply(ConvertToJson(client.Run().data.Get()));


}


void dlgOptions::OnbtnUpdatePlayer3Click(wxCommandEvent& event)
{
    UpdateApp("player3");
}

void dlgOptions::OnbtnUpdateControllerClick(wxCommandEvent& event)
{
    dlgUsb aDlg(this, m_pstHostname->GetLabel(), "controller");
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
                int nResult;

                if(m_nType == CONTROLLER)
                {
                    nResult = rename(buffer, "/tmp/controller.old");
                }
                else
                {
                    nResult = rename(buffer, "/tmp/cartcontroller.old");
                }


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

void dlgOptions::OnbtnUpdatePlayer67Click(wxCommandEvent& event)
{
    UpdateApp("player67");
}

void dlgOptions::OnbtnUpdateLauncherClick(wxCommandEvent& event)
{
    //@todo(martim01) Update Launcher
}

void dlgOptions::OnbtnRestartServerClick(wxCommandEvent& event)
{
    std::string sEndpoint = (m_sUrl+STR_ENDPOINTS[POWER]).ToStdString();
    std::string sCommand = "{ \"command\": \"restart server\"}";

    pml::restgoose::HttpClient client(pml::restgoose::PUT, endpoint(sEndpoint), textData(sCommand));
    PowerReply(ConvertToJson(client.Run().data.Get()));
}

void dlgOptions::OnbtnRestartOSClick(wxCommandEvent& event)
{
    std::string sEndpoint = (m_sUrl+STR_ENDPOINTS[POWER]).ToStdString();
    std::string sCommand = "{ \"command\": \"restart os\"}";

    pml::restgoose::HttpClient client(pml::restgoose::PUT, endpoint(sEndpoint), textData(sCommand));
    PowerReply(ConvertToJson(client.Run().data.Get()));

}

void dlgOptions::OnbtnShutdownOSClick(wxCommandEvent& event)
{
    std::string sEndpoint = (m_sUrl+STR_ENDPOINTS[POWER]).ToStdString();
    std::string sCommand = "{ \"command\": \"shutdown\"}";

    pml::restgoose::HttpClient client(pml::restgoose::PUT, endpoint(sEndpoint), textData(sCommand));
    PowerReply(ConvertToJson(client.Run().data.Get()));
}

void dlgOptions::OnbtnShutdownControllerClick(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void dlgOptions::OnbtnSSHClick(wxCommandEvent& event)
{

    wxExecute(wxString::Format("xterm -maximized -e 'ssh %s'", m_sIpAddress.Before(':').c_str()));
}

void dlgOptions::OnbtnBackClick(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}


void dlgOptions::ShowStringValue(const Json::Value& jsData, const std::string& sKey, wmLabel* pLabel)
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

void dlgOptions::ShowIntValue(const Json::Value& jsData, const std::string& sKey, wmLabel* pLabel)
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

void dlgOptions::FileGetReply(const Json::Value& jsData)
{

    ShowFileDetails(jsData);
}

void dlgOptions::FileUpdateReply(const Json::Value& jsData)
{
    if(jsData.isMember("success") == false || jsData["success"] == false)
    {
        ShowError("Failed to update file!", jsData);
    }

    ShowFileDetails(jsData);
}

void dlgOptions::ShowFileDetails(const Json::Value& jsData)
{
    ShowIntValue(jsData, "channels", m_pstChannels);
    ShowStringValue(jsData, "created", m_pstCreated);
    ShowStringValue(jsData, "file_modified", m_pstModified);
    ShowStringValue(jsData["format"], "type", m_pstType);
    ShowStringValue(jsData["format"], "subtype", m_pstSubType);
    ShowStringValue(jsData, "label", m_pstLabel);

    if(jsData["length"].isInt())
    {
        wxTimeSpan ts(0,0,0, jsData["length"].asInt());
        m_pstLength->SetLabel(ts.Format("%H:%M:%S:%l"));
    }
    else
    {
        m_pstLength->SetLabel(wxEmptyString);
    }

    ShowIntValue(jsData, "samplerate", m_pstSampleRate);
    ShowStringValue(jsData, "uid", m_pstUid);
    ShowStringValue(jsData, "description", m_pstDescription);
}


void dlgOptions::VersionReply(const Json::Value& jsData)
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

void dlgOptions::ShowError(wxString sMessage, const Json::Value& jsData)
{
    dlgError aDlg(this, sMessage, jsData);
    aDlg.ShowModal();
}

void dlgOptions::FileDeleteReply(const Json::Value& jsData)
{
    if(jsData["success"].isBool() && jsData["success"] == false)
    {
        ShowError("Unable to delete file!", jsData);
    }
    else
    {
        ShowFileDetails(jsData);    //clear the file details
        if(m_nType == CONTROLLER)
        {
            pml::restgoose::HttpClient client(pml::restgoose::GET, endpoint((m_sUrl+STR_ENDPOINTS[FILES]).ToStdString()));
            auto resp = client.Run();
            if(resp.nCode > 100)
            {
                FileGetReply(ConvertToJson(resp.data.Get()));
            }
        }
    }
}

void dlgOptions::FilesReply(const Json::Value& jsData)
{
    if(jsData.isArray() && jsData.size() > 0)
    {
        m_sUid = jsData[0]["uid"].asString();
    }
    else
    {
        m_sUid = "";
    }
    GetFileDetails();
}


void dlgOptions::GetFileDetails()
{
    if(m_sUid.empty() == false)
	{
	    std::string sEnd = (m_sUrl+STR_ENDPOINTS[FILES]+"/").ToStdString()+m_sUid;

	    pml::restgoose::HttpClient client(pml::restgoose::GET, endpoint(sEnd));
        FileGetReply(ConvertToJson(client.Run().data.Get()));

	    m_pbtnDelete->Show();
	    m_pbtnReplace->SetLabel("Replace");
	}
	else
    {
        m_pbtnDelete->Hide();
	    m_pbtnReplace->SetLabel("Upload");
	    m_pbtnUpdate->Show(false);
    }
}


void dlgOptions::PowerReply(const Json::Value& jsData)
{
    if(jsData["success"].isBool() && jsData["success"] == false)
    {
        ShowError("Power command failed!", jsData);
    }
}


void dlgOptions::OnbtnRestartAllClick(const wxCommandEvent& event)
{
    std::cout << "command:restart_all" << std::endl;
}

void dlgOptions::OnbtnInfoClick(const wxCommandEvent& event)
{
    dlgInfo aDlg(this, m_wsClient, m_pstHostname->GetLabel());
    aDlg.ShowModal();

}


void dlgOptions::ShowConnectedButtons(bool bConnected)
{
    if(m_nType != CART_SYSTEM)
    {
        m_pbtnUpdate->Enable(bConnected);
        m_pbtnReplace->Enable(bConnected);
        m_pbtnDelete->Enable(bConnected);
    }
    if(m_nType != CART_FILE)
    {
        m_pbtnUpdateController->Enable(bConnected);
        m_pbtnUpdateEpiServer->Enable(bConnected);
        m_pbtnUpdatePlayer3->Enable(bConnected);
        m_pbtnUpdatePlayer67->Enable(bConnected);
        m_pbtnRestartServer->Enable(bConnected);
        m_pbtnRestartOS->Enable(bConnected);
        m_pbtnShutdownOS->Enable(bConnected);
	}
}
