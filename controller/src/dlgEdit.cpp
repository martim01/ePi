#include "../include/dlgEdit.h"

//(*InternalHeaders(dlgEdit)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(dlgEdit)
const long dlgEdit::ID_STATICTEXT30 = wxNewId();
const long dlgEdit::ID_STATICTEXT1 = wxNewId();
const long dlgEdit::ID_TEXTCTRL1 = wxNewId();
const long dlgEdit::ID_STATICTEXT2 = wxNewId();
const long dlgEdit::ID_TEXTCTRL2 = wxNewId();
const long dlgEdit::ID_M_PKBD1 = wxNewId();
const long dlgEdit::ID_BUTTON_EDIT_UPDATE = wxNewId();
const long dlgEdit::ID_BUTTON_EDIT_CANCEL = wxNewId();
//*)

BEGIN_EVENT_TABLE(dlgEdit,wxDialog)
	//(*EventTable(dlgEdit)
	//*)
END_EVENT_TABLE()

dlgEdit::dlgEdit(wxWindow* parent, const wxString& sHostname, const wxString& sLabel, const wxString& sDescription, wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	wxBoxSizer* BoxSizer1;
	wxGridBagSizer* GridBagSizer1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetBackgroundColour(wxColour(0,0,0));
	wxFont thisFont(12,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	SetFont(thisFont);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	m_pstHostname = new wmLabel(this, ID_STATICTEXT30, _("Label"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE, _T("ID_STATICTEXT30"));
	m_pstHostname->SetMinSize(wxSize(800,40));
	m_pstHostname->SetForegroundColour(wxColour(255,255,255));
	m_pstHostname->SetBackgroundColour(wxColour(0,128,64));
	wxFont m_pstHostnameFont(24,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT);
	m_pstHostname->SetFont(m_pstHostnameFont);
	BoxSizer1->Add(m_pstHostname, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	GridBagSizer1 = new wxGridBagSizer(0, 0);
	StaticText1 = new wmLabel(this, ID_STATICTEXT1, _("Label:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticText1->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(StaticText1, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_pedtLabel = new wmEdit(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(150,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	m_pedtLabel->SetValidation(0);
	m_pedtLabel->SetBorderStyle(0,0);
	GridBagSizer1->Add(m_pedtLabel, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
	StaticText2 = new wmLabel(this, ID_STATICTEXT2, _("Description:"), wxDefaultPosition, wxSize(150,-1), 0, _T("ID_STATICTEXT2"));
	StaticText2->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(StaticText2, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	m_pedtDescription = new wmEdit(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	m_pedtDescription->SetValidation(0);
	m_pedtDescription->SetBorderStyle(0,0);
	GridBagSizer1->Add(m_pedtDescription, wxGBPosition(1, 1), wxDefaultSpan, wxALL|wxEXPAND, 2);
	m_pKbd1 = new wmKeyboard(this, ID_M_PKBD1, wxDefaultPosition, wxDefaultSize, 0, 0);
	m_pKbd1->SetForegroundColour(wxColour(255,255,255));
	GridBagSizer1->Add(m_pKbd1, wxGBPosition(2, 0), wxGBSpan(3, 2), wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_pbtnEditUpdate = new wmButton(this, ID_BUTTON_EDIT_UPDATE, _("Update"), wxDefaultPosition, wxSize(100,50), 0, wxDefaultValidator, _T("ID_BUTTON_EDIT_UPDATE"));
	GridBagSizer1->Add(m_pbtnEditUpdate, wxGBPosition(2, 2), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_pbtnEditCancel = new wmButton(this, ID_BUTTON_EDIT_CANCEL, _("Cancel"), wxDefaultPosition, wxSize(100,50), 0, wxDefaultValidator, _T("ID_BUTTON_EDIT_CANCEL"));
	GridBagSizer1->Add(m_pbtnEditCancel, wxGBPosition(3, 2), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(GridBagSizer1, 0, wxALL|wxEXPAND, 2);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BUTTON_EDIT_UPDATE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgEdit::OnbtnEditUpdateClick);
	Connect(ID_BUTTON_EDIT_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&dlgEdit::OnbtnEditCancelClick);


	Center();
    Move(wxPoint(0,0));
	SetSize(800,480);

	m_pstHostname->SetTextAlign(wxALIGN_CENTER);
	m_pstHostname->SetLabel(sHostname);
    m_pedtLabel->SetValue(sLabel);
    m_pedtDescription->SetValue(sDescription);

    m_pbtnEditUpdate->Enable((m_pedtLabel->GetValue().empty() == false && m_pedtDescription->GetValue().empty() == false));

    m_pedtLabel->SetFocus();

}

dlgEdit::~dlgEdit()
{
	//(*Destroy(dlgEdit)
	//*)
}


void dlgEdit::OnbtnEditUpdateClick(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void dlgEdit::OnbtnEditCancelClick(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void dlgEdit::OnedtLabelText(wxCommandEvent& event)
{
    m_pbtnEditUpdate->Enable((m_pedtLabel->GetValue().empty() == false && m_pedtDescription->GetValue().empty() == false));
}

void dlgEdit::OnedtDescriptionText(wxCommandEvent& event)
{
    m_pbtnEditUpdate->Enable((m_pedtLabel->GetValue().empty() == false && m_pedtDescription->GetValue().empty() == false));
}
