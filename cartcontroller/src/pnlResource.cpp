#include "pnlResource.h"

//(*InternalHeaders(pnlResource)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(pnlResource)
//*)

BEGIN_EVENT_TABLE(pnlResource,wxPanel)
	//(*EventTable(pnlResource)
	//*)
END_EVENT_TABLE()

const wxColour pnlResource::CLR_PLAYING = wxColour(146,208,80);
const wxColour pnlResource::CLR_IDLE = wxColour(141,180,226);
const wxColour pnlResource::CLR_ERROR =  wxColour(255,0,0);
const wxColour pnlResource::CLR_NO_FILE = wxColour(160,160,160);
const wxColour pnlResource::CLR_CONNECTING = wxColour(255,255,0);

pnlResource::pnlResource(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(pnlResource)
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	//*)

	SetBackgroundColour(CLR_NO_FILE);

	m_uiName.SetRect(1,1,GetClientRect().GetWidth()-2,GetClientRect().GetHeight()/2);
    m_uiStatus.SetRect(1,m_uiName.GetBottom(),m_uiName.GetWidth(),m_uiName.GetHeight());

    m_uiName.SetGradient(0);
    m_uiStatus.SetGradient(0);
    m_uiName.SetForegroundColour(*wxBLACK);
    m_uiStatus.SetForegroundColour(*wxBLACK);
}

pnlResource::~pnlResource()
{
	//(*Destroy(pnlResource)
	//*)
}


void pnlResource::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    wxFont fnt(GetFont());
    fnt.SetPointSize(20);
    dc.SetFont(fnt);
    m_uiName.Draw(dc, uiRect::BORDER_NONE);
    if(m_nPlaying != STOPPED)
    {
        fnt.SetPointSize(16);
    }
    else
    {
        fnt.SetPointSize(12);
    }
    dc.SetFont(fnt);
    m_uiStatus.Draw(dc, uiRect::BORDER_NONE);
}

void pnlResource::OnLeftDown(wxMouseEvent& event)
{
}

void pnlResource::OnLeftUp(wxMouseEvent& event)
{
}
