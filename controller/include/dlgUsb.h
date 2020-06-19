#ifndef DLGUSB_H
#define DLGUSB_H

//(*Headers(dlgUsb)
#include "wmlist.h"
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class dlgUsb: public wxDialog
{
	public:

		dlgUsb(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgUsb();

		//(*Declarations(dlgUsb)
		wmList* m_pLst1;
		wxButton* m_pbtnCancel;
		wxButton* m_pbtnUpload;
		wxStaticText* m_pstHostname;
		//*)

	protected:

		//(*Identifiers(dlgUsb)
		static const long ID_STATICTEXT30;
		static const long ID_M_PLST1;
		static const long ID_BUTTON_UPLOAD;
		static const long ID_BUTTON_CANCEL;
		//*)

	private:

		//(*Handlers(dlgUsb)
		void OnbtnCancelClick(wxCommandEvent& event);
		void OnbtnUploadClick(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
