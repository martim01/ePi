#pragma once
#include <wx/event.h>
#include <thread>
#include <memory>

class UsbChecker
{
    public:
        UsbChecker(wxEvtHandler* pHandler) :m_pHandler(pHandler), m_pThread(nullptr){}
        ~UsbChecker();
        void RunCheck(const wxString& sFilename);

        static int MountDevice(const wxString& sDevice);
        static int UnmountDevice();

    private:
        void Abort();
        void MountAndSearch(const wxString& sDevice, const wxString& sFilename);
        std::unique_ptr<std::thread> m_pThread;
        wxEvtHandler* m_pHandler;

};

wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_USB_FOUND, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_USB_FILE_FOUND, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_USB_FINISHED, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXEXPORT, wxEVT_USB_ERROR, wxCommandEvent);
