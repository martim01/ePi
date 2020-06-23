#include "../include/dlgUpload.h"
#include "restfulclient.h"
#include "json/json.h"
#include "jsonutils.h"
#include <wx/msgdlg.h>
#include <wx/log.h>

//(*InternalHeaders(dlgUpload)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(dlgUpload)
const long dlgUpload::ID_STATICTEXT30 = wxNewId();
const long dlgUpload::ID_STATICTEXT1 = wxNewId();
const long dlgUpload::ID_CUSTOM1 = wxNewId();
const long dlgUpload::ID_STATICTEXT2 = wxNewId();
const long dlgUpload::ID_BUTTON_CANCEL = wxNewId();
//*)

BEGIN_EVENT_TABLE(dlgUpload,wxDialog)
	//(*EventTable(dlgUpload)
	//*)
END_EVENT_TABLE()

dlgUpload::dlgUpload(wxWindow* parent, const wxString& sHostname, const wxString& sIpAddress, const wxString& sEndpoint, int nMethod, const wxString& sFilename, const wxString& sFilepath, wxWindowID id,const wxPoint& pos,const wxSize& size) : m_upload(this),
    m_sIpAddress(sIpAddress),
    m_sEndpoint(sEndpoint),
    m_nMethod(nMethod),
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
	m_pstHostname = new wmLabel(this, ID_STATICTEXT30, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTER, _T("ID_STATICTEXT30"));
	m_pstHostname->SetMinSize(wxSize(800,40));
	m_pstHostname->SetForegroundColour(wxColour(255,255,255));
	m_pstHostname->SetBackgroundColour(wxColour(0,128,64));
	wxFont m_pstHostnameFont(24,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	m_pstHostname->SetFont(m_pstHostnameFont);
	BoxSizer1->Add(m_pstHostname, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_pstDetails = new wmLabel(this, ID_STATICTEXT1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTER, _T("ID_STATICTEXT1"));
	m_pstDetails->SetMinSize(wxSize(800,40));
	m_pstDetails->SetForegroundColour(wxColour(255,255,255));
	wxFont m_pstDetailsFont(24,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	m_pstDetails->SetFont(m_pstDetailsFont);
	BoxSizer1->Add(m_pstDetails, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_pGuage = new Progress(this,ID_CUSTOM1,wxDefaultPosition,wxDefaultSize,0);
	BoxSizer1->Add(m_pGuage, 0, wxLEFT|wxRIGHT|wxEXPAND, 20);
	m_pstProgress = new wmLabel(this, ID_STATICTEXT2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTER, _T("ID_STATICTEXT2"));
	m_pstProgress->SetMinSize(wxSize(800,40));
	m_pstProgress->SetForegroundColour(wxColour(0,152,230));
	wxFont m_pstProgressFont(14,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	m_pstProgress->SetFont(m_pstProgressFont);
	BoxSizer1->Add(m_pstProgress, 0, wxALL|wxEXPAND, 0);
	BoxSizer1->Add(-1,-1,1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_pbtnCancel = new wmButton(this, ID_BUTTON_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CANCEL"));
	BoxSizer1->Add(m_pbtnCancel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BUTTON_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgUpload::OnbtnCancelClick);
	//*)

    SetSize(800,480);
    Center();
	Move(wxPoint(0,0));

	//Connect(ID_BUTTON_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgUpload::OnbtnCancelClick);
    m_pstHostname->SetTextAlign(wxALIGN_CENTER);
    m_pstDetails->SetTextAlign(wxALIGN_CENTER);
    m_pstProgress->SetTextAlign(wxALIGN_CENTER);

    m_pstHostname->SetLabel(sHostname);
    m_pstDetails->SetForegroundColour(wxColour(0,152,230));
	m_pGuage->SetBackgroundColour(*wxBLACK);
	m_pGuage->SetForegroundColour(wxColour(0,152,230));



	Connect(wxID_ANY, wxEVT_R_REPLY, (wxObjectEventFunction)&dlgUpload::OnReply);
	Connect(wxID_ANY, wxEVT_R_PROGRESS, (wxObjectEventFunction)&dlgUpload::OnProgress);

	m_pstDetails->SetLabel(wxString::Format("Uploading '%s'...", m_sFilename.c_str()));
}

int dlgUpload::ShowModal()
{
    std::map<std::string,std::string> mData;
    std::map<std::string, muFile> mFiles;
    mFiles.insert(std::make_pair("file", muFile(muFileName(m_sFilename.ToStdString()), muFilePath(m_sFilePath.ToStdString()))));

    switch(m_nMethod)
    {
        case MultipartUpload::UPLOAD_PUT:
            m_upload.Put(m_sIpAddress.ToStdString(), m_sEndpoint.ToStdString(), mData, mFiles, 0);
            break;
        case MultipartUpload::UPLOAD_POST:
            m_upload.Post(m_sIpAddress.ToStdString(), m_sEndpoint.ToStdString(), mData, mFiles, 0);
            break;
    }


    return wxDialog::ShowModal();
}

dlgUpload::~dlgUpload()
{
	//(*Destroy(dlgUpload)
	//*)
}


void dlgUpload::OnbtnCancelClick(wxCommandEvent& event)
{
    m_upload.Cancel();
    m_jsReply["result"] = false;
    m_jsReply["reason"].append("User cancelled");
    EndModal(wxID_CANCEL);
}


void dlgUpload::OnReply(const wxCommandEvent& event)
{
    wxLogDebug("dglUpload::OnReply  %s", event.GetString().c_str());
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
    wxLogDebug("Upload: %d", event.GetInt());
    m_pGuage->SetValue(event.GetInt());
    m_pstProgress->SetLabel(wxString::Format("%03d%%", event.GetInt()));
}
