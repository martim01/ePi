#include "usbchecker.h"
#include <thread>
#include <wx/dir.h>
#include <wx/log.h>


wxDEFINE_EVENT(wxEVT_USB_FOUND, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_USB_FILE_FOUND, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_USB_FINISHED, wxCommandEvent);



void UsbChecker::RunCheck(const wxString& sFilename)
{
    std::thread th([this, sFilename](){
        wxArrayString asFiles;
        //look through /dev/disk/by-id for drives starting usb-
        wxDir::GetAllFiles("/dev/disk/by-id", &asFiles, "usb*");
        //if have same name only try to mount -part1

        for(size_t i = 0; i < asFiles.GetCount(); i++)
        {
            wxString sDebug(asFiles[i]);
            wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_USB_FOUND);
            pEvent->SetString(asFiles[i]);
            wxQueueEvent(m_pHandler, pEvent);
            MountAndSearch(sDebug, sFilename);
        }
        wxCommandEvent* pEventFinished = new wxCommandEvent(wxEVT_USB_FINISHED);
        wxQueueEvent(m_pHandler, pEventFinished);
    });

    th.detach();
}


void UsbChecker::MountAndSearch(const wxString& sDevice, const wxString& sFilename)
{
    wxString sPath;

    //mount the drive
    if(wxDirExists("/mnt/share") == false)
    {
        wxMkdir("/mnt/share");
    }
    wxExecute("sudo umount /mnt/share", wxEXEC_SYNC);
    wxString sCommand(wxString::Format("sudo mount -o umask=000 /dev/%s /mnt/share", sDevice.c_str()));

    wxExecute(sCommand, wxEXEC_SYNC);

    sPath = "/mnt/share";

    wxArrayString asFiles;
    wxDir::GetAllFiles("/mnt/share", &asFiles, sFilename);

    for(size_t i = 0; i < asFiles.size(); i++)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_USB_FILE_FOUND);
        pEvent->SetString(wxString::Format("{ \"Device\": \"%s\", \"File\": \"%s\"}", sDevice.c_str(), asFiles[i].c_str()));
        wxQueueEvent(m_pHandler, pEvent);
    }

}
