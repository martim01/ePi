#ifndef DLGINFO_H
#define DLGINFO_H

//(*Headers(dlgInfo)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)
#include "wmlabel.h"
#include "wmbutton.h"
class WebSocketClient;

class dlgInfo: public wxDialog
{
	public:

		dlgInfo(wxWindow* parent,WebSocketClient& wsClient, const wxString& sHostname, wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgInfo();

		//(*Declarations(dlgInfo)
		wmButton* m_pbtnClose;
		wmLabel* StaticText10;
		wmLabel* StaticText11;
		wmLabel* StaticText12;
		wmLabel* StaticText13;
		wmLabel* StaticText14;
		wmLabel* StaticText15;
		wmLabel* StaticText16;
		wmLabel* StaticText17;
		wmLabel* StaticText18;
		wmLabel* StaticText19;
		wmLabel* StaticText1;
		wmLabel* StaticText20;
		wmLabel* StaticText24;
		wmLabel* StaticText25;
		wmLabel* StaticText26;
		wmLabel* StaticText27;
		wmLabel* StaticText28;
		wmLabel* StaticText29;
		wmLabel* StaticText32;
		wmLabel* StaticText33;
		wmLabel* StaticText34;
		wmLabel* StaticText35;
		wmLabel* StaticText3;
		wmLabel* StaticText5;
		wmLabel* StaticText6;
		wmLabel* StaticText7;
		wmLabel* StaticText8;
		wmLabel* StaticText9;
		wmLabel* m_plblApplicationTime;
		wmLabel* m_plblApplicationUptime;
		wmLabel* m_plblApplicationVersion;
		wmLabel* m_plblApplicationlStartTime;
		wmLabel* m_plblBytesAvailable;
		wmLabel* m_plblBytesFree;
		wmLabel* m_plblBytesTotal;
		wmLabel* m_plblCPU0;
		wmLabel* m_plblCPU1;
		wmLabel* m_plblCPU2;
		wmLabel* m_plblCPU3;
		wmLabel* m_plblCPUOverall;
		wmLabel* m_plblHighFree;
		wmLabel* m_plblHighTotal;
		wmLabel* m_plblInodesAvailable;
		wmLabel* m_plblInodesFree;
		wmLabel* m_plblInodesTotal;
		wmLabel* m_plblLoads15;
		wmLabel* m_plblLoads1;
		wmLabel* m_plblLoads5;
		wmLabel* m_plblRamBuffered;
		wmLabel* m_plblRamFree;
		wmLabel* m_plblRamShared;
		wmLabel* m_plblRamTotal;
		wmLabel* m_plblSwapFree;
		wmLabel* m_plblSwapTotal;
		wmLabel* m_plblSystemProcs;
		wmLabel* m_plblSystemUpTime;
		wmLabel* m_pstHostname;
		//*)

	protected:

		//(*Identifiers(dlgInfo)
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT36;
		static const long ID_STATICTEXT6;
		static const long ID_STATICTEXT37;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXT38;
		static const long ID_STATICTEXT8;
		static const long ID_STATICTEXT39;
		static const long ID_STATICTEXT9;
		static const long ID_STATICTEXT40;
		static const long ID_STATICTEXT10;
		static const long ID_STATICTEXT41;
		static const long ID_STATICTEXT11;
		static const long ID_STATICTEXT42;
		static const long ID_STATICTEXT12;
		static const long ID_STATICTEXT43;
		static const long ID_STATICTEXT13;
		static const long ID_STATICTEXT44;
		static const long ID_STATICTEXT14;
		static const long ID_STATICTEXT45;
		static const long ID_STATICTEXT15;
		static const long ID_STATICTEXT46;
		static const long ID_STATICTEXT16;
		static const long ID_STATICTEXT47;
		static const long ID_STATICTEXT17;
		static const long ID_STATICTEXT48;
		static const long ID_STATICTEXT18;
		static const long ID_STATICTEXT21;
		static const long ID_STATICTEXT19;
		static const long ID_STATICTEXT22;
		static const long ID_STATICTEXT20;
		static const long ID_STATICTEXT23;
		static const long ID_STATICTEXT24;
		static const long ID_STATICTEXT49;
		static const long ID_STATICTEXT25;
		static const long ID_STATICTEXT50;
		static const long ID_STATICTEXT26;
		static const long ID_STATICTEXT51;
		static const long ID_STATICTEXT27;
		static const long ID_STATICTEXT52;
		static const long ID_STATICTEXT28;
		static const long ID_STATICTEXT53;
		static const long ID_STATICTEXT29;
		static const long ID_STATICTEXT54;
		static const long ID_STATICTEXT32;
		static const long ID_STATICTEXT30;
		static const long ID_STATICTEXT33;
		static const long ID_STATICTEXT31;
		static const long ID_STATICTEXT34;
		static const long ID_STATICTEXT55;
		static const long ID_STATICTEXT35;
		static const long ID_STATICTEXT56;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(dlgInfo)
		void OnbtnCloseClick(wxCommandEvent& event);
		//*)

		WebSocketClient& m_wsClient;

		void OnWebsocketConnection(const wxCommandEvent& event);
        void OnWebsocketHandshake(const wxCommandEvent& event);
        void OnWebsocketFrame(const wxCommandEvent& event);
        void OnWebsocketFinished(const wxCommandEvent& event);

        void ClearLabels();

        static const unsigned long MBYTES = 1048576;
        static const unsigned long GBYTES = 1073741824;

		DECLARE_EVENT_TABLE()
};

#endif
