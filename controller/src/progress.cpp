#include "progress.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(Progress, wxWindow)
    EVT_PAINT(Progress::OnPaint)
    EVT_SIZE(Progress::OnSize)
END_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(Progress, wxWindow);



Progress::Progress() : wxWindow(),
m_nMax(100),
m_nValue(0),
m_dResolution(1.0),
m_nBorder(uiRect::BORDER_FLAT)
{
}

Progress::Progress(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& szSize, long nStyle) : wxWindow(parent, id, pos, szSize, nStyle),
m_nMax(100),
m_nValue(0),
m_dResolution(szSize.x/100.0),
m_nBorder(uiRect::BORDER_FLAT)
{
    wxSize szInit(szSize);
    wxSize bestSize = DoGetBestSize();
    if(szSize.x<=0)
        szInit.SetWidth(bestSize.x);
    if(szSize.y <= 0)
        szInit.SetHeight(bestSize.y);

    SetMinSize(szSize);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    CalculateResolution();
    CreateRect();
}

bool Progress::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& szSize, long nStyle)
{
    wxSize szInit(szSize);
    wxSize bestSize = DoGetBestSize();
    if(szSize.x<=0)
        szInit.SetWidth(bestSize.x);
    if(szSize.y <= 0)
        szInit.SetHeight(bestSize.y);

    if(!wxWindow::Create(parent,id,pos,szInit))
        return false;

    SetMinSize(szSize);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    CalculateResolution();
    CreateRect();

    return true;
}

void Progress::CreateRect()
{
    m_uiRect.SetRect(0,0, static_cast<double>(m_nValue)*m_dResolution, GetClientSize().y);
}

Progress::~Progress()
{
}


bool Progress::SetBackgroundColour(const wxColour &colour)
{
    wxWindow::SetBackgroundColour(colour);
    Refresh();
    return true;
}
bool Progress::SetForegroundColour(const wxColour &colour)
{
    m_uiRect.SetBackgroundColour(colour);
    Refresh();
    return true;
}

void Progress::SetBorderState(unsigned short nState)
{
    m_nBorder = nState;
    Refresh();
}


void Progress::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();

    m_uiRect.Draw(dc, m_nBorder);

}

void Progress::OnSize(wxSizeEvent& event)
{
    CalculateResolution();
    CreateRect();
    Refresh();
}

void Progress::SetRange(unsigned int nMax)
{
    m_nMax = nMax;
    CalculateResolution();
    CreateRect();
    Refresh();
}

void Progress::SetValue(unsigned int nValue)
{
    m_nValue = nValue;
    CreateRect();
    Refresh();
}


void Progress::CalculateResolution()
{
    m_dResolution = static_cast<double>(GetClientSize().x)/static_cast<double>(m_nMax);
}
