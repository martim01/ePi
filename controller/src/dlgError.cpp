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

dlgError::dlgError(wxWindow* parent,const wxString& sMessage, const Json::Value& jsError, wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;


	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, _T("id"));


	SetClientSize(wxSize(300,300));
	Move(wxDefaultPosition);
	SetBackgroundColour(wxColour(255,0,0));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	m_plblMessage = new wmLabel(this, ID_STATICTEXT1, sMessage, wxDefaultPosition, wxSize(300,40), 0, _T("ID_STATICTEXT1"));
    m_plblMessage->SetBackgroundColour(*wxBLACK);
    m_plblMessage->SetForegroundColour(*wxWHITE);
    m_plblMessage->SetTextAlign(wxALIGN_CENTER);
    m_plblMessage->SetFont(wxFont(16,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Arial"),wxFONTENCODING_DEFAULT));

	BoxSizer1->Add(m_plblMessage, 0, wxALL|wxEXPAND, 2);
	m_plstReasons = new wmList(this, ID_M_PLST1, wxDefaultPosition, wxSize(300,200), 0, 0, wxSize(-1,25), 1, wxSize(1,1));
	m_plstReasons->SetBackgroundColour(wxColour(255,255,255));
	m_plstReasons->SetButtonColour(*wxWHITE);
	m_plstReasons->SetTextButtonColour(*wxBLACK);
	m_plstReasons->SetTextAlign(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
	BoxSizer1->Add(m_plstReasons, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	panel1->SetBackgroundColour(*wxWHITE);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_pbtnClose = new wmButton(panel1, ID_BUTTON1, _("Close"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(m_pbtnClose, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	panel1->SetSizer(BoxSizer2);
	BoxSizer2->Fit(panel1);
	BoxSizer2->SetSizeHints(panel1);
	BoxSizer1->Add(panel1, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 2);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	SetSize(300,300);
	Center();

	Connect(m_pbtnClose->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&dlgError::OnbtnCloseClick);

	if(jsError["reason"].isArray())
	{
        for(size_t i = 0; i < jsError["reason"].size(); i++)
        {
            if(jsError["reason"][i].isString())
            {
                m_plstReasons->AddButton(wxString::FromUTF8(jsError["reason"][i].asString().c_str()));
            }
        }
    }
	m_plstReasons->Refresh();
}

dlgError::~dlgError()
{
	//(*Destroy(dlgError)
	//*)
}


void dlgError::OnbtnCloseClick(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}
