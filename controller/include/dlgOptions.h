#ifndef DLGOPTIONS_H
#define DLGOPTIONS_H

//(*Headers(dlgOptions)
#include "wmbutton.h"
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/progdlg.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
//*)
#include <string>
#include "json/json.h"
#include "wmlabel.h"
#include "response.h"

namespace pml
{
    namespace restgoose
    {
        class HttpClient;
    }
}

class wxWebSocketClient;
class dlgOptions: public wxDialog
{
	public:

		dlgOptions(wxWindow* parent, int nType, const endpoint& endWS, const wxString& sHostname, const wxString& sIpAddress, const wxString& sUrl, const std::string& sUid, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgOptions();

		wmButton* m_pbtnBack;
		wmButton* m_pbtnDelete;
		wmButton* m_pbtnReplace;
		wmButton* m_pbtnRestartOS;
		wmButton* m_pbtnRestartServer;
		wmButton* m_pbtnSSH;
		wmButton* m_pbtnShutdownOS;
		wmButton* m_pbtnStopController;
		wmButton* m_pbtnRestartAll;
		wmButton* m_pbtnUpdate;
		wmButton* m_pbtnUpdateController;
		wmButton* m_pbtnUpdateEpiServer;
		wmButton* m_pbtnUpdateLauncher;
		wmButton* m_pbtnUpdatePlayer3;
		wmButton* m_pbtnUpdatePlayer67;
		wxProgressDialog* m_pdlgProgress;
		wmLabel* StaticText10;
		wmLabel* StaticText11;
		wmLabel* StaticText12;
		wmLabel* StaticText13;
		wmLabel* StaticText14;
		wmLabel* StaticText15;
		wmLabel* StaticText16;
		wmLabel* StaticText1;
		wmLabel* StaticText2;
		wmLabel* StaticText3;
		wmLabel* StaticText4;
		wmLabel* StaticText5;
		wmLabel* StaticText7;
		wmLabel* StaticText9;
		wmLabel* m_pstChannels;
		wmLabel* m_pstCreated;
		wmLabel* m_pstDescription;
		wmLabel* m_pstHostname;
		wmLabel* m_pstLabel;
		wmLabel* m_pstLength;
		wmLabel* m_pstModified;
		wmLabel* m_pstSampleRate;
		wmLabel* m_pstSubType;
		wmLabel* m_pstType;
		wmLabel* m_pstUid;
		wmLabel* m_pstVersionController;
		wmLabel* m_pstVersionEpiServer;
		wmLabel* m_pstVersionLauncher;
		wmLabel* m_pstVersionPlayer3;
		wmLabel* m_pstVersionPlayer67;

		wmButton* m_pbtnInfo;

		enum {CONTROLLER, CART_FILE, CART_SYSTEM};

	protected:

		//(*Identifiers(dlgOptions)
		static const long ID_STATICTEXT30;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT6;
		static const long ID_BUTTON_UPDATE;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXT8;
		static const long ID_STATICTEXT9;
		static const long ID_STATICTEXT10;
		static const long ID_STATICLINE1;
		static const long ID_STATICTEXT11;
		static const long ID_STATICTEXT12;
		static const long ID_STATICTEXT13;
		static const long ID_STATICTEXT17;
		static const long ID_STATICTEXT14;
		static const long ID_STATICTEXT18;
		static const long ID_STATICTEXT15;
		static const long ID_STATICTEXT19;
		static const long ID_STATICTEXT20;
		static const long ID_STATICLINE2;
		static const long ID_BUTTON_REPLACE;
		static const long ID_BUTTON_DELETE;
		static const long ID_STATICTEXT16;
		static const long ID_STATICTEXT25;
		static const long ID_BUTTON_UPDATE_EPISERVER;
		static const long ID_STATICTEXT21;
		static const long ID_STATICTEXT26;
		static const long ID_BUTTON_UPDATE_PLAYER3;
		static const long ID_STATICTEXT22;
		static const long ID_STATICTEXT27;
		static const long ID_BUTTON_UPDATE_PLAYER67;
		static const long ID_STATICTEXT23;
		static const long ID_STATICTEXT28;
		static const long ID_BUTTON_UPDATE_CONTROLLER;
		static const long ID_STATICTEXT24;
		static const long ID_STATICTEXT29;
		static const long ID_BUTTON_UPDATE_LAUNCHER;
		static const long ID_BUTTON_RESTART_SERVER;
		static const long ID_BUTTON_RESTART_OS;
		static const long ID_BUTTON_SHUTDOWN_OS;
		static const long ID_BUTTON_SSH;
		static const long ID_BUTTON_BACK;
		static const long ID_PROGRESSDIALOG1;
		//*)

	private:

		//(*Handlers(dlgOptions)
		void OnbtnFileUpdateClick(wxCommandEvent& event);
		void OnbtnReplaceClick(wxCommandEvent& event);
		void OnbtnDeleteClick(wxCommandEvent& event);
		void OnbtnUpdateEpiServerClick(wxCommandEvent& event);
		void OnbtnUpdatePlayer3Click(wxCommandEvent& event);
		void OnbtnUpdateControllerClick(wxCommandEvent& event);
		void OnbtnUpdatePlayer67Click(wxCommandEvent& event);
		void OnbtnUpdateLauncherClick(wxCommandEvent& event);
		void OnbtnRestartServerClick(wxCommandEvent& event);
		void OnbtnRestartOSClick(wxCommandEvent& event);
		void OnbtnShutdownOSClick(wxCommandEvent& event);
		void OnbtnSSHClick(wxCommandEvent& event);
		void OnbtnBackClick(wxCommandEvent& event);
		//*)

		void OnbtnShutdownControllerClick(wxCommandEvent& event);
		void OnbtnRestartAllClick(const wxCommandEvent& event);
        void OnbtnInfoClick(const wxCommandEvent& event);

        void OnRestfulReply(const wxCommandEvent& event);

        void ShowConnectedButtons(bool bConnected);

        void GetFileDetails();
        void FilesReply(const Json::Value& jsData);
        void FileGetReply(const Json::Value& jsData);
        void FileUpdateReply(const Json::Value& jsData);
        void VersionReply(const Json::Value& jsData);
        void FileDeleteReply(const Json::Value& jsData);
        void PowerReply(const Json::Value& jsData);
        void ShowFileDetails(const Json::Value& jsData);

        void ShowStringValue(const Json::Value& jsData, const std::string& sKey, wmLabel* pLabel);
        void ShowIntValue(const Json::Value& jsData, const std::string& sKey, wmLabel* pLabel);

        void ShowError(wxString sMessage, const Json::Value& jsData);

        void UpdateApp(const wxString& sApp);


        int m_nType;
        endpoint m_endpointWS;
        wxString m_sIpAddress;
        wxString m_sUrl;
        std::string m_sUid;


		DECLARE_EVENT_TABLE()
};

#endif
