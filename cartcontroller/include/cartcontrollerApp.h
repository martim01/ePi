/***************************************************************
 * Name:      cartcontrollerApp.h
 * Purpose:   Defines Application Class
 * Author:    Matthew Martin (martim01@outlook.com)
 * Created:   2020-06-15
 * Copyright: Matthew Martin (https://github.com/martim01)
 * License:
 **************************************************************/

#ifndef CARTCONTROLLERAPP_H
#define CARTCONTROLLERAPP_H

#include <wx/app.h>

class cartcontrollerApp : public wxApp
{
    public:
        virtual bool OnInit();

        /**	Initialise command line parser */
		void OnInitCmdLine(wxCmdLineParser& parser) override;

		/**	Handle command line parser result */
		bool OnCmdLineParsed(wxCmdLineParser& parser) override;

    private:
        wxString m_sIpAddress;
        unsigned long m_nPort;
};

#endif // CONTROLLERAPP_H
