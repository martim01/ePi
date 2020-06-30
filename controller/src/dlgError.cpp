#include "../include/dlgError.h"

//(*InternalHeaders(dlgError)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(dlgError)
const long dlgError::ID_STATICTEXT1 = wxNewId();
const long dlgError::ID_M_PLST1 = wxNewId();
const long dlgError::ID_BUTTON1 = wxNewId();
const long dlgError::ID_PANEL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(dlgError,wxDialog)
	//(*EventTable(dlgError)
	//*)
END_EVENT_TABLE()

dlgError::dlgError(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(dlgError)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	SetBackgroundColour(wxColour(255,0,0));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Label"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(StaticText1, 0, wxALL|wxEXPAND, 2);
	m_plstReasons = new wmList(this, ID_M_PLST1, wxDefaultPosition, wxSize(300,300), 0, 0, wxSize(-1,25), 1, wxSize(1,1));
	m_plstReasons->SetBackgroundColour(wxColour(0,0,0));
	m_plstReasons->SetButtonColour(wxColour(wxT("#000000")));
	m_plstReasons->SetTextButtonColour(wxColour(wxT("#FFFFFF")));
	BoxSizer1->Add(m_plstReasons, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	panel1->SetBackgroundColour(wxColour(0,0,0));
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_pbtnClose = new wxButton(panel1, ID_BUTTON1, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(m_pbtnClose, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	panel1->SetSizer(BoxSizer2);
	BoxSizer2->Fit(panel1);
	BoxSizer2->SetSizeHints(panel1);
	BoxSizer1->Add(panel1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 2);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	//*)
}

dlgError::~dlgError()
{
	//(*Destroy(dlgError)
	//*)
}


void dlgError::OnbtnCloseClick(wxCommandEvent& event)
{
}
