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

class dlgError: public wxDialog
{
	public:

		dlgError(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgError();

		//(*Declarations(dlgError)
		wmList* m_plstReasons;
		wxButton* m_pbtnClose;
		wxPanel* panel1;
		wxStaticText* StaticText1;
		//*)

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
