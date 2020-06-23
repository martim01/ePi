#ifndef DLGUSB_H
#define DLGUSB_H

//(*Headers(dlgUsb)
#include "wmbutton.h"
#include "wmlist.h"
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)
#include "wmlabel.h"
#include <wx/timer.h>
#include <map>
#include "usbchecker.h"

class dlgUsb: public wxDialog
{
	public:

		dlgUsb(wxWindow* parent, const wxString& sFilename, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgUsb();

		//(*Declarations(dlgUsb)
		wmButton* m_pbtnCancel;
		wmButton* m_pbtnUpload;
		wmList* m_plstFiles;
		wmLabel* m_pstHostname;
		//*)
		wmLabel* m_plblUSB;

	protected:

		//(*Identifiers(dlgUsb)
		static const long ID_STATICTEXT30;
		static const long ID_M_PLST1;
		static const long ID_BUTTON_UPLOAD;
		static const long ID_BUTTON_CANCEL;
		//*)
		wxString m_sFilename;

	private:

		//(*Handlers(dlgUsb)
		void OnbtnCancelClick(wxCommandEvent& event);
		void OnbtnUploadClick(wxCommandEvent& event);
		//*)

		void OnUsbFound(const wxCommandEvent& event);
		void OnUsbFileFound(const wxCommandEvent& event);
		void OnUsbFinished(const wxCommandEvent& event);

		void StartCheck();
		void CheckUSB();

		UsbChecker m_checker;

		DECLARE_EVENT_TABLE()
};

#endif
