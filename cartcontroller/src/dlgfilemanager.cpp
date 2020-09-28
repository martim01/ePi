#include "dlgfilemanager.h"
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
//(*InternalHeaders(dlgFileManager)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(dlgFileManager)
const long dlgFileManager::ID_STATICTEXT30 = wxNewId();
const long dlgFileManager::ID_STATICTEXT1 = wxNewId();
const long dlgFileManager::ID_STATICTEXT2 = wxNewId();
const long dlgFileManager::ID_STATICTEXT3 = wxNewId();
const long dlgFileManager::ID_STATICTEXT4 = wxNewId();
const long dlgFileManager::ID_STATICTEXT5 = wxNewId();
const long dlgFileManager::ID_STATICTEXT6 = wxNewId();
const long dlgFileManager::ID_BUTTON_UPDATE = wxNewId();
const long dlgFileManager::ID_STATICTEXT7 = wxNewId();
const long dlgFileManager::ID_STATICTEXT8 = wxNewId();
const long dlgFileManager::ID_STATICTEXT9 = wxNewId();
const long dlgFileManager::ID_STATICTEXT10 = wxNewId();
const long dlgFileManager::ID_STATICLINE1 = wxNewId();
const long dlgFileManager::ID_STATICTEXT11 = wxNewId();
const long dlgFileManager::ID_STATICTEXT12 = wxNewId();
const long dlgFileManager::ID_STATICTEXT13 = wxNewId();
const long dlgFileManager::ID_STATICTEXT17 = wxNewId();
const long dlgFileManager::ID_STATICTEXT14 = wxNewId();
const long dlgFileManager::ID_STATICTEXT18 = wxNewId();
const long dlgFileManager::ID_STATICTEXT15 = wxNewId();
const long dlgFileManager::ID_STATICTEXT19 = wxNewId();
const long dlgFileManager::ID_STATICTEXT20 = wxNewId();
const long dlgFileManager::ID_STATICLINE2 = wxNewId();
const long dlgFileManager::ID_BUTTON_REPLACE = wxNewId();
const long dlgFileManager::ID_BUTTON_DELETE = wxNewId();
const long dlgFileManager::ID_STATICTEXT16 = wxNewId();
const long dlgFileManager::ID_STATICTEXT25 = wxNewId();
const long dlgFileManager::ID_BUTTON_UPDATE_EPISERVER = wxNewId();
const long dlgFileManager::ID_STATICTEXT21 = wxNewId();
const long dlgFileManager::ID_STATICTEXT26 = wxNewId();
const long dlgFileManager::ID_BUTTON_UPDATE_PLAYER3 = wxNewId();
const long dlgFileManager::ID_STATICTEXT22 = wxNewId();
const long dlgFileManager::ID_STATICTEXT27 = wxNewId();
const long dlgFileManager::ID_BUTTON_UPDATE_PLAYER67 = wxNewId();
const long dlgFileManager::ID_STATICTEXT23 = wxNewId();
const long dlgFileManager::ID_STATICTEXT28 = wxNewId();
const long dlgFileManager::ID_BUTTON_UPDATE_CONTROLLER = wxNewId();
const long dlgFileManager::ID_STATICTEXT24 = wxNewId();
const long dlgFileManager::ID_STATICTEXT29 = wxNewId();
const long dlgFileManager::ID_BUTTON_UPDATE_LAUNCHER = wxNewId();
const long dlgFileManager::ID_BUTTON_RESTART_SERVER = wxNewId();
const long dlgFileManager::ID_BUTTON_RESTART_OS = wxNewId();
const long dlgFileManager::ID_BUTTON_SHUTDOWN_OS = wxNewId();
const long dlgFileManager::ID_BUTTON_SSH = wxNewId();
const long dlgFileManager::ID_BUTTON_BACK = wxNewId();
const long dlgFileManager::ID_PROGRESSDIALOG1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(dlgFileManager,wxDialog)
	//(*EventTable(dlgFileManager)
	//*)
END_EVENT_TABLE()

dlgFileManager::dlgFileManager(wxWindow* parent, WebSocketClient& wsClient, const wxString& sHostname, const wxString& sIpAddress, const wxString& sUrl, const std::string& sUid, wxWindowID id,const wxPoint& pos,const wxSize& size) :
    m_wsClient(wsClient),
    m_sIpAddress(sIpAddress),
    m_sUrl(sUrl),
    m_sUid(sUid),
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
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
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
	BoxSizer3->Add(StaticBoxSizer1, 1, wxLEFT|wxBOTTOM|wxEXPAND, 5);

	StaticBoxSizer3 = new wxStaticBoxSizer(wxVERTICAL, this, wxEmptyString);

	StaticBoxSizer3->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);

    m_pbtnBack = new wmButton(this, ID_BUTTON_BACK, _("Back"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_BACK"));
	m_pbtnBack->SetForegroundColour(wxColour(255,255,255));
	m_pbtnBack->SetBackgroundColour(wxColour(0,128,0));
	StaticBoxSizer3->Add(m_pbtnBack, 0, wxALL|wxEXPAND,2);
	BoxSizer3->Add(StaticBoxSizer3, 0, wxBOTTOM|wxRIGHT|wxEXPAND, 5);
	BoxSizer0->Add(BoxSizer3, 1, wxALL|wxEXPAND, 0);
	SetSizer(BoxSizer0);
	BoxSizer0->Fit(this);
	BoxSizer0->SetSizeHints(this);

	Connect(ID_BUTTON_UPDATE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgFileManager::OnbtnFileUpdateClick);
	Connect(ID_BUTTON_REPLACE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgFileManager::OnbtnReplaceClick);
	Connect(ID_BUTTON_DELETE,wxEVT_BUTTON_HELD,(wxObjectEventFunction)&dlgFileManager::OnbtnDeleteClick);
	Connect(ID_BUTTON_BACK,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgFileManager::OnbtnBackClick);

	m_pbtnUpdate->SetBackgroundColour(wxColour(128,128,0));
    m_pbtnReplace->SetBackgroundColour(wxColour(0,80,0));
    m_pbtnDelete->SetBackgroundColour(wxColour(0,80,0));

    m_pbtnUpdate->SetColourDisabled(wxColour(140,140,140));
    m_pbtnReplace->SetColourDisabled(wxColour(140,140,140));
    m_pbtnDelete->SetColourDisabled(wxColour(140,140,140));


	m_pstHostname->SetTextAlign(wxALIGN_CENTER);
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

    ShowConnectedButtons(false);

	GridBagSizer1->AddGrowableCol(1);
	GridBagSizer1->AddGrowableRow(2);

	Center();
	Move(wxPoint(0,0));
	SetSize(800,480);


	m_pstHostname->SetLabel(sHostname);
	Connect(wxID_ANY, wxEVT_R_REPLY, (wxObjectEventFunction)&dlgFileManager::OnRestfulReply);

	GetFileDetails();

	m_client.Get((m_sUrl+STR_ENDPOINTS[UPDATE]).ToStdString(), UPDATE);

}

dlgFileManager::~dlgFileManager()
{
	//(*Destroy(dlgFileManager)
	//*)
}


void dlgFileManager::OnbtnFileUpdateClick(wxCommandEvent& event)
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


void dlgFileManager::OnbtnReplaceClick(wxCommandEvent& event)
{
    dlgUpload aDlg(this, m_pstHostname->GetLabel(), m_sIpAddress);
    if(m_pbtnReplace->GetLabel() == "Upload")
    {
        if(aDlg.PostAudioFile() == wxID_OK)
        {
            if(aDlg.m_jsReply["result"].asBool() == false)
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

void dlgFileManager::OnbtnDeleteClick(wxCommandEvent& event)
{
    {
        std::string sEndpoint((m_sUrl+STR_ENDPOINTS[FILES]+"/").ToStdString());
        sEndpoint += m_sUid;
        m_client.Delete(sEndpoint, FILE_DELETE);
    }
}


void dlgFileManager::OnbtnBackClick(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}


void dlgFileManager::OnRestfulReply(const wxCommandEvent &event)
{
    ShowConnectedButtons(true);
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
    }
}

void dlgFileManager::ShowStringValue(const Json::Value& jsData, const std::string& sKey, wmLabel* pLabel)
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

void dlgFileManager::ShowIntValue(const Json::Value& jsData, const std::string& sKey, wmLabel* pLabel)
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

void dlgFileManager::FileGetReply(const Json::Value& jsData)
{
    ShowFileDetails(jsData);
}

void dlgFileManager::FileUpdateReply(const Json::Value& jsData)
{
    if(jsData["result"].isBool() && jsData["result"] == false)
    {
        ShowError("Failed to update file!", jsData);
    }

    ShowFileDetails(jsData);
}

void dlgFileManager::ShowFileDetails(const Json::Value& jsData)
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



void dlgFileManager::ShowError(wxString sMessage, const Json::Value& jsData)
{
    dlgError aDlg(this, sMessage, jsData);
    aDlg.ShowModal();
}

void dlgFileManager::FileDeleteReply(const Json::Value& jsData)
{
    if(jsData["result"].isBool() && jsData["result"] == false)
    {
        ShowError("Unable to delete file!", jsData);
    }
    else
    {
        m_pstChannels->SetLabel(wxEmptyString);
		m_pstCreated->SetLabel(wxEmptyString);
		m_pstDescription->SetLabel(wxEmptyString);
		m_pstHostname->SetLabel(wxEmptyString);
		m_pstLabel->SetLabel(wxEmptyString);
		m_pstLength->SetLabel(wxEmptyString);
		m_pstModified->SetLabel(wxEmptyString);
		m_pstSampleRate->SetLabel(wxEmptyString);
		m_pstSubType->SetLabel(wxEmptyString);
		m_pstType->SetLabel(wxEmptyString);
		m_pstUid->SetLabel(wxEmptyString);
		m_pbtnDelete->Show(false);
        m_pbtnReplace->SetLabel("Upload");
        m_pbtnUpdate->Show(false);
    }
}



void dlgFileManager::GetFileDetails()
{
    if(m_sUid.empty() == false)
	{
	    m_client.Get((m_sUrl+STR_ENDPOINTS[FILES]+"/").ToStdString()+m_sUid, FILE_GET);
	    m_pbtnDelete->Show();
	    m_pbtnReplace->SetLabel("Replace");
	    m_pbtnUpdate->Show();
	}
	else
    {
        m_pbtnDelete->Hide();
	    m_pbtnReplace->SetLabel("Upload");
	    m_pbtnUpdate->Show(false);
    }
}



void dlgFileManager::ShowConnectedButtons(bool bConnected)
{
    m_pbtnUpdate->Enable(bConnected);
    m_pbtnReplace->Enable(bConnected);
    m_pbtnDelete->Enable(bConnected);

}

