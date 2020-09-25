#ifndef PNLRESOURCE_H
#define PNLRESOURCE_H
#include "uirect.h"

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

	protected:

		//(*Identifiers(pnlResource)
		//*)

	private:

		//(*Handlers(pnlResource)
		void OnPaint(wxPaintEvent& event);
		void OnLeftDown(wxMouseEvent& event);
		void OnLeftUp(wxMouseEvent& event);
		//*)

		uiRect m_uiName;
        uiRect m_uiStatus;

		DECLARE_EVENT_TABLE()

		static const wxColour CLR_PLAYING;
        static const wxColour CLR_IDLE;
        static const wxColour CLR_CONNECTING;
        static const wxColour CLR_ERROR;
        static const wxColour CLR_NO_FILE;
};

#endif
