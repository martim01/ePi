#include "usbchecker.h"
#include <thread>
#include <wx/dir.h>
#include <wx/log.h>
#include <sys/mount.h>
#include <wx/msgdlg.h>
#include "json/json.h"
#include "epiwriter.h"
#include <fstream>
#include <iostream>

wxDEFINE_EVENT(wxEVT_USB_FOUND, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_USB_FILE_FOUND, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_USB_FINISHED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_USB_ERROR, wxCommandEvent);



void UsbChecker::RunCheck(const wxString& sFilename)
{
    std::thread th([this, sFilename](){
        wxArrayString asFiles;
        //look through /dev/disk/by-id for drives starting usb-
        wxDir::GetAllFiles("/dev/disk/by-id", &asFiles, "usb*");
        //if have same name only try to mount -part1

        for(size_t i = 0; i < asFiles.GetCount(); i++)
        {
            if(asFiles[i].Find("part") != wxNOT_FOUND)
            {
                wxString sDebug(asFiles[i]);
                wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_USB_FOUND);
                pEvent->SetString(asFiles[i]);
                wxQueueEvent(m_pHandler, pEvent);
                MountAndSearch(sDebug, sFilename);
            }
        }
        wxCommandEvent* pEventFinished = new wxCommandEvent(wxEVT_USB_FINISHED);
        wxQueueEvent(m_pHandler, pEventFinished);
    });

    th.detach();
}

int UsbChecker::UnmountDevice()
{
    int nResult = umount("/mnt/share");
    if(nResult == -1 && errno != EAGAIN && errno != EINVAL)
    {
        return errno;
    }
    return 0;
}


int UsbChecker::MountDevice(const wxString& sDevice)
{
    if(sDevice.empty())
    {
        return EINVAL;
    }

    if(wxDirExists("/mnt/share") == false)
    {
        wxMkdir("/mnt/share");
    }
    int nResult = umount("/mnt/share");
    if(nResult == -1 && errno != EAGAIN && errno != EINVAL)
    {
        return errno;
    }


    std::array<std::string, 8> fs({"ext3", "ext2", "ext4", "vfat", "msdos", "f2fs", "fuseblk", "ntfs"});
    for(size_t i = 0; i < fs.size(); i++)
    {
        nResult = mount(sDevice.ToStdString().c_str(), "/mnt/share", fs[i].c_str(), MS_RDONLY | MS_SILENT, nullptr);
        if(nResult == 0)
        {
            wxLogDebug("Mounted as %s",wxString::FromUTF8(fs[i].c_str()).c_str());
            return 0;
        }
    }
    return errno;

}


void UsbChecker::MountAndSearch(const wxString& sDevice, const wxString& sFilename)
{
    int nError = MountDevice(sDevice);
    if(nError != 0)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_USB_ERROR);
        pEvent->SetInt(nError);
        pEvent->SetString(wxString::Format("Mount: %s", wxString::FromUTF8(strerror(errno)).c_str()));

        wxQueueEvent(m_pHandler, pEvent);
        return;
    }

    wxString sPath = "/mnt/share";

    wxArrayString asFiles;
    wxDir::GetAllFiles("/mnt/share", &asFiles, sFilename);

    for(size_t i = 0; i < asFiles.size(); i++)
    {
        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_USB_FILE_FOUND);
        Json::Value jsValue;
        jsValue["device"] = sDevice.ToStdString();
        jsValue["file"] = asFiles[i].ToStdString();
        std::stringstream ss;
        epiWriter::Get().writeToSStream(jsValue, ss);
        pEvent->SetString(wxString::FromUTF8(ss.str().c_str()));
        wxQueueEvent(m_pHandler, pEvent);
    }
    umount("/mnt/share");
}
