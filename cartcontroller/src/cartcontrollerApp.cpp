/***************************************************************
 * Name:      cartcontrollerApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Matthew Martin (martim01@outlook.com)
 * Created:   2020-09-25
 * Copyright: Matthew Martin (https://github.com/martim01)
 * License:
 **************************************************************/

#include "cartcontrollerApp.h"

//(*AppHeaders
#include "cartcontrollerMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(cartcontrollerApp);

bool cartcontrollerApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	cartcontrollerDialog Dlg(0);
    	SetTopWindow(&Dlg);
    	Dlg.ShowModal();
    	wxsOK = false;
    }
    //*)
    return wxsOK;

}
