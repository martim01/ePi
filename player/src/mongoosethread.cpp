#include "mongoosethread.h"
#include <iostream>
#include <thread>
#include <initializer_list>
#include "json/json.h"
#include <sstream>
#include "inisection.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <map>
#include "log.h"
#include <chrono>
#include "utils.h"

using namespace std;
using namespace pml;

const std::string MongooseThread::EP_ROOT        = "x-epi";          //GET
const std::string MongooseThread::EP_STATUS      = "status";         //GET PUT (Play or Stop)
const std::string MongooseThread::EP_POWER       = "power";          //PUT= set power
const std::string MongooseThread::EP_CONFIG      = "config";         //GET PUT
const std::string MongooseThread::EP_SCHEDULES   = "schedules";      //GET
const std::string MongooseThread::EP_PLAYLISTS   = "playlists";      //GET
const std::string MongooseThread::EP_FILES       = "files";          //GET
const std::string MongooseThread::EP_INFO        = "info";        //GET


static mg_serve_http_opts s_ServerOpts;


string GetNowAsIso()
{
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

map<string, string> DecodeQueryString(http_message* pMessage)
{
    map<string, string> mDecode;

    vector<string> vQuery;
    SplitString(vQuery, pMessage->query_string.p, '&');
    for(size_t i = 0; i < vQuery.size(); i++)
    {
        vector<string> vValue;
        SplitString(vValue, vQuery[i], '=');
        if(vValue.size() == 2)
        {
            mDecode.insert(make_pair(vValue[0], vValue[1]));
        }
        else if(vValue.size() == 1)
        {
            mDecode.insert(make_pair(vValue[0], ""));
        }
    }
    return mDecode;
}


static int is_websocket(const struct mg_connection *nc)
{
    return nc->flags & MG_F_IS_WEBSOCKET;
}



static void ev_handler(mg_connection *pConnection, int nEvent, void* pData)
{
    if(nEvent == 0)
    {
        return;
    }

    MongooseThread* pThread = reinterpret_cast<MongooseThread*>(pConnection->user_data);
    pThread->HandleEvent(pConnection, nEvent, pData);
}

static mg_str file_upload(mg_connection*, mg_str file_name)
{
    Log::Get(Log::LOG_DEBUG) << "upload" << endl;
    // @todo need to get the endpoint to see what the file type is...

    char* filename= reinterpret_cast<char*>(malloc(strlen("/tmp/audio")+1));
    strcpy(filename, "/tmp/audio");
    return mg_mk_str(filename);
}




void MongooseThread::EventWebsocket(mg_connection *pConnection, int nEvent, void* pData)
{

    http_message* pMessage = reinterpret_cast<http_message*>(pData);

    Log::Get(Log::LOG_INFO) << "MG_EV_WEBSOCKET_HANDSHAKE_DONE" << endl;

    string sUri;
    sUri.assign(pMessage->uri.p, pMessage->uri.len);


}

void MongooseThread::EventHttp(mg_connection *pConnection, int nEvent, void* pData)
{

    //lock_guard<mutex> lg(m_mutex);
    http_message* pMessage = reinterpret_cast<http_message*>(pData);
    string sMethod(pMessage->method.p);
    string sUri;
    sUri.assign(pMessage->uri.p, pMessage->uri.len);

    if(CmpNoCase(sMethod, "GET"))
    {
        DoHttpGet(pConnection, sUri);
    }
    else if(CmpNoCase(sMethod, "POST"))
    {
        DoHttpPost(pConnection, sUri, pMessage);
    }
    else if(CmpNoCase(sMethod, "PUT"))
    {
        DoHttpPut(pConnection, sUri, pMessage);
    }
    else if(CmpNoCase(sMethod, "DELETE"))
    {
        DoHttpDelete(pConnection, sUri, pMessage);
    }
    else
    {

    }
}

void MongooseThread::HandleEvent(mg_connection *pConnection, int nEvent, void* pData)
{
    //lock_guard<recursive_mutex> lg(m_mutex);
    if(nEvent != 4 && nEvent != 6)
        Log::Get(Log::LOG_DEBUG) << "HandleEvent: " << nEvent << endl;
    switch (nEvent)
    {
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    {
        Log::Get(Log::LOG_DEBUG) << "Event Websocket" << endl;
        //lock_guard<recursive_mutex> lg(m_mutex);
        EventWebsocket(pConnection, nEvent, pData);
        Log::Get(Log::LOG_DEBUG) << "Done" << endl;
    }
    break;
    case MG_EV_HTTP_REQUEST:
    {
        Log::Get(Log::LOG_DEBUG) << "Event HTTP" << endl;
        //lock_guard<recursive_mutex> lg(m_mutex);
        EventHttp(pConnection, nEvent, pData);
        Log::Get(Log::LOG_DEBUG) << "Done" << endl;
    }
    break;
    case MG_EV_TIMER:
        /* Perform session maintenance. */
    {
    }
    break;
    case MG_EV_CLOSE:
    {
        Log::Get(Log::LOG_DEBUG) << "Event Close" << endl;
        //lock_guard<recursive_mutex> lg(m_mutex);
        if (is_websocket(pConnection))
        {
            pConnection->user_data = 0;
        }
        Log::Get(Log::LOG_DEBUG) << "Done" << endl;
    }
    break;
    case MG_EV_HTTP_MULTIPART_REQUEST:
        if(UploadAllowed(pConnection, reinterpret_cast<http_message*>(pData)))
        {
        }
        else
        {
            // @todo send a not allowed to upload to this endpoint error
        }
        break;
    case MG_EV_HTTP_PART_BEGIN:
        Log::Get(Log::LOG_DEBUG) << "MG_EV_HTTP_PART_BEGIN" << endl;
        mg_file_upload_handler(pConnection, nEvent, pData, file_upload);
        break;
    case MG_EV_HTTP_PART_DATA:
        Log::Get(Log::LOG_DEBUG) << "MG_EV_HTTP_PART_DATA" << endl;
        mg_file_upload_handler(pConnection, nEvent, pData, file_upload);
        break;
    case MG_EV_HTTP_PART_END:
        Log::Get(Log::LOG_DEBUG) << "MG_EV_HTTP_PART_END" << endl;
        mg_file_upload_handler(pConnection, nEvent, pData, file_upload);
        EndUpload(pConnection);
    case 0:
        break;
    }
}


MongooseThread::MongooseThread() : m_pConnection(0)
{

}

MongooseThread& MongooseThread::Get()
{
    static MongooseThread mong;
    return mong;
}

void MongooseThread::Init(iniManager& iniConfig)
{

    m_sIniPath = CreatePath(iniConfig.GetIniString("paths", "inifiles","."));

    //check for ssl
    string sCert = iniConfig.GetIniString("ssl", "cert", "");
    string sKey = iniConfig.GetIniString("ssl", "key", "");


    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    stringstream ssRewrite;
    ssRewrite << "%80=https://" << hostname;

    s_ServerOpts.document_root = iniConfig.GetIniString("paths", "webpages","html").c_str();
    s_ServerOpts.enable_directory_listing = "yes";
    s_ServerOpts.url_rewrites=ssRewrite.str().c_str();
    s_ServerOpts.extra_headers="X-Frame-Options: sameorigin\r\nCache-Control: no-cache\r\nStrict-Transport-Security: max-age=31536000; includeSubDomains\r\nX-Content-Type-Options: nosniff\r\nReferrer-Policy: no-referrer\r\nServer: unknown";

    mg_mgr_init(&m_mgr, NULL);

    stringstream ss;
    ss << iniConfig.GetIniInt("webserver", "port", 8080);

    if(!sCert.empty() && !sKey.empty())
    {
        const char* err;
        struct mg_bind_opts bind_opts;
        memset(&bind_opts, 0, sizeof(bind_opts));
        bind_opts.ssl_cert = sCert.c_str();
        bind_opts.ssl_key = sKey.c_str();
        bind_opts.error_string = &err;
        bind_opts.ssl_cipher_suites = "ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256";


        m_pConnection = mg_bind_opt(&m_mgr, ss.str().c_str(), ev_handler, bind_opts);
        if(m_pConnection == NULL)
        {
            Log::Get(Log::LOG_ERROR) << err << endl;
        }
    }
    else
    {
        m_pConnection = mg_bind(&m_mgr, ss.str().c_str(), ev_handler);
    }



    if(m_pConnection)
    {
        m_pConnection->user_data = reinterpret_cast<void*>(this);

        Log::Get(Log::LOG_INFO) << m_pConnection->user_data << endl;
        Log::Get(Log::LOG_INFO) << s_ServerOpts.document_root << endl;
        Log::Get(Log::LOG_INFO) << "--------------------------" << endl;
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << "Could not start webserver" << endl;
    }
}

void MongooseThread::Run()
{
    if(m_pConnection)
    {
        thread th(&MongooseThread::Loop, this);
        th.detach();
    }
}

void MongooseThread::Loop()
{
    if(m_pConnection)
    {
        mg_set_protocol_http_websocket(m_pConnection);
        while (true)
        {
            mg_mgr_poll(&m_mgr, 250);
            SendStatus();
        }
        mg_mgr_free(&m_mgr);
    }
}


void MongooseThread::SendStatus()
{
    lock_guard<mutex> lg(m_mutex);
    if(m_pConnection)
    {
    }
}



void MongooseThread::SendError(mg_connection* pConnection, const string& sError, int nCode)
{
    Json::Value jsValue;
    jsValue["error"] = sError;
    jsValue["code"] = nCode;

    DoReply(pConnection, nCode, jsValue);
}


bool MongooseThread::UploadAllowed(mg_connection* pConnection, http_message* pMessage)
{
    // @todo check not already uploading/checking a file
    // @todo Check the endpoint is allowed

    Log::Get(Log::LOG_INFO) << "Starting upload" << endl;
    return true;
}

void MongooseThread::EndUpload(mg_connection *pConnection)
{
    // @todo  get the endpoint so we know what should have been uploaded
    Log::Get(Log::LOG_INFO) << "Finished upload" << endl;

    //copy the file...
//    stringstream ssFile;
//    ssFile << m_sNsFilePath << "network_" << m_iniNs.GetIniInt("pending", "version",0) << ".src";
//    ifstream src("/tmp/ns_temp.src", ios::binary);
//    ofstream dst(ssFile.str(), ios::binary);
//    dst << src.rdbuf();
//    dst.close();
//    src.close();
//    remove("/tmp/ns_temp.src");

}

void MongooseThread::DoReply(mg_connection* pConnection, int nCode, const Json::Value& jsonResponse, const std::string& sContentType)
{
    Log::Get() << "MongooseThread::DoReply " << jsonResponse << endl;

    stringstream ssHeaders;
    ssHeaders << "HTTP/1.1 " << nCode << "\r\n"
              << "Transfer-Encoding: chunked\r\n"
              << "Content-Type: " << sContentType << "\r\n"
              << "Access-Control-Allow-Origin:*\r\n"
              << "Access-Control-Allow-Methods:GET, PUT, POST, HEAD, OPTIONS, DELETE\r\n"
              << "Access-Control-Allow-Headers:Content-Type, Accept\r\n"
              << "Access-Control-Max-AgeL3600\r\n\r\n";
    mg_printf(pConnection, "%s", ssHeaders.str().c_str());

    std::stringstream ssJson;
    ssJson << jsonResponse;
    mg_printf_http_chunk(pConnection, ssJson.str().c_str());
    mg_send_http_chunk(pConnection, "", 0); //send empty chunk to inidicate end
}




void MongooseThread::DoHttpGet(mg_connection* pConnection, const std::string& sUrl)
{
    queue<string> qSplit;
    SplitString(qSplit, sUrl, '/');

    if(qSplit.empty())
    {
        Json::Value jsNode;
        jsNode.append(EP_ROOT);
        DoReply(pConnection, 200, jsNode);
    }
    else if(CmpNoCase(qSplit.front(), EP_ROOT))
    {
        qSplit.pop();
        if(qSplit.empty())
        {
            Json::Value jsNode;
            jsNode.append(EP_FILES);
            jsNode.append(EP_PLAYLISTS);
            jsNode.append(EP_SCHEDULES);
            jsNode.append(EP_POWER);
            jsNode.append(EP_CONFIG);
            jsNode.append(EP_INFO);

            DoReply(pConnection, 200, jsNode);
        }
        else
        {
            if(CmpNoCase(qSplit.front(), EP_FILES))
            {
                qSplit.pop();
                DoGetFiles(pConnection, qSplit);
            }
            else if(CmpNoCase(qSplit.front(), EP_PLAYLISTS))
            {
                qSplit.pop();
                DoGetPlaylists(pConnection, qSplit);
            }
            else if(CmpNoCase(qSplit.front(), EP_SCHEDULES))
            {
                qSplit.pop();
                DoGetSchedules(pConnection, qSplit);
            }
            else if(CmpNoCase(qSplit.front(), EP_CONFIG))
            {
                DoGetConfig(pConnection);
            }
            else if(CmpNoCase(qSplit.front(), EP_INFO))
            {
                DoGetInfo(pConnection);
            }
            else if(CmpNoCase(qSplit.front(), EP_POWER))
            {
                DoGetPower(pConnection);
            }
            else
            {
                SendError(pConnection, "Not Found", 404);
            }
        }
    }
    else
    {
        SendError(pConnection, "Not Found", 404);
    }
}

void MongooseThread::DoHttpPut(mg_connection* pConnection, const std::string& sUrl, http_message* pMessage)
{
    queue<string> qSplit;
    SplitString(qSplit, sUrl, '/');
    if(CmpNoCase(qSplit.front(), EP_STATUS))
    {
        StatusChange(pConnection, pMessage);
    }
    else if(CmpNoCase(qSplit.front(), EP_POWER))
    {
        PowerChange(pConnection, pMessage);
    }
    else if(CmpNoCase(qSplit.front(), EP_CONFIG))
    {
        ConfigChange(pConnection, pMessage);
    }
    else if(qSplit.size() > 1)  //second is the id of the resource
    {
        if(CmpNoCase(qSplit.front(), EP_FILES))
        {
            qSplit.pop();
            FileMove(pConnection, pMessage, qSplit.front());   //1=fileid
        }
        else if(CmpNoCase(qSplit.front(), EP_PLAYLISTS))
        {
            qSplit.pop();
            PlaylistModify(pConnection, pMessage, qSplit.front());   //1=playlistid
        }
        else if(CmpNoCase(qSplit.front(), EP_SCHEDULES))
        {
            qSplit.pop();
            ScheduleModify(pConnection, pMessage, qSplit.front());   //1=schedulid
        }
        else
        {
            SendError(pConnection, "Method not allowed here!", 405);
        }
    }
    else
    {
        SendError(pConnection, "Method not allowed here!", 405);
    }
}

void MongooseThread::DoHttpDelete(mg_connection* pConnection, const std::string& sUrl, http_message* pMessage)
{
    queue<string> qSplit;
    SplitString(qSplit, sUrl, '/');
    if(qSplit.size() > 1)
    {
        if(CmpNoCase(qSplit.front(), EP_FILES))
        {
            qSplit.pop();
            FileDelete(pConnection, pMessage, qSplit.front());   //1=fileid
        }
        else if(CmpNoCase(qSplit.front(), EP_PLAYLISTS))
        {
            qSplit.pop();
            PlaylistDelete(pConnection, pMessage, qSplit.front());   //1=playlistid
        }
        else if(CmpNoCase(qSplit.front(), EP_SCHEDULES))
        {
            qSplit.pop();
            ScheduleDelete(pConnection, pMessage, qSplit.front());   //1=schedulid
        }
        else
        {
            SendError(pConnection, "Method not allowed here!", 405);
        }
    }
    else
    {
        SendError(pConnection, "Method not allowed here!", 405);
    }
}

void MongooseThread::DoHttpPost(mg_connection* pConnection, const std::string& sUrl, http_message* pMessage)
{
    queue<string> qSplit;
    SplitString(qSplit, sUrl, '/');
    if(CmpNoCase(qSplit.front(), EP_FILES))
    {
        FileUpload(pConnection, pMessage);
    }
    else if(CmpNoCase(qSplit.front(), EP_PLAYLISTS))
    {
        PlaylistUpload(pConnection, pMessage);
    }
    else if(CmpNoCase(qSplit.front(), EP_SCHEDULES))
    {
        ScheduleUpload(pConnection, pMessage);
    }
    else
    {
        SendError(pConnection, "Method not allowed here!", 405);
    }

}



void MongooseThread::DoGetFiles(mg_connection* pConnection, std::queue<std::string>& qSplit)
{
    if(qSplit.empty())
    {
        //get a list of all the files
    }
    else if(qSplit.size() == 1)
    {
        //return details of particular file
    }
    else
    {
        SendError(pConnection, "Not Found!", 404);
    }
}

void MongooseThread::DoGetPlaylists(mg_connection* pConnection, std::queue<std::string>& qSplit)
{
    if(qSplit.empty())
    {
        //get a list of all the files
    }
    else if(qSplit.size() == 1)
    {
        //return details of particular file
    }
    else
    {
        SendError(pConnection, "Not Found!", 404);
    }
}

void MongooseThread::DoGetSchedules(mg_connection* pConnection, std::queue<std::string>& qSplit)
{
    if(qSplit.empty())
    {
        //get a list of all the files
    }
    else if(qSplit.size() == 1)
    {
        //return details of particular file
    }
    else
    {
        SendError(pConnection, "Not Found!", 404);
    }
}

void MongooseThread::DoGetConfig(mg_connection* pConnection)
{

}

void MongooseThread::DoGetInfo(mg_connection* pConnection)
{

}

void MongooseThread::DoGetPower(mg_connection* pConnection)
{

}


void MongooseThread::StatusChange(mg_connection* pConnection, http_message* pMessage)
{

}

void MongooseThread::PowerChange(mg_connection* pConnection, http_message* pMessage)
{

}

void MongooseThread::ConfigChange(mg_connection* pConnection, http_message* pMessage)
{

}

void MongooseThread::FileMove(mg_connection* pConnection, http_message* pMessage, const std::string& sUid)
{

}

void MongooseThread::PlaylistModify(mg_connection* pConnection, http_message* pMessage, const std::string& sUid)
{

}

void MongooseThread::ScheduleModify(mg_connection* pConnection, http_message* pMessage, const std::string& sUid)
{

}



void MongooseThread::FileDelete(mg_connection* pConnection, http_message* pMessage, const std::string& sUid)
{

}

void MongooseThread::PlaylistDelete(mg_connection* pConnection, http_message* pMessage, const std::string& sUid)
{

}

void MongooseThread::ScheduleDelete(mg_connection* pConnection, http_message* pMessage, const std::string& sUid)
{

}


void MongooseThread::FileUpload(mg_connection* pConnection, http_message* pMessage)
{

}

void MongooseThread::PlaylistUpload(mg_connection* pConnection, http_message* pMessage)
{

}

void MongooseThread::ScheduleUpload(mg_connection* pConnection, http_message* pMessage)
{

}


