#include "multipartupload.h"
#include <thread>
#include <wx/log.h>
#include "mongoose.h"
#include <fstream>
#include <sstream>
#include <wx/tokenzr.h>
#include "restfulclient.h"
#include <iostream>
#include <linux/magic.h>

wxDEFINE_EVENT(wxEVT_R_PROGRESS, wxCommandEvent);

const std::string MultipartUpload::BOUNDARY = "44E4975E-3D60";
const std::string MultipartUpload::CRLF = "\r\n";

const std::string MultipartUpload::UPLOAD[3] = {"POST", "PATCH", "PUT"};

static void upload_handler(mg_connection* pConnection, int nEvent, void* pData)
{
    MultipartUpload* pThread = reinterpret_cast<MultipartUpload*>(pConnection->user_data);
    pThread->HandleEvent(pConnection, nEvent, pData);
}

MultipartUpload::MultipartUpload(wxEvtHandler* pHandler) : m_pHandler(pHandler),
m_bLoop(false)
{
    m_timerTask.SetOwner(this, wxNewId());
    Connect(m_timerTask.GetId(), wxEVT_TIMER, (wxObjectEventFunction)&MultipartUpload::OnTimerTask);
    Connect(wxID_ANY, wxEVT_R_FINISHED, (wxObjectEventFunction)&MultipartUpload::OnFinishedEvent);
}


bool MultipartUpload::Post(const std::string& sIpAddress, const std::string& sEndpoint,  const std::map<std::string,std::string>& mData, const std::map<std::string, muFile> mFiles, int nId)
{
    m_qTasks.push(task(sIpAddress, sEndpoint, UPLOAD_POST, mData, mFiles, nId));

    if(m_qTasks.size() == 1)
    {
        DoNextTask();
    }
    return true;
}

bool MultipartUpload::Put(const std::string& sIpAddress, const std::string& sEndpoint,  const std::map<std::string,std::string>& mData, const std::map<std::string, muFile> mFiles, int nId)
{
    m_qTasks.push(task(sIpAddress, sEndpoint, UPLOAD_PUT, mData, mFiles, nId));

    if(m_qTasks.size() == 1)
    {
        DoNextTask();
    }
    return true;
}


void MultipartUpload::DoNextTask()
{
        wxLogDebug("MultipartUpload::DoNextTask = '%s'", wxString::FromUTF8(m_qTasks.front().sHost.c_str()).c_str());

    m_pManager = new mg_mgr;

    mg_mgr_init(m_pManager, nullptr);
    mg_connection* pConnection = mg_connect(m_pManager, m_qTasks.front().sHost.c_str(), upload_handler);
    if(pConnection != nullptr)
    {
        pConnection->user_data = reinterpret_cast<void*>(this);
        m_bLoop = true;

        std::thread th([this](){
            while(m_bLoop)
            {
                mg_mgr_poll(m_pManager, 100);
            }
            mg_mgr_free(m_pManager);

            if(m_pHandler)
            {
                wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_R_FINISHED);
                wxQueueEvent(this, pEvent);
            }
        });
        th.detach();
    }
    else
    {
        wxLogDebug("MultipartUpload::DoNextTask: Failed");

    }
}

void MultipartUpload::HandleEvent(mg_connection *pConnection, int nEvent, void* pData)
{
    switch(nEvent)
    {
        case MG_EV_CONNECT:
            ConnectionEvent(pConnection, *reinterpret_cast<int*>(pData));
            break;
        case MG_EV_RECV:
            {
                DataReceived(wxString::FromUTF8(pConnection->recv_mbuf.buf, pConnection->recv_mbuf.len));
                mbuf_remove(&pConnection->recv_mbuf, pConnection->recv_mbuf.len);
            }
            break;
        case MG_EV_SEND:
            {
                m_dSent += *reinterpret_cast<int*>(pData);
                if(m_dLength != 0)
                {
                    int nProg = m_dSent/m_dLength*100.0;
                    if(nProg != m_nProgress)
                    {
                        m_nProgress = nProg;
                        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_R_PROGRESS);
                        pEvent->SetInt(m_nProgress);
                        pEvent->SetExtraLong(m_dRead-m_dSent);
                        wxQueueEvent(m_pHandler, pEvent);
                    }
                }
                SendSomeData(pConnection);
            }
            break;
        case MG_EV_CLOSE:
            break;
    }
}

void MultipartUpload::ConnectionEvent(mg_connection* pConnection, int nStatus)
{
    wxLogDebug("MultipartUpload::ConnectionEvent %d", nStatus);
    if(nStatus != 0)
    {
        wxLogDebug("Failed to connect %d",nStatus);
        //@todo(martim01) log this somehow
        m_bLoop = false;
    }
    else
    {
        std::string sDataBody = GetData();
        m_mFileBody = GetFileBody();

        size_t nSize = sDataBody.size();
        for(auto pairFile : m_mFileBody)
        {
            nSize += pairFile.second.size();
        }
        size_t nFileSize =GetTotalFileLength();

        if(nFileSize > 0)
        {
            nSize += nFileSize;
            nSize += std::string("--"+BOUNDARY+"--"+CRLF+CRLF).size();

            m_dLength = nSize;
            m_dRead = 0;
            m_dSent = 0;

            //send the header...

            std::stringstream ssHeader;
            ssHeader << UPLOAD[m_qTasks.front().nMethod] << " " << m_qTasks.front().sEndpoint << " HTTP/1.1" << CRLF;
            ssHeader << "HOST: " << m_qTasks.front().sHost << CRLF;
            ssHeader << "User-Agent: Mongoose" << MG_VERSION << CRLF;
            ssHeader << "Content-Length: " << nSize << CRLF;
            ssHeader << "Connection: keep-alive" << CRLF;
            ssHeader << "Content-Type: multipart/form-data; boundary=" << BOUNDARY << CRLF;
            ssHeader << "Accept: */* " << CRLF;
            ssHeader << "Origin: null " << CRLF;
            ssHeader << CRLF;
            ssHeader << sDataBody;

            mg_printf(pConnection, ssHeader.str().c_str());

            wxLogDebug(wxString::FromUTF8(ssHeader.str().c_str()));

            m_itFile = m_mFileBody.begin();
            m_bMore = true;
        }
        else
        {
            wxLogDebug("Could not open files");
            m_bLoop = false;
        }
    }
}

void MultipartUpload::InitSendFile(mg_connection* pConnection)
{
    if(m_itFile != m_mFileBody.end())
    {
        auto itFileData = m_qTasks.front().mFiles.find(m_itFile->first);

        if(m_ifs.is_open())
        {
            m_ifs.close();
        }
        //Open and find the length of the file
        wxLogDebug("InitSendFile %s", wxString::FromUTF8((itFileData->second.second.Get()+"/"+itFileData->second.first.Get()).c_str()));
        m_ifs.open(itFileData->second.second.Get()+"/"+itFileData->second.first.Get(), std::ifstream::binary | std::ifstream::ate);
        m_dLength = static_cast<double>(static_cast<size_t>(m_ifs.tellg()));
        m_ifs.seekg(0);

        m_dRead = 0;
        m_nProgress = 0;

        //Send the part "header"
        mg_printf(pConnection, m_itFile->second.c_str());
    }
    else if(m_bMore)
    {
        //All done send the end bit
        std::string sEnd("--"+BOUNDARY+"--"+CRLF+CRLF);
        mg_printf(pConnection, sEnd.c_str());
        m_bMore = false;
    }
}

void MultipartUpload::SendSomeData(mg_connection* pConnection)
{
    if(m_ifs.is_open() == false || m_ifs.eof())
    {
        InitSendFile(pConnection);
    }
    else
    {
        char buffer[1460];
        m_ifs.read(buffer, 1460);
        m_dRead += m_ifs.gcount();


        mg_send(pConnection, buffer, m_ifs.gcount());

        if(m_ifs.eof()) //finished sending
        {
            ++m_itFile;
        }
    }
}

void MultipartUpload::OnTimerTask(const wxTimerEvent& event)
{
    if(m_qTasks.empty() == false)
    {
        DoNextTask();
    }
}

std::string MultipartUpload::GetData()
{
    std::stringstream ssBody;
    for(auto pairData : m_qTasks.front().mData)
    {
        ssBody << "--" << BOUNDARY << CRLF
               << "Content-Disposition: form-data; name=\"" << pairData.first << "\"" << CRLF
               << CRLF
               << pairData.second << CRLF;
    };
    return ssBody.str();
}

std::map<std::string, std::string> MultipartUpload::GetFileBody()
{
    std::map<std::string, std::string> mBody;

    for(auto pairFile : m_qTasks.front().mFiles)
    {
        auto itBody = mBody.insert(std::make_pair(pairFile.first, "")).first;
        itBody->second.append("--"+BOUNDARY+CRLF);
        itBody->second.append("Content-Disposition: form-data; name=\""+pairFile.first+"\"; filename=\""+pairFile.second.first.Get()+"\""+CRLF);
        itBody->second.append("Content-Type: application/octet-stream"+CRLF);
        itBody->second.append("Content-Transfer-Encoding: binary"+CRLF);
        itBody->second.append(CRLF);

    }
    return mBody;
}

size_t MultipartUpload::GetTotalFileLength()
{
    wxLogDebug("MultipartUpload::GetTotalFileLength");
    size_t nSize = 0;
    for(auto pairFile : m_qTasks.front().mFiles)
    {
        std::string sPath(pairFile.second.second.Get()+"/"+pairFile.second.first.Get());

        wxLogDebug("MultipartUpload::GetTotalFileLength: '%s'", wxString::FromUTF8(sPath.c_str()).c_str());
        std::ifstream ifs(sPath, std::ifstream::ate | std::ifstream::binary);
        if(ifs)
        {
           nSize += ifs.tellg();
        }
    }
    return nSize;
}


void MultipartUpload::DataReceived(const wxString& sData)
{

    if(sData.Left(4) == "HTTP")
    {
        unsigned long nCode(0);
        wxString sCode = sData.BeforeFirst('\r').AfterFirst(' ');
        sCode.ToULong(&nCode);

        size_t nPos = sData.Find(CRLF+CRLF);    //end of header
        wxString sMessage = sData.Mid(nPos+4);

        std::cout << sData.ToStdString() << std::endl;
        wxLogDebug("Code= %u", nCode);
        wxLogDebug("Message = %s",sMessage.c_str());
        m_bLoop = false;

        wxCommandEvent* pEvent = new wxCommandEvent(wxEVT_R_REPLY);
        pEvent->SetInt(m_qTasks.front().nId);
        pEvent->SetExtraLong(nCode);
        pEvent->SetString(sMessage);

        wxQueueEvent(m_pHandler, pEvent);

    }
}


void MultipartUpload::OnFinishedEvent(const wxCommandEvent& event)
{
    m_qTasks.pop();
    m_timerTask.Start(250, true);
}


void MultipartUpload::Cancel()
{
    m_bLoop = false;
}
