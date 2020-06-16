/***************************************************************
 * Name:      controllerApp.h
 * Purpose:   Defines Application Class
 * Author:    Matthew Martin (martim01@outlook.com)
 * Created:   2020-06-15
 * Copyright: Matthew Martin (https://github.com/martim01)
 * License:
 **************************************************************/

#ifndef CONTROLLERAPP_H
#define CONTROLLERAPP_H

#include <wx/app.h>

class controllerApp : public wxApp
{
    public:
        virtual bool OnInit();

        /**	Initialise command line parser */
		void OnInitCmdLine(wxCmdLineParser& parser) override;

		/**	Handle command line parser result */
		bool OnCmdLineParsed(wxCmdLineParser& parser) override;

    private:
        unsigned long m_nColumns;
        unsigned long m_nRows;
        unsigned long m_nController;
        wxString m_sIpAddress;
        unsigned long m_nPort;
};

#endif // CONTROLLERAPP_H
