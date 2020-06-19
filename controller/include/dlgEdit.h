#ifndef DLGEDIT_H
#define DLGEDIT_H

//(*Headers(dlgEdit)
#include "wmkeyboard.h"
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class dlgEdit: public wxDialog
{
	public:

		dlgEdit(wxWindow* parent, const wxString& sHostname, const wxString& sLabel, const wxString& sDescription, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgEdit();

		//(*Declarations(dlgEdit)
		wmKeyboard* m_pKbd1;
		wxButton* m_pbtnEditCancel;
		wxButton* m_pbtnEditUpdate;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* m_pstHostname;
		wxTextCtrl* m_pedtDescription;
		wxTextCtrl* m_pedtLabel;
		//*)

	protected:

		//(*Identifiers(dlgEdit)
		static const long ID_STATICTEXT30;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_M_PKBD1;
		static const long ID_BUTTON_EDIT_UPDATE;
		static const long ID_BUTTON_EDIT_CANCEL;
		//*)

	private:

		//(*Handlers(dlgEdit)
		void OnbtnEditUpdateClick(wxCommandEvent& event);
		void OnbtnEditCancelClick(wxCommandEvent& event);
		void OnedtLabelText(wxCommandEvent& event);
		void OnedtDescriptionText(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
