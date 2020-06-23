#include "../include/dlgUsb.h"
#include "jsonutils.h"
//(*InternalHeaders(dlgUsb)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(dlgUsb)
const long dlgUsb::ID_STATICTEXT30 = wxNewId();
const long dlgUsb::ID_M_PLST1 = wxNewId();
const long dlgUsb::ID_BUTTON_UPLOAD = wxNewId();
const long dlgUsb::ID_BUTTON_CANCEL = wxNewId();
//*)

BEGIN_EVENT_TABLE(dlgUsb,wxDialog)
	//(*EventTable(dlgUsb)
	//*)
END_EVENT_TABLE()

dlgUsb::dlgUsb(wxWindow* parent, const wxString& sFilename,wxWindowID id,const wxPoint& pos,const wxSize& size) :
m_sFilename(sFilename),
m_checker(this)
{
	//(*Initialize(dlgUsb)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetBackgroundColour(wxColour(0,0,0));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	m_pstHostname = new wmLabel(this, ID_STATICTEXT30, _("Label"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT30"));
	m_pstHostname->SetMinSize(wxSize(800,40));
	m_pstHostname->SetForegroundColour(wxColour(255,255,255));
	m_pstHostname->SetBackgroundColour(wxColour(0,128,64));
	m_pstHostname->SetTextAlign(wxALIGN_CENTER);
	wxFont m_pstHostnameFont(24,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	m_pstHostname->SetFont(m_pstHostnameFont);
	BoxSizer1->Add(m_pstHostname, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

	m_plblUSB = new wmLabel(this, wxNewId(), _(""), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT30"));
	m_plblUSB->SetMinSize(wxSize(800,40));
	m_plblUSB->SetForegroundColour(wxColour(255,255,255));
	m_plblUSB->SetBackgroundColour(wxColour(0,0,0));
	m_plblUSB->SetTextAlign(wxALIGN_CENTER);
	m_plblUSB->SetFont(wxFont(20,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT));
	BoxSizer1->Add(m_plblUSB, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);

	m_plstFiles = new wmList(this, ID_M_PLST1, wxDefaultPosition, wxSize(800,300), 0, 0, wxSize(-1,-1), 6, wxSize(-1,-1));
	BoxSizer1->Add(m_plstFiles, 0, wxALL|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_pbtnUpload = new wmButton(this, ID_BUTTON_UPLOAD, _("Upload"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_UPLOAD"));
	BoxSizer2->Add(m_pbtnUpload, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_pbtnCancel = new wmButton(this, ID_BUTTON_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CANCEL"));
	BoxSizer2->Add(m_pbtnCancel, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BUTTON_UPLOAD,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgUsb::OnbtnUploadClick);
	Connect(ID_BUTTON_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgUsb::OnbtnCancelClick);
	//*)

	Connect(wxID_ANY, wxEVT_USB_FOUND, (wxObjectEventFunction)&dlgUsb::OnUsbFound);
	Connect(wxID_ANY, wxEVT_USB_FILE_FOUND, (wxObjectEventFunction)&dlgUsb::OnUsbFileFound);
	Connect(wxID_ANY, wxEVT_USB_FINISHED, (wxObjectEventFunction)&dlgUsb::OnUsbFinished);

	Center();
	Move(wxPoint(0,0));
	SetSize(800,480);

	CheckUSB();

}

dlgUsb::~dlgUsb()
{
	//(*Destroy(dlgUsb)
	//*)
}


void dlgUsb::OnbtnCancelClick(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void dlgUsb::OnbtnUploadClick(wxCommandEvent& event)
{
    //EndModal(wxID_OK);
    CheckUSB();
}


void dlgUsb::StartCheck()
{
    if(IsShownOnScreen())
    {
        CheckUSB();
    }
}

void dlgUsb::CheckUSB()
{
    m_plstFiles->Clear();

    m_plblUSB->SetLabel("Searching for USB drives...");

    m_checker.RunCheck(m_sFilename);

}


void dlgUsb::OnUsbFound(const wxCommandEvent& event)
{
    m_plblUSB->SetLabel(wxString::Format("Found USB drive '%s'. Searching for files...", event.GetString().c_str()));
}


void dlgUsb::OnUsbFileFound(const wxCommandEvent& event)
{
    Json::Value jsValue(ConvertToJson(event.GetString().ToStdString()));
    if(jsValue["Device"].isString() && jsValue["File"].isString())
    {
        m_plstFiles->AddButton(wxString::FromUTF8(jsValue["File"].asString().c_str()));
    }
}


void dlgUsb::OnUsbFinished(const wxCommandEvent& event)
{
    m_plblUSB->SetLabel("Finished searching.");
}
