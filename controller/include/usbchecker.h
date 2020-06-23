#pragma once
#include <wx/event.h>

class UsbChecker
{
    public:
        UsbChecker(wxEvtHandler* pHandler) :m_pHandler(pHandler){}
        void RunCheck(const wxString& sFilename);

    private:
        void MountAndSearch(const wxString& sDevice, const wxString& sFilename);

        wxEvtHandler* m_pHandler;

};

wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_USB_FOUND, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_USB_FILE_FOUND, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_USB_FINISHED, wxCommandEvent);
