#ifndef DLGINFO_H
#define DLGINFO_H

//(*Headers(dlgInfo)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

class dlgInfo: public wxDialog
{
	public:

		dlgInfo(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~dlgInfo();

		//(*Declarations(dlgInfo)
		wxButton* m_pbtnClose;
		wxStaticText* StaticText10;
		wxStaticText* StaticText11;
		wxStaticText* StaticText12;
		wxStaticText* StaticText13;
		wxStaticText* StaticText14;
		wxStaticText* StaticText15;
		wxStaticText* StaticText16;
		wxStaticText* StaticText17;
		wxStaticText* StaticText18;
		wxStaticText* StaticText19;
		wxStaticText* StaticText1;
		wxStaticText* StaticText20;
		wxStaticText* StaticText24;
		wxStaticText* StaticText25;
		wxStaticText* StaticText26;
		wxStaticText* StaticText27;
		wxStaticText* StaticText28;
		wxStaticText* StaticText29;
		wxStaticText* StaticText32;
		wxStaticText* StaticText33;
		wxStaticText* StaticText34;
		wxStaticText* StaticText35;
		wxStaticText* StaticText3;
		wxStaticText* StaticText5;
		wxStaticText* StaticText6;
		wxStaticText* StaticText7;
		wxStaticText* StaticText8;
		wxStaticText* StaticText9;
		wxStaticText* m_plblApplicationTime;
		wxStaticText* m_plblApplicationUptime;
		wxStaticText* m_plblApplicationVersion;
		wxStaticText* m_plblApplicationlStartTime;
		wxStaticText* m_plblBytesAvailable;
		wxStaticText* m_plblBytesFree;
		wxStaticText* m_plblBytesTotal;
		wxStaticText* m_plblCPU0;
		wxStaticText* m_plblCPU1;
		wxStaticText* m_plblCPU2;
		wxStaticText* m_plblCPU3;
		wxStaticText* m_plblCPUOverall;
		wxStaticText* m_plblHighFree;
		wxStaticText* m_plblHighTotal;
		wxStaticText* m_plblInodesAvailable;
		wxStaticText* m_plblInodesFree;
		wxStaticText* m_plblInodesTotal;
		wxStaticText* m_plblLoads15;
		wxStaticText* m_plblLoads1;
		wxStaticText* m_plblLoads5;
		wxStaticText* m_plblRamBuffered;
		wxStaticText* m_plblRamFree;
		wxStaticText* m_plblRamShared;
		wxStaticText* m_plblRamTotal;
		wxStaticText* m_plblSwapFree;
		wxStaticText* m_plblSwapTotal;
		wxStaticText* m_plblSystemProcs;
		wxStaticText* m_plblSystemUpTime;
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

		DECLARE_EVENT_TABLE()
};

#endif
