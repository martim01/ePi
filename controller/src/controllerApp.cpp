/***************************************************************
 * Name:      controllerApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Matthew Martin (martim01@outlook.com)
 * Created:   2020-06-15
 * Copyright: Matthew Martin (https://github.com/martim01)
 * License:
 **************************************************************/

#include "controllerApp.h"
#include <wx/cmdline.h>

//(*AppHeaders
#include "controllerMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(controllerApp);

void controllerApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.SetLogo(wxT("ePi Controller"));
	parser.AddParam(wxT("Columns"));
	parser.AddParam(wxT("Rows"));
	parser.AddParam(wxT("Controller"));
	parser.AddParam(wxT("IpAddress"));
	parser.AddParam(wxT("Port"));

}

bool controllerApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    for(size_t i = 0; i < parser.GetParamCount(); i++)
    {
        wxLogDebug(parser.GetParam(i));
    }

    if(parser.GetParam(0).ToULong(&m_nColumns) == false)
    {
        return false;
    }
    if(parser.GetParam(1).ToULong(&m_nRows) == false)
    {
        return false;
    }
    if(parser.GetParam(2).ToULong(&m_nController)== false)
    {
        return false;
    }
    m_sIpAddress = parser.GetParam(3);
    if(parser.GetParam(4).ToULong(&m_nPort) == false)
    {
        return false;
    }


	return true;
}


bool controllerApp::OnInit()
{
    if(wxApp::OnInit() == false)
    {
        return false;
    }

    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        wxLogDebug("(%u,%u) %d %s:%u", m_nColumns, m_nRows, m_nController, m_sIpAddress.c_str(), m_nPort);
    	controllerDialog Dlg(0, wxPoint(m_nColumns,m_nRows), m_nController, m_sIpAddress, m_nPort);
    	SetTopWindow(&Dlg);
    	Dlg.ShowModal();
    	wxsOK = false;
    }

    return wxsOK;

}
