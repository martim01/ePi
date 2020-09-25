#include "pnlResource.h"
#include <wx/dcbuffer.h>

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

wxDEFINE_EVENT(wxEVT_RESOURCE_PLAY, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_RESOURCE_MENU, wxCommandEvent);

pnlResource::pnlResource(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size) :
m_nPlaying(STOPPED),
m_bDown(false),
m_bIgnoreUp(false)
{
	Create(parent, id, pos, size, wxTAB_TRAVERSAL, _T("id"));

	SetBackgroundColour(*wxWHITE);

	m_uiName.SetRect(1,1,GetClientRect().GetWidth()-2,GetClientRect().GetHeight()/2);
    m_uiStatus.SetRect(1,m_uiName.GetBottom(),m_uiName.GetWidth(),m_uiName.GetHeight());

    m_uiName.SetGradient(0);
    m_uiStatus.SetGradient(0);
    m_uiName.SetForegroundColour(*wxBLACK);
    m_uiStatus.SetForegroundColour(*wxBLACK);
    m_uiName.SetBackgroundColour(CLR_NO_FILE);
    m_uiStatus.SetBackgroundColour(CLR_NO_FILE);

    Connect(wxEVT_PAINT, (wxObjectEventFunction)&pnlResource::OnPaint);
    Connect(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&pnlResource::OnLeftDown);
    Connect(wxEVT_LEFT_UP, (wxObjectEventFunction)&pnlResource::OnLeftUp);
    Connect(wxEVT_SIZE, (wxObjectEventFunction)&pnlResource::OnSize);

    m_timerMenu.SetOwner(this, wxNewId());
    Connect(m_timerMenu.GetId(),wxEVT_TIMER,(wxObjectEventFunction)&pnlResource::OntimerMenuTrigger);

}

pnlResource::~pnlResource()
{
	//(*Destroy(pnlResource)
	//*)
}

void pnlResource::SetResource(const std::string& sUid, const std::string& sLabel)
{
    m_sUid = wxString::FromUTF8(sUid.c_str());
    m_uiName.SetLabel(wxString::FromUTF8(sLabel.c_str()));

    if(m_sUid.empty() == false)
    {
        m_uiName.SetBackgroundColour(CLR_IDLE);
        m_uiStatus.SetBackgroundColour(CLR_IDLE);
    }
    else
    {
        m_uiName.SetBackgroundColour(CLR_NO_FILE);
        m_uiStatus.SetBackgroundColour(CLR_NO_FILE);
    }
    Refresh();
}

void pnlResource::OnSize(wxSizeEvent& event)
{
    m_uiName.SetRect(1,1,GetClientRect().GetWidth()-2,GetClientRect().GetHeight()/2);
    m_uiStatus.SetRect(1,m_uiName.GetBottom(),m_uiName.GetWidth(),m_uiName.GetHeight());


    Refresh();
}

void pnlResource::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    wxFont fnt(GetFont());
    fnt.SetPointSize(8);
    dc.SetFont(fnt);
    m_uiName.Draw(dc, uiRect::BORDER_NONE);
    dc.SetFont(fnt);
    m_uiStatus.Draw(dc, uiRect::BORDER_NONE);
}

void pnlResource::OnLeftDown(wxMouseEvent& event)
{
    m_bDown = true;
    m_clrUp = m_uiName.GetBackgroundColour();

    m_uiName.SetBackgroundColour(wxColour(255,128,0));
    m_uiStatus.SetBackgroundColour(wxColour(255,128,0));
    Refresh();
    m_timerMenu.Start(4000,true);

}

void pnlResource::OnLeftUp(wxMouseEvent& event)
{
    ColourUp();

    m_timerMenu.Stop();
    if(m_bIgnoreUp)
    {
        m_bIgnoreUp = false;
    }
    else if(m_bDown)
    {
        wxCommandEvent resourceEvent(wxEVT_RESOURCE_PLAY);
        resourceEvent.SetString(m_sUid);
        wxPostEvent(GetParent(), resourceEvent);

    }
    m_bDown = false;
}

void pnlResource::ColourUp()
{
    m_uiName.SetBackgroundColour(m_clrUp);
    m_uiStatus.SetBackgroundColour(m_clrUp);
    Refresh();
}

void pnlResource::OntimerMenuTrigger(wxTimerEvent& event)
{
    ColourUp();
    m_bIgnoreUp = true;
    m_bDown = false;

    wxCommandEvent resourceEvent(wxEVT_RESOURCE_MENU);
    resourceEvent.SetString(m_sUid);
    wxPostEvent(GetParent(), resourceEvent);

    /*

    */
}


void pnlResource::UpdatePlayingStatus(const Json::Value& jsData)
{
    if(jsData["player"].asString() == "Running")
    {
        if(jsData["resource"]["uid"] == m_sUid.ToStdString())
        {
            m_uiName.SetBackgroundColour(CLR_PLAYING);
            m_uiStatus.SetBackgroundColour(CLR_PLAYING);
            if(jsData["status"].isObject() && jsData["status"]["playing"].isObject())
            {
                m_uiStatus.SetLabel(wxTimeSpan(0,0,0, jsData["status"]["playing"]["time"].asInt()).Format("%H:%M:%S"));
            }
        }
        else
        {
            ColourIdle();
        }
    }
    else if(jsData["player"].asString() == "Orphaned")
    {
        m_uiName.
       // m_nPlaying = ORPHANED;
       // m_tsLength = wxTimeSpan(0);
       // m_tsPosition = wxTimeSpan(0);
    }
    else if(jsData["player"].asString() == "Stopped")
    {
        ColourIdle();
    }
    Refresh();
}

void pnlResource::ColourIdle()
{
    wxColour clr(CLR_IDLE);
    if(m_sUid.empty())
    {
        clr = CLR_NO_FILE;
    }
    m_uiStatus.SetLabel(wxEmptyString);
    m_uiName.SetBackgroundColour(clr);
    m_uiStatus.SetBackgroundColour(clr);
}
