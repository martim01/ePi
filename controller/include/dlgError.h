#ifndef DLGERROR_H
#define DLGERROR_H

//(*Headers(dlgError)
#include "wmlist.h"
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)
#include "wmbutton.h"
#include "wmlabel.h"
#include "json/json.h"

class dlgError: public wxDialog
{
	public:

		dlgError(wxWindow* parent,const wxString& sMessage, const Json::Value& jsError, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgError();

        wmList* m_plstReasons;
		wmButton* m_pbtnClose;
		wxPanel* panel1;
		wmLabel* m_plblMessage;


	protected:

		//(*Identifiers(dlgError)
		static const long ID_STATICTEXT1;
		static const long ID_M_PLST1;
		static const long ID_BUTTON1;
		static const long ID_PANEL1;
		//*)

	private:

		//(*Handlers(dlgError)
		void OnbtnCloseClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
