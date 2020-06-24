#ifndef DLGUPLOAD_H
#define DLGUPLOAD_H

//(*Headers(dlgUpload)
#include "progress.h"
#include "wmbutton.h"
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

#include "multipartupload.h"
#include "json/json.h"
#include "wmlabel.h"
#include <map>
#include <string>
#include <wx/timer.h>

class dlgUpload: public wxDialog
{
	public:

		dlgUpload(wxWindow* parent, const wxString& sHostname, const wxString& sIpAddress, const wxString& sEndpoint, int nMethod, const wxString& sFilename,
		const wxString& sFilepath, const wxString& sDevice, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);

		void SetMulitpartTextData(const std::map<std::string, std::string>& mData);
		virtual ~dlgUpload();

		//(*Declarations(dlgUpload)
		Progress* m_pGuage;
		wmButton* m_pbtnCancel;
		wmLabel* m_pstDetails;
		wmLabel* m_pstHostname;
		wmLabel* m_pstProgress;
		//*)

		int ShowModal() override;

		Json::Value m_jsReply;

	protected:

		//(*Identifiers(dlgUpload)
		static const long ID_STATICTEXT30;
		static const long ID_STATICTEXT1;
		static const long ID_CUSTOM1;
		static const long ID_STATICTEXT2;
		static const long ID_BUTTON_CANCEL;
		//*)
		wxTimer m_timer;

	private:

		//(*Handlers(dlgUpload)
		void OnbtnCancelClick(wxCommandEvent& event);
		//*)
        void OnReply(const wxCommandEvent& event);
        void OnProgress(const wxCommandEvent& event);
        void OnTimer(const wxTimerEvent& event);
        bool MountDevice();

		MultipartUpload m_upload;

		wxString m_sIpAddress;
		wxString m_sEndpoint;
		int m_nMethod;
		wxString m_sFilename;
		wxString m_sFilePath;
		wxString m_sDevice;

		std::map<std::string, std::string> m_mData;

		DECLARE_EVENT_TABLE()
};

#endif
