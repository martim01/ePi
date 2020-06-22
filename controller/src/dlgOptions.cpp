#include "dlgOptions.h"
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
#include "dlgupload.h"

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

dlgOptions::dlgOptions(wxWindow* parent,  const wxString& sHostname, const wxString& sIpAddress, const wxString& sUrl, const std::string& sUid, wxWindowID id,const wxPoint& pos,const wxSize& size) :
    m_sIpAddress(sIpAddress),
    m_sUrl(sUrl),
    m_sUid(sUid),
    m_client(this)

{
	//(*Initialize(dlgOptions)
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
	SetBackgroundColour(wxColour(0,0,0));
	wxFont thisFont(12,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	SetFont(thisFont);
	BoxSizer0 = new wxBoxSizer(wxVERTICAL);
	m_pstHostname = new wxStaticText(this, ID_STATICTEXT30, _("Label"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT30"));
	m_pstHostname->SetMinSize(wxSize(800,40));
	m_pstHostname->SetForegroundColour(wxColour(255,255,255));
	m_pstHostname->SetBackgroundColour(wxColour(0,128,64));
	wxFont m_pstHostnameFont(24,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	m_pstHostname->SetFont(m_pstHostnameFont);
	BoxSizer0->Add(m_pstHostname, 0, wxALL|wxEXPAND, 2);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, wxEmptyString);
	GridBagSizer1 = new wxGridBagSizer(0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("UID:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticText1->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(StaticText1, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstUid = new wxStaticText(this, ID_STATICTEXT2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT2"));
	m_pstUid->SetMinSize(wxSize(400,-1));
	m_pstUid->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	m_pstUid->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(m_pstUid, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT3, _("Label:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	StaticText2->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(StaticText2, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstLabel = new wxStaticText(this, ID_STATICTEXT4, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT4"));
	m_pstLabel->SetForegroundColour(wxColour(0,0,0));
	m_pstLabel->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(m_pstLabel, wxGBPosition(1, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT5, _("Description:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticText3->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(StaticText3, wxGBPosition(2, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstDescription = new wxStaticText(this, ID_STATICTEXT6, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT6"));
	m_pstDescription->SetForegroundColour(wxColour(0,0,0));
	m_pstDescription->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(m_pstDescription, wxGBPosition(2, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
	m_pbtnUpdate = new wxButton(this, ID_BUTTON_UPDATE, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE"));
	GridBagSizer1->Add(m_pbtnUpdate, wxGBPosition(1, 2), wxGBSpan(2, 1), wxALL, 2);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT7, _("Created:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	StaticText5->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(StaticText5, wxGBPosition(3, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstCreated = new wxStaticText(this, ID_STATICTEXT8, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT8"));
	m_pstCreated->SetForegroundColour(wxColour(0,0,0));
	m_pstCreated->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(m_pstCreated, wxGBPosition(3, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT9, _("Modified:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	StaticText7->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(StaticText7, wxGBPosition(4, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstModified = new wxStaticText(this, ID_STATICTEXT10, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT10"));
	m_pstModified->SetForegroundColour(wxColour(0,0,0));
	m_pstModified->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(m_pstModified, wxGBPosition(4, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
	StaticBoxSizer1->Add(GridBagSizer1, 1, wxALL|wxEXPAND, 2);
	StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	StaticBoxSizer1->Add(StaticLine1, 0, wxALL|wxEXPAND, 1);
	GridBagSizer2 = new wxGridBagSizer(0, 0);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT11, _("Channels:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	StaticText4->SetForegroundColour(wxColour(0,164,164));
	GridBagSizer2->Add(StaticText4, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstChannels = new wxStaticText(this, ID_STATICTEXT12, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	m_pstChannels->SetMinSize(wxSize(25,-1));
	m_pstChannels->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer2->Add(m_pstChannels, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	StaticText10 = new wxStaticText(this, ID_STATICTEXT13, _("Sample Rate:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT13"));
	StaticText10->SetForegroundColour(wxColour(0,164,164));
	GridBagSizer2->Add(StaticText10, wxGBPosition(0, 2), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstSampleRate = new wxStaticText(this, ID_STATICTEXT17, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT17"));
	m_pstSampleRate->SetMinSize(wxSize(50,-1));
	m_pstSampleRate->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer2->Add(m_pstSampleRate, wxGBPosition(0, 3), wxDefaultSpan, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	StaticText11 = new wxStaticText(this, ID_STATICTEXT14, _("Format:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT14"));
	StaticText11->SetForegroundColour(wxColour(0,164,164));
	GridBagSizer2->Add(StaticText11, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstType = new wxStaticText(this, ID_STATICTEXT18, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT18"));
	m_pstType->SetMinSize(wxSize(50,-1));
	m_pstType->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer2->Add(m_pstType, wxGBPosition(1, 1), wxDefaultSpan, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	StaticText12 = new wxStaticText(this, ID_STATICTEXT15, _("Length:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT15"));
	StaticText12->SetForegroundColour(wxColour(0,164,164));
	GridBagSizer2->Add(StaticText12, wxGBPosition(0, 4), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstSubType = new wxStaticText(this, ID_STATICTEXT19, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT19"));
	m_pstSubType->SetMinSize(wxSize(150,-1));
	m_pstSubType->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer2->Add(m_pstSubType, wxGBPosition(1, 2), wxGBSpan(1, 3), wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstLength = new wxStaticText(this, ID_STATICTEXT20, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT20"));
	m_pstLength->SetMinSize(wxSize(75,-1));
	m_pstLength->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer2->Add(m_pstLength, wxGBPosition(0, 5), wxDefaultSpan, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 2);
	StaticBoxSizer1->Add(GridBagSizer2, 0, wxALL|wxEXPAND, 2);
	StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
	StaticBoxSizer1->Add(StaticLine2, 0, wxALL|wxEXPAND, 2);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_pbtnReplace = new wxButton(this, ID_BUTTON_REPLACE, _("Replace"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_REPLACE"));
	BoxSizer2->Add(m_pbtnReplace, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	BoxSizer2->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnDelete = new wxButton(this, ID_BUTTON_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DELETE"));
	BoxSizer2->Add(m_pbtnDelete, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticBoxSizer1->Add(BoxSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 0);
	BoxSizer1->Add(StaticBoxSizer1, 1, wxLEFT|wxEXPAND, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, wxEmptyString);
	GridBagSizer3 = new wxGridBagSizer(0, 0);
	StaticText9 = new wxStaticText(this, ID_STATICTEXT16, _("episerver:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT16"));
	StaticText9->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText9, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionEpiServer = new wxStaticText(this, ID_STATICTEXT25, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT25"));
	m_pstVersionEpiServer->SetMinSize(wxSize(100,-1));
	m_pstVersionEpiServer->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionEpiServer->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionEpiServer, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnUpdateEpiServer = new wxButton(this, ID_BUTTON_UPDATE_EPISERVER, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_EPISERVER"));
	GridBagSizer3->Add(m_pbtnUpdateEpiServer, wxGBPosition(0, 2), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText13 = new wxStaticText(this, ID_STATICTEXT21, _("player3:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT21"));
	StaticText13->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText13, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionPlayer3 = new wxStaticText(this, ID_STATICTEXT26, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT26"));
	m_pstVersionPlayer3->SetMinSize(wxSize(100,-1));
	m_pstVersionPlayer3->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionPlayer3->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionPlayer3, wxGBPosition(1, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnUpdatePlayer3 = new wxButton(this, ID_BUTTON_UPDATE_PLAYER3, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_PLAYER3"));
	GridBagSizer3->Add(m_pbtnUpdatePlayer3, wxGBPosition(1, 2), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText14 = new wxStaticText(this, ID_STATICTEXT22, _("player67:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT22"));
	StaticText14->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText14, wxGBPosition(1, 3), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionPlayer67 = new wxStaticText(this, ID_STATICTEXT27, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT27"));
	m_pstVersionPlayer67->SetMinSize(wxSize(100,-1));
	m_pstVersionPlayer67->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionPlayer67->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionPlayer67, wxGBPosition(1, 4), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnUpdatePlayer67 = new wxButton(this, ID_BUTTON_UPDATE_PLAYER67, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_PLAYER67"));
	GridBagSizer3->Add(m_pbtnUpdatePlayer67, wxGBPosition(1, 5), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText15 = new wxStaticText(this, ID_STATICTEXT23, _("controller:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT23"));
	StaticText15->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText15, wxGBPosition(2, 0), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionController = new wxStaticText(this, ID_STATICTEXT28, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT28"));
	m_pstVersionController->SetMinSize(wxSize(100,-1));
	m_pstVersionController->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionController->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionController, wxGBPosition(2, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnUpdateController = new wxButton(this, ID_BUTTON_UPDATE_CONTROLLER, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_CONTROLLER"));
	GridBagSizer3->Add(m_pbtnUpdateController, wxGBPosition(2, 2), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	StaticText16 = new wxStaticText(this, ID_STATICTEXT24, _("launcher:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT24"));
	StaticText16->SetForegroundColour(wxColour(255,0,128));
	GridBagSizer3->Add(StaticText16, wxGBPosition(2, 3), wxDefaultSpan, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 2);
	m_pstVersionLauncher = new wxStaticText(this, ID_STATICTEXT29, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSTATIC_BORDER, _T("ID_STATICTEXT29"));
	m_pstVersionLauncher->SetMinSize(wxSize(100,-1));
	m_pstVersionLauncher->SetForegroundColour(wxColour(0,0,0));
	m_pstVersionLauncher->SetBackgroundColour(wxColour(255,255,255));
	GridBagSizer3->Add(m_pstVersionLauncher, wxGBPosition(2, 4), wxDefaultSpan, wxALL|wxALIGN_CENTER_VERTICAL, 2);
	m_pbtnUpdateLauncher = new wxButton(this, ID_BUTTON_UPDATE_LAUNCHER, _("Update"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPDATE_LAUNCHER"));
	GridBagSizer3->Add(m_pbtnUpdateLauncher, wxGBPosition(2, 5), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticBoxSizer2->Add(GridBagSizer3, 1, wxALL|wxEXPAND, 0);
	BoxSizer1->Add(StaticBoxSizer2, 0, wxBOTTOM|wxLEFT|wxEXPAND, 5);
	BoxSizer3->Add(BoxSizer1, 1, wxALL|wxEXPAND, 0);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, wxEmptyString);
	m_pbtnRestartServer = new wxButton(this, ID_BUTTON_RESTART_SERVER, _("Restart Server"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_RESTART_SERVER"));
	m_pbtnRestartServer->SetForegroundColour(wxColour(255,255,255));
	m_pbtnRestartServer->SetBackgroundColour(wxColour(128,0,0));
	StaticBoxSizer3->Add(m_pbtnRestartServer, 1, wxALL|wxEXPAND, 5);
	m_pbtnRestartOS = new wxButton(this, ID_BUTTON_RESTART_OS, _("Restart OS"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_RESTART_OS"));
	m_pbtnRestartOS->SetForegroundColour(wxColour(255,255,255));
	m_pbtnRestartOS->SetBackgroundColour(wxColour(128,0,0));
	StaticBoxSizer3->Add(m_pbtnRestartOS, 1, wxALL|wxEXPAND, 5);
	m_pbtnShutdownOS = new wxButton(this, ID_BUTTON_SHUTDOWN_OS, _("Shutdown OS"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_SHUTDOWN_OS"));
	m_pbtnShutdownOS->SetForegroundColour(wxColour(255,255,255));
	m_pbtnShutdownOS->SetBackgroundColour(wxColour(128,0,0));
	StaticBoxSizer3->Add(m_pbtnShutdownOS, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer3->Add(-1,-1,2, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_pbtnSSH = new wxButton(this, ID_BUTTON_SSH, _("SSH"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_SSH"));
	m_pbtnSSH->SetForegroundColour(wxColour(255,255,255));
	m_pbtnSSH->SetBackgroundColour(wxColour(68,1,158));
	StaticBoxSizer3->Add(m_pbtnSSH, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer3->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_pbtnBack = new wxButton(this, ID_BUTTON_BACK, _("Back"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BACK"));
	m_pbtnBack->SetForegroundColour(wxColour(255,255,255));
	m_pbtnBack->SetBackgroundColour(wxColour(0,128,0));
	StaticBoxSizer3->Add(m_pbtnBack, 1, wxALL|wxEXPAND, 5);
	BoxSizer3->Add(StaticBoxSizer3, 0, wxBOTTOM|wxRIGHT|wxEXPAND, 5);
	BoxSizer0->Add(BoxSizer3, 1, wxALL|wxEXPAND, 0);
	SetSizer(BoxSizer0);
	BoxSizer0->Fit(this);
	BoxSizer0->SetSizeHints(this);

	Connect(ID_BUTTON_UPDATE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnFileUpdateClick);
	Connect(ID_BUTTON_REPLACE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnReplaceClick);
	Connect(ID_BUTTON_DELETE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnDeleteClick);
	Connect(ID_BUTTON_UPDATE_EPISERVER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdateEpiServerClick);
	Connect(ID_BUTTON_UPDATE_PLAYER3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdatePlayer3Click);
	Connect(ID_BUTTON_UPDATE_PLAYER67,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdatePlayer67Click);
	Connect(ID_BUTTON_UPDATE_CONTROLLER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdateControllerClick);
	Connect(ID_BUTTON_UPDATE_LAUNCHER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdateLauncherClick);
	Connect(ID_BUTTON_RESTART_SERVER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnRestartServerClick);
	Connect(ID_BUTTON_RESTART_OS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnRestartOSClick);
	Connect(ID_BUTTON_SHUTDOWN_OS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnShutdownOSClick);
	Connect(ID_BUTTON_SSH,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnSSHClick);
	Connect(ID_BUTTON_BACK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnBackClick);
	//*)

	/*
	Connect(ID_BUTTON_UPDATE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnFileUpdateClick);
	Connect(ID_BUTTON_REPLACE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnReplaceClick);
	Connect(ID_BUTTON_DELETE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnDeleteClick);
	Connect(ID_BUTTON_UPDATE_EPISERVER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdateEpiServerClick);
	Connect(ID_BUTTON_UPDATE_PLAYER3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdatePlayer3Click);
	Connect(ID_BUTTON_UPDATE_PLAYER67,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdatePlayer67Click);
	Connect(ID_BUTTON_UPDATE_CONTROLLER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdateControllerClick);
	Connect(ID_BUTTON_UPDATE_LAUNCHER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnUpdateLauncherClick);
	Connect(ID_BUTTON_RESTART_SERVER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnRestartServerClick);
	Connect(ID_BUTTON_RESTART_OS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnRestartOSClick);
	Connect(ID_BUTTON_SHUTDOWN_OS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnShutdownOSClick);
	Connect(ID_BUTTON_SSH,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnSSHClick);
	Connect(ID_BUTTON_BACK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgOptions::OnbtnBackClick);
	*/

	GridBagSizer1->AddGrowableCol(1);
	GridBagSizer1->AddGrowableRow(2);

	SetPosition(wxPoint(0,0));
	SetSize(800,480);


	m_pstHostname->SetLabel(sHostname);
	Connect(wxID_ANY, wxEVT_R_REPLY, (wxObjectEventFunction)&dlgOptions::OnRestfulReply);

	GetFileDetails();

	m_client.Get((m_sUrl+STR_ENDPOINTS[UPDATE]).ToStdString(), UPDATE);

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

        m_client.Patch(sEndpoint, ssData.str().c_str(), FILE_UPDATE);

    }
}


void dlgOptions::OnbtnReplaceClick(wxCommandEvent& event)
{
    //@todo(martim01) Replace File
    //dlgUsb aDlg(this);
    //aDlg.ShowModal();


    wxString sEndpoint("/x-epi/"+STR_ENDPOINTS[FILES]+"/");
    sEndpoint += wxString::FromUTF8(m_sUid.c_str());


    wxString sFilename="palace.wav";
    wxString sFilepath = "C:\\Users\\marti\\Desktop\\Peter Hammill\\";

    dlgUpload aDlg(this, m_sIpAddress, sEndpoint, sFilename, sFilepath);
    aDlg.ShowModal();
    FileUpdateReply(aDlg.m_jsReply);

}

void dlgOptions::OnbtnDeleteClick(wxCommandEvent& event)
{
    if(wxMessageBox("Really delete the file?", "ePi Controller", wxYES_NO) == wxYES)
    {
        std::string sEndpoint((m_sUrl+STR_ENDPOINTS[FILES]+"/").ToStdString());
        sEndpoint += m_sUid;
        m_client.Delete(sEndpoint, FILE_DELETE);
    }
}

void dlgOptions::OnbtnUpdateEpiServerClick(wxCommandEvent& event)
{
    //@todo(martim01) Update EpiServer
}

void dlgOptions::OnbtnUpdatePlayer3Click(wxCommandEvent& event)
{
    //@todo(martim01) Update Player3
}

void dlgOptions::OnbtnUpdateControllerClick(wxCommandEvent& event)
{
    //@todo(martim01) Update Controller
}

void dlgOptions::OnbtnUpdatePlayer67Click(wxCommandEvent& event)
{
    //@todo(martim01) Update Player67
}

void dlgOptions::OnbtnUpdateLauncherClick(wxCommandEvent& event)
{
    //@todo(martim01) Update Launcher
}

void dlgOptions::OnbtnRestartServerClick(wxCommandEvent& event)
{
    if(wxMessageBox("Restart Server?", "ePi Controller", wxYES_NO) == wxYES)
    {
        std::string sEndpoint = (m_sUrl+STR_ENDPOINTS[POWER]).ToStdString();
        std::string sCommand = "{ \"command\": \"restart server\"}";
        m_client.Put(sEndpoint, sCommand.c_str(), POWER);
    }
}

void dlgOptions::OnbtnRestartOSClick(wxCommandEvent& event)
{
    if(wxMessageBox("Restart OS?", "ePi Controller", wxYES_NO) == wxYES)
    {
        std::string sEndpoint = (m_sUrl+STR_ENDPOINTS[POWER]).ToStdString();
        std::string sCommand = "{ \"command\": \"restart os\"}";
        m_client.Put(sEndpoint, sCommand.c_str(), POWER);
    }
}

void dlgOptions::OnbtnShutdownOSClick(wxCommandEvent& event)
{
    if(wxMessageBox("Shutdown OS?", "ePi Controller", wxYES_NO) == wxYES)
    {
        std::string sEndpoint = (m_sUrl+STR_ENDPOINTS[POWER]).ToStdString();
        std::string sCommand = "{ \"command\": \"shutdown\"}";
        m_client.Put(sEndpoint, sCommand.c_str(), POWER);
    }
}

void dlgOptions::OnbtnSSHClick(wxCommandEvent& event)
{
    //@todo(martim01) SSH
}

void dlgOptions::OnbtnBackClick(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}


void dlgOptions::OnRestfulReply(const wxCommandEvent &event)
{
    wxLogDebug(event.GetString());
    Json::Value jsValue(ConvertToJson(event.GetString().ToStdString()));

    switch(event.GetInt())
    {
        case FILE_GET:
            FileGetReply(jsValue);
            break;
        case FILE_UPDATE:
            FileUpdateReply(jsValue);
        case FILE_REPLACE:
            FileUpdateReply(jsValue);
            break;
        case FILE_DELETE:
            FileDeleteReply(jsValue);
            break;
        case FILES:
            FilesReply(jsValue);
            break;
        case UPDATE:
            VersionReply(jsValue);
            break;
        case POWER:
            PowerReply(jsValue);

    }
}

void dlgOptions::ShowStringValue(const Json::Value& jsData, const std::string& sKey, wxStaticText* pLabel)
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

void dlgOptions::ShowIntValue(const Json::Value& jsData, const std::string& sKey, wxStaticText* pLabel)
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
    if(jsData["result"].isBool() && jsData["result"] == false)
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
    for(size_t i = 0; i < jsData["reason"].size(); i++)
    {
        if(jsData["reason"][i].isString())
        {
            sMessage += "\n";
            sMessage += wxString::FromUTF8(jsData["reason"][i].asString().c_str());
        }
    }
    wxMessageBox(sMessage);
}

void dlgOptions::FileDeleteReply(const Json::Value& jsData)
{
    if(jsData["result"].isBool() && jsData["result"] == false)
    {
        ShowError("Unable to delete file!", jsData);
    }
    else
    {
        ShowFileDetails(jsData);    //clear the file details
        m_client.Get((m_sUrl+STR_ENDPOINTS[FILES]).ToStdString(), FILES);   //see if any other files on the server
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
	    m_client.Get((m_sUrl+STR_ENDPOINTS[FILES]+"/").ToStdString()+m_sUid, FILE_GET);
	    m_pbtnDelete->Show();
	    m_pbtnReplace->SetLabel("Replace");
	}
	else
    {
        m_pbtnDelete->Hide();
	    m_pbtnReplace->SetLabel("Upload");
    }
}


void dlgOptions::PowerReply(const Json::Value& jsData)
{
    if(jsData["result"].isBool() && jsData["result"] == false)
    {
        ShowError("Power command failed!", jsData);
    }
}


