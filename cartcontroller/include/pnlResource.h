#ifndef PNLRESOURCE_H
#define PNLRESOURCE_H
#include "uirect.h"
#include <string>
#include <wx/timer.h>
#include "json/json.h"


//(*Headers(pnlResource)
#include <wx/panel.h>
//*)

class pnlResource: public wxPanel
{
	public:

		pnlResource(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~pnlResource();

		//(*Declarations(pnlResource)
		//*)

		void SetResource(const std::string& sUid, const std::string& sLabel);

		const wxString& GetUid() const
		{
            return m_sUid;
		}

		void UpdatePlayingStatus(const Json::Value& jsStatus);

	protected:

		//(*Identifiers(pnlResource)
		//*)

	private:

		//(*Handlers(pnlResource)
		void OnPaint(wxPaintEvent& event);
		void OnLeftDown(wxMouseEvent& event);
		void OnLeftUp(wxMouseEvent& event);
		//*)

		void OnSize(wxSizeEvent& event);
		void OntimerMenuTrigger(wxTimerEvent& event);

		void ColourRects();
		void ColourDown();

		wxTimer m_timerMenu;

		uiRect m_uiName;
        uiRect m_uiStatus;

        wxString m_sUid;

		DECLARE_EVENT_TABLE()

		enum {NO_FILE, STOPPED, PLAYING, ORPHANED};
        int m_nPlaying;
        bool m_bDown;
        bool m_bIgnoreUp;

        int m_nHoldCount;

		static const wxColour CLR_PLAYING;
        static const wxColour CLR_IDLE;
        static const wxColour CLR_CONNECTING;
        static const wxColour CLR_ERROR;
        static const wxColour CLR_NO_FILE;
        static const wxColour CLR_ORPHANED;
        static const wxColour CLR_DOWN;
};

wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_RESOURCE_PLAY, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_RESOURCE_MENU, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_RESOURCE_STOP, wxCommandEvent);

#endif
