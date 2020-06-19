#ifndef DLGOPTIONS_H
#define DLGOPTIONS_H

//(*Headers(dlgOptions)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
//*)
#include <string>
#include "json/json.h"
#include "restfulclient.h"

class dlgOptions: public wxDialog
{
	public:

		dlgOptions(wxWindow* parent, const wxString& sHostname, const wxString& sUrl, const std::string& sUid, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgOptions();

		//(*Declarations(dlgOptions)
		wxButton* m_pbtnBack;
		wxButton* m_pbtnDelete;
		wxButton* m_pbtnReplace;
		wxButton* m_pbtnRestartOS;
		wxButton* m_pbtnRestartServer;
		wxButton* m_pbtnSSH;
		wxButton* m_pbtnShutdownOS;
		wxButton* m_pbtnUpdate;
		wxButton* m_pbtnUpdateController;
		wxButton* m_pbtnUpdateEpiServer;
		wxButton* m_pbtnUpdateLauncher;
		wxButton* m_pbtnUpdatePlayer3;
		wxButton* m_pbtnUpdatePlayer67;
		wxStaticLine* StaticLine1;
		wxStaticLine* StaticLine2;
		wxStaticText* StaticText10;
		wxStaticText* StaticText11;
		wxStaticText* StaticText12;
		wxStaticText* StaticText13;
		wxStaticText* StaticText14;
		wxStaticText* StaticText15;
		wxStaticText* StaticText16;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText7;
		wxStaticText* StaticText9;
		wxStaticText* m_pstChannels;
		wxStaticText* m_pstCreated;
		wxStaticText* m_pstDescription;
		wxStaticText* m_pstHostname;
		wxStaticText* m_pstLabel;
		wxStaticText* m_pstLength;
		wxStaticText* m_pstModified;
		wxStaticText* m_pstSampleRate;
		wxStaticText* m_pstSubType;
		wxStaticText* m_pstType;
		wxStaticText* m_pstUid;
		wxStaticText* m_pstVersionController;
		wxStaticText* m_pstVersionEpiServer;
		wxStaticText* m_pstVersionLauncher;
		wxStaticText* m_pstVersionPlayer3;
		wxStaticText* m_pstVersionPlayer67;
		//*)

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

        void OnRestfulReply(const wxCommandEvent& event);

        void GetFileDetails();
        void FilesReply(const Json::Value& jsData);
        void FileGetReply(const Json::Value& jsData);
        void FileUpdateReply(const Json::Value& jsData);
        void VersionReply(const Json::Value& jsData);
        void FileDeleteReply(const Json::Value& jsData);
        void PowerReply(const Json::Value& jsData);
        void ShowFileDetails(const Json::Value& jsData);

        void ShowStringValue(const Json::Value& jsData, const std::string& sKey, wxStaticText* pLabel);
        void ShowIntValue(const Json::Value& jsData, const std::string& sKey, wxStaticText* pLabel);

        void ShowError(wxString sMessage, const Json::Value& jsData);

        wxString m_sUrl;
        std::string m_sUid;
        RestfulClient m_client;
		DECLARE_EVENT_TABLE()
};

#endif
