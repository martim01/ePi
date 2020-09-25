/***************************************************************
 * Name:      cartcontrollerApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Matthew Martin (martim01@outlook.com)
 * Created:   2020-06-15
 * Copyright: Matthew Martin (https://github.com/martim01)
 * License:
 **************************************************************/

#include "cartcontrollerApp.h"
#include <wx/cmdline.h>

//(*AppHeaders
#include "cartcontrollerMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(cartcontrollerApp);

void cartcontrollerApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.SetLogo(wxT("ePi Cart Controller"));
	parser.AddParam(wxT("IpAddress"));
	parser.AddParam(wxT("Port"));

}

bool cartcontrollerApp::OnCmdLineParsed(wxCmdLineParser& parser)
{

    m_sIpAddress = parser.GetParam(0);
    if(parser.GetParam(1).ToULong(&m_nPort) == false)
    {
        return false;
    }


	return true;
}


bool cartcontrollerApp::OnInit()
{
    if(wxApp::OnInit() == false)
    {
        return false;
    }

    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        cartcontrollerDialog Dlg(0, m_sIpAddress, m_nPort);
    	SetTopWindow(&Dlg);
    	Dlg.ShowModal();
    	wxsOK = false;
    }

    return wxsOK;

}
