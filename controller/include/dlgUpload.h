#ifndef DLGUPLOAD_H
#define DLGUPLOAD_H

//(*Headers(dlgUpload)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

#include "multipartupload.h"
#include "json/json.h"

class dlgUpload: public wxDialog
{
	public:

		dlgUpload(wxWindow* parent, const wxString& sIpAddress, const wxString& sEndpoint, const wxString& sFilename, const wxString& sFilepath, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgUpload();

		//(*Declarations(dlgUpload)
		wxButton* m_pbtnCancel;
		wxGauge* m_pGuage;
		wxStaticText* m_pstDetails;
		wxStaticText* m_pstHostname;
		//*)

		int ShowModal() override;

		Json::Value m_jsReply;

	protected:

		//(*Identifiers(dlgUpload)
		static const long ID_STATICTEXT30;
		static const long ID_STATICTEXT1;
		static const long ID_GAUGE1;
		static const long ID_BUTTON_CANCEL;
		//*)

	private:

		//(*Handlers(dlgUpload)
		void OnbtnCancelClick(wxCommandEvent& event);
		//*)
        void OnReply(const wxCommandEvent& event);
        void OnProgress(const wxCommandEvent& event);

		MultipartUpload m_upload;

		wxString m_sIpAddress;
		wxString m_sEndpoint;
		wxString m_sFilename;
		wxString m_sFilePath;

		DECLARE_EVENT_TABLE()
};

#endif
