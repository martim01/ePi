#include "../include/dlgUpload.h"
#include "restfulclient.h"
#include "json/json.h"
#include "jsonutils.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(dlgUpload)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(dlgUpload)
const long dlgUpload::ID_STATICTEXT30 = wxNewId();
const long dlgUpload::ID_STATICTEXT1 = wxNewId();
const long dlgUpload::ID_GAUGE1 = wxNewId();
const long dlgUpload::ID_BUTTON_CANCEL = wxNewId();
//*)

BEGIN_EVENT_TABLE(dlgUpload,wxDialog)
	//(*EventTable(dlgUpload)
	//*)
END_EVENT_TABLE()

dlgUpload::dlgUpload(wxWindow* parent, const wxString& sIpAddress, const wxString& sEndpoint, const wxString& sFilename, const wxString& sFilepath, wxWindowID id,const wxPoint& pos,const wxSize& size) : m_upload(this),
    m_sIpAddress(sIpAddress),
    m_sEndpoint(sEndpoint),
    m_sFilename(sFilename),
    m_sFilePath(sFilepath)
{
	//(*Initialize(dlgUpload)
	wxBoxSizer* BoxSizer1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetBackgroundColour(wxColour(0,0,0));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	m_pstHostname = new wxStaticText(this, ID_STATICTEXT30, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTER, _T("ID_STATICTEXT30"));
	m_pstHostname->SetMinSize(wxSize(800,40));
	m_pstHostname->SetForegroundColour(wxColour(255,255,255));
	m_pstHostname->SetBackgroundColour(wxColour(0,128,64));
	wxFont m_pstHostnameFont(24,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	m_pstHostname->SetFont(m_pstHostnameFont);
	BoxSizer1->Add(m_pstHostname, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	BoxSizer1->Add(0,0,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_pstDetails = new wxStaticText(this, ID_STATICTEXT1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTER, _T("ID_STATICTEXT1"));
	m_pstDetails->SetMinSize(wxSize(800,40));
	m_pstDetails->SetForegroundColour(wxColour(255,255,255));
	wxFont m_pstDetailsFont(24,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	m_pstDetails->SetFont(m_pstDetailsFont);
	BoxSizer1->Add(m_pstDetails, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_pGuage = new wxGauge(this, ID_GAUGE1, 100, wxDefaultPosition, wxSize(-1,40), wxGA_SMOOTH, wxDefaultValidator, _T("ID_GAUGE1"));
	m_pGuage->SetForegroundColour(wxColour(0,128,192));
	m_pGuage->SetBackgroundColour(wxColour(0,0,0));
	BoxSizer1->Add(m_pGuage, 0, wxLEFT|wxRIGHT|wxEXPAND, 20);
	BoxSizer1->Add(0,0,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_pbtnCancel = new wxButton(this, ID_BUTTON_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CANCEL"));
	BoxSizer1->Add(m_pbtnCancel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
	SetSize(800,480);
	SetPosition(wxPoint(0,0));

	Connect(wxID_ANY, wxEVT_R_REPLY, (wxObjectEventFunction)&dlgUpload::OnReply);
	Connect(wxID_ANY, wxEVT_R_PROGRESS, (wxObjectEventFunction)&dlgUpload::OnProgress);

	m_pstDetails->SetLabel(wxString::Format("Uploading %s...", m_sFilename.c_str()));
}

int dlgUpload::ShowModal()
{
    std::map<std::string,std::string> mData;
    std::map<std::string, muFile> mFiles;
    mFiles.insert(std::make_pair("file", muFile(muFileName(m_sFilename.ToStdString()), muFilePath(m_sFilePath.ToStdString()))));

    m_upload.Put(m_sIpAddress.ToStdString(), m_sEndpoint.ToStdString(), mData, mFiles, 0);

    wxDialog::ShowModal();
}

dlgUpload::~dlgUpload()
{
	//(*Destroy(dlgUpload)
	//*)
}


void dlgUpload::OnbtnCancelClick(wxCommandEvent& event)
{
    m_upload.Cancel();
}


void dlgUpload::OnReply(const wxCommandEvent& event)
{
    m_jsReply = ConvertToJson(event.GetString().ToStdString());
    if(m_jsReply["result"].isBool() && m_jsReply["result"].asBool() == false)
    {
        EndModal(wxID_CANCEL);
    }
    else
    {
        EndModal(wxID_OK);
    }
}

void dlgUpload::OnProgress(const wxCommandEvent& event)
{
    m_pGuage->SetValue(event.GetInt());
}
