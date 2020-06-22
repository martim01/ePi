#include "mongooseserver.h"
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
#include "uidutils.h"
#include "resourcemanager.h"
#include "sysinfomanager.h"

using namespace std;
using namespace std::placeholders;
using namespace pml;

const method MongooseServer::GET    = method("GET");
const method MongooseServer::POST   = method("POST");
const method MongooseServer::PUT    = method("PUT");
const method MongooseServer::PATCH  = method("PATCH");
const method MongooseServer::DELETE = method("DELETE");
const method MongooseServer::OPTIONS = method("OPTIONS");


static mg_serve_http_opts s_ServerOpts;

bool operator<(const endpoint e1, const endpoint& e2)
{
    return (e1.first.Get() < e2.first.Get() || (e1.first.Get() == e2.first.Get() && e1.second.Get() < e2.second.Get()));
}




map<string, string> DecodeQueryString(http_message* pMessage)
{
    map<string, string> mDecode;

    vector<string> vQuery(SplitString(pMessage->query_string.p, '&'));
    for(size_t i = 0; i < vQuery.size(); i++)
    {
        vector<string> vValue(SplitString(vQuery[i], '='));
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

    MongooseServer* pThread = reinterpret_cast<MongooseServer*>(pConnection->user_data);
    pThread->HandleEvent(pConnection, nEvent, pData);
}


void MongooseServer::EventWebsocket(mg_connection *pConnection, int nEvent, void* pData)
{

    http_message* pMessage = reinterpret_cast<http_message*>(pData);

    Log::Get(Log::LOG_INFO) << "MG_EV_WEBSOCKET_HANDSHAKE_DONE" << endl;

    string sUri;
    sUri.assign(pMessage->uri.p, pMessage->uri.len);


}

void MongooseServer::EventHttp(mg_connection *pConnection, int nEvent, void* pData)
{
    http_message* pMessage = reinterpret_cast<http_message*>(pData);
    string sUri;
    sUri.assign(pMessage->uri.p, pMessage->uri.len);

    if(sUri[sUri.length()-1] == '/')    //get rid of trailling /
    {
        sUri = sUri.substr(0, sUri.length()-1);
    }

    string sMethod(pMessage->method.p);
    size_t nSpace = sMethod.find(' ');
    sMethod = sMethod.substr(0, nSpace);

    Log::Get() << "MongooseServer\tEndpoint: <" << sMethod << ", " << sUri << ">" << std::endl;

    if(CmpNoCase(sMethod, OPTIONS.Get()))
    {
        SendOptions(pConnection, sUri);
    }
    else
    {
        std::string sQuery, sData;
        if(pMessage->body.len > 0)
        {
            sData.assign(pMessage->body.p, pMessage->body.len);
        }
        if(pMessage->query_string.len > 0)
        {
            sQuery.assign(pMessage->query_string.p, pMessage->query_string.len);
        }

        //find the callback function assigned to the method and url
        auto itCallback = m_mEndpoints.find(endpoint(method(sMethod),url(sUri)));
        if(itCallback != m_mEndpoints.end())
        {
            DoReply(pConnection, itCallback->second(pConnection, query(sQuery), postData(sData), url(sUri)));
        }
        else
        {   //none found so sne a "not found" error
            SendError(pConnection, "Not Found", 404);
        }
    }
}

void MongooseServer::HandleEvent(mg_connection *pConnection, int nEvent, void* pData)
{
    switch (nEvent)
    {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
            EventWebsocket(pConnection, nEvent, pData);
            break;
        case MG_EV_HTTP_REQUEST:
            EventHttp(pConnection, nEvent, pData);
            break;
        case MG_EV_TIMER:
            break;
        case MG_EV_CLOSE:
            Log::Get(Log::LOG_DEBUG) << "MongooseServer\tEvent Close" << endl;
            if (is_websocket(pConnection))
            {
                pConnection->user_data = nullptr;
            }
            Log::Get(Log::LOG_DEBUG) << "MongooseServer\tDone" << endl;
            break;
        case MG_EV_HTTP_MULTIPART_REQUEST:
            MultipartBegin(pConnection, reinterpret_cast<http_message*>(pData));
            break;
        case MG_EV_HTTP_PART_BEGIN:
            PartBegin(pConnection, reinterpret_cast<mg_http_multipart_part*>(pData));
            break;
        case MG_EV_HTTP_PART_DATA:
            PartData(pConnection, reinterpret_cast<mg_http_multipart_part*>(pData));
            break;
        case MG_EV_HTTP_PART_END:
            PartEnd(pConnection, reinterpret_cast<mg_http_multipart_part*>(pData));
            break;
        case MG_EV_HTTP_MULTIPART_REQUEST_END:
            MultipartEnd(pConnection, reinterpret_cast<mg_http_multipart_part*>(pData));
            break;

        case 0:
            break;
    }
}



MongooseServer::MongooseServer() :
    m_pConnection(nullptr),
    m_nPollTimeout(100),
    m_loopCallback(nullptr),
    m_bLoop(true)
{

    m_multipartData.itEndpoint = m_mEndpoints.end();


}

bool MongooseServer::Init(const iniManager& iniConfig)
{
    //check for ssl
    string sCert = iniConfig.GetIniString("api", "sslCert", "");
    string sKey = iniConfig.GetIniString("api", "ssKey", "");


    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    stringstream ssRewrite;
    ssRewrite << "%80=https://" << hostname;

    s_ServerOpts.document_root = ".";
    s_ServerOpts.enable_directory_listing = "no";
    s_ServerOpts.url_rewrites=ssRewrite.str().c_str();
    s_ServerOpts.extra_headers="X-Frame-Options: sameorigin\r\nCache-Control: no-cache\r\nStrict-Transport-Security: max-age=31536000; includeSubDomains\r\nX-Content-Type-Options: nosniff\r\nReferrer-Policy: no-referrer\r\nServer: unknown";

    mg_mgr_init(&m_mgr, NULL);

    stringstream ss;
    ss << iniConfig.GetIniInt("api", "port", 8080);

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
        return true;
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << "Could not start webserver" << endl;
        return false;
    }
}

void MongooseServer::Run(bool bThread, unsigned int nTimeoutMs)
{
    if(m_pConnection)
    {
        m_nPollTimeout = nTimeoutMs;

        if(bThread)
        {
            thread th(&MongooseServer::Loop, this);
            th.detach();
        }
        else
        {
            Loop();
        }
    }
}

void MongooseServer::Loop()
{
    if(m_pConnection)
    {
        mg_set_protocol_http_websocket(m_pConnection);
        int nCount = 0;
        while (m_bLoop)
        {
            auto now = std::chrono::high_resolution_clock::now();

            mg_mgr_poll(&m_mgr, m_nPollTimeout);

            auto took = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()-now.time_since_epoch());

            if(m_loopCallback)
            {
                m_loopCallback(took.count());
            }
            SendWSQueue();

            ++nCount;
        }
        mg_mgr_free(&m_mgr);
    }
}

void MongooseServer::Stop()
{
    m_bLoop = false;
}

bool MongooseServer::AddEndpoint(const endpoint& theEndpoint, std::function<response(mg_connection*, const query&, const postData&, const url& )> func)
{
    Log::Get(Log::LOG_DEBUG) << "MongooseServer\t" << "AddEndpoint <" << theEndpoint.first.Get() << ", " << theEndpoint.second.Get() << "> ";
    if(m_mEndpoints.find(theEndpoint) != m_mEndpoints.end())
    {
        Log::Get(Log::LOG_WARN) << "failed as endpoint already exists";
        return false;
    }

    m_mEndpoints.insert(std::make_pair(theEndpoint, func));
    m_mmOptions.insert(std::make_pair(theEndpoint.second.Get(), theEndpoint.first));

    Log::Get(Log::LOG_DEBUG) << "success" << std::endl;
    return true;
}

bool MongooseServer::DeleteEndpoint(const endpoint& theEndpoint)
{
    m_mmOptions.erase(theEndpoint.second.Get());
    return (m_mEndpoints.erase(theEndpoint) != 0);
}


void MongooseServer::SendError(mg_connection* pConnection, const string& sError, int nCode)
{
    response theResponse(nCode);
    theResponse.jsonData["result"] = false;
    theResponse.jsonData["reason"].append(sError);
    theResponse.jsonData["code"] = nCode;

    DoReply(pConnection, theResponse);
}

void MongooseServer::ClearMultipartData()
{
    m_multipartData.itEndpoint = m_mEndpoints.end();
    m_multipartData.mData.clear();
    m_multipartData.mFiles.clear();
    if(m_multipartData.ofs.is_open())
    {
        m_multipartData.ofs.close();
    }
}

bool MongooseServer::MultipartBegin(mg_connection* pConnection, http_message* pMessage)
{
    Log::Get(Log::LOG_INFO) << "Starting upload" << endl;

    if(pMessage->message.len > 0)
    {
        Log::Get() << string(pMessage->message.p) << endl;
    }

    string sUri;
    sUri.assign(pMessage->uri.p, pMessage->uri.len);
    string sMethod(pMessage->method.p);
    Log::Get() << sMethod << std::endl;
    size_t nSpace = sMethod.find(' ');
    sMethod = sMethod.substr(0, nSpace);

    Log::Get() << "MongooseServer\tUpload: <" << sMethod << ", " << sUri << ">" << std::endl;

    ClearMultipartData();
    m_multipartData.itEndpoint = m_mEndpoints.find(endpoint(sMethod, sUri));
    if(m_multipartData.itEndpoint == m_mEndpoints.end())
    {
        SendError(pConnection, "Method not allowed.", 405);
        return false;
    }
    else
    {
        return true;
    }
}


bool MongooseServer::PartBegin(mg_connection* pConnection, mg_http_multipart_part* pPart)
{
    Log::Get() << "MongooseServer\tPartBegin: '" << pPart->file_name << "' '" << pPart->var_name << "' " << std::endl;

    if(std::string(pPart->file_name) != "")
    {
        auto pairFile = m_multipartData.mFiles.insert(std::make_pair(std::string(pPart->var_name), "/tmp/"+CreateGuid()));
        m_multipartData.ofs.open(pairFile.first->second, std::ios::binary);
        if(m_multipartData.ofs.is_open() == false)
        {
            ClearMultipartData();
            SendError(pConnection, "Failed to open a file", 500);
            return false;
        }
    }
    else
    {
        m_multipartData.mData.insert(std::make_pair(pPart->var_name, ""));
    }
    return true;
}

bool MongooseServer::PartData(mg_connection* pConnection, mg_http_multipart_part* pPart)
{
    if(std::string(pPart->file_name) != "" && m_multipartData.ofs.is_open())
    {
        m_multipartData.ofs.write(pPart->data.p, pPart->data.len);
        m_multipartData.ofs.flush();
        if(!m_multipartData.ofs)
        {
            ClearMultipartData();
            SendError(pConnection, "Failed to write to file", 500);
            return false;
        }
    }
    else
    {
        auto itData = m_multipartData.mData.find(pPart->var_name);
        if(itData != m_multipartData.mData.end())
        {
            itData->second.append(pPart->data.p, pPart->data.len);
        }
        else
        {
            SendError(pConnection, "Failed to store form data", 500);
            return false;
        }
    }
    return true;
}

bool MongooseServer::PartEnd(mg_connection* pConnection, mg_http_multipart_part* pPart)
{
    if(m_multipartData.ofs.is_open())
    {
        m_multipartData.ofs.close();
    }
    return true;
}


bool MongooseServer::MultipartEnd(mg_connection* pConnection, mg_http_multipart_part* pPart)
{
    Log::Get(Log::LOG_DEBUG) << "MongooseServer\tFinished Multipart" << endl;


    Json::Value jsData;

    for(auto pairData : m_multipartData.mData)
    {
        Log::Get() << "MongooseServer\tMultipart: " << pairData.first << "=" << pairData.second << std::endl;
        jsData[pairData.first] = pairData.second;
    }
    Json::Value jsFiles;
    for(auto pairFiles : m_multipartData.mFiles)
    {
        Log::Get() << "MongooseServer\tMultipart Files: " << pairFiles.first << "=" << pairFiles.second << std::endl;
        jsFiles[pairFiles.first] = pairFiles.second;
    }

    Json::Value jsBody;
    jsBody["multipart"]["data"] = jsData;
    jsBody["multipart"]["files"] = jsFiles;

    std::stringstream ss;
    ss << jsBody;

    if(m_multipartData.itEndpoint != m_mEndpoints.end())
    {
        DoReply(pConnection, m_multipartData.itEndpoint->second(pConnection, query(""), postData(ss.str()), m_multipartData.itEndpoint->first.second));
        return true;
    }
    else
    {
        SendError(pConnection, "Not found", 404);
        return false;
    }
}


void MongooseServer::DoReply(mg_connection* pConnection,const response& theResponse)
{
    Log::Get() << "MongooseServer::DoReply " << theResponse.nHttpCode << endl;
    Log::Get() << "MongooseServer::DoReply " << theResponse.jsonData << endl;

    std::stringstream ssJson;
    ssJson << theResponse.jsonData;

    stringstream ssHeaders;
    ssHeaders << "HTTP/1.1 " << theResponse.nHttpCode << " \r\n"
              << "Content-Type: " << "application/json" << "\r\n"
              << "Content-Length: " << ssJson.str().length() << "\r\n"
              << "Access-Control-Allow-Origin:*\r\n"
              << "Access-Control-Allow-Methods:GET, PUT, POST, HEAD, OPTIONS, DELETE\r\n"
              << "Access-Control-Allow-Headers:Content-Type, Accept\r\n"
              << "Access-Control-Max-AgeL3600\r\n\r\n";

    mg_send(pConnection, ssHeaders.str().c_str(), ssHeaders.str().length());
    mg_send(pConnection, ssJson.str().c_str(), ssJson.str().length());

    pConnection->flags |= MG_F_SEND_AND_CLOSE;

    //mg_send_http_chunk(pConnection, ssJson.str().c_str(), ssJson.str().length());
    //mg_send_http_chunk(pConnection, "", 0); //send empty chunk to inidicate end

}


void MongooseServer::SendOptions(mg_connection* pConnection, const std::string& sUrl)
{
    auto itOption = m_mmOptions.lower_bound(sUrl);
    if(itOption == m_mmOptions.upper_bound(sUrl))
    {
        SendError(pConnection, "Not Found", 404);
    }
    else
    {
        stringstream ssHeaders;
        ssHeaders << "HTTP/1.1 200\r\n"
                  << "Transfer-Encoding: chunked\r\n"
                  << "Content-Type: " << "application/json" << "\r\n"
                  << "Access-Control-Allow-Origin:*\r\n"
                  << "Access-Control-Allow-Methods: OPTIONS";

        for(; itOption != m_mmOptions.upper_bound(sUrl); ++itOption)
        {
            ssHeaders << ", " << itOption->second.Get();
        }
        ssHeaders << "\r\n"
                  << "Access-Control-Allow-Headers:Content-Type, Accept\r\n"
                  << "Access-Control-Max-AgeL3600\r\n\r\n";
        mg_printf(pConnection, "%s", ssHeaders.str().c_str());

        mg_send_http_chunk(pConnection, "", 0); //send empty chunk to inidicate end

    }
}





void MongooseServer::SendWSQueue()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    if(m_pConnection)
    {
        while(m_qWsMessages.empty() == false)
        {
            std::stringstream ssMessage;
            ssMessage << m_qWsMessages.front();

            //turn message into array
            char *cstr = new char[ssMessage.str().length() + 1];
            strcpy(cstr, ssMessage.str().c_str());


            for (mg_connection* pConnection = mg_next(m_pConnection->mgr, NULL); pConnection != NULL; pConnection = mg_next(m_pConnection->mgr, pConnection))
            {
                if(is_websocket(pConnection))
                {
                    mg_send_websocket_frame(pConnection, WEBSOCKET_OP_TEXT, cstr, strlen(cstr));
                }
            }
            delete[] cstr;
            m_qWsMessages.pop();
        }
    }
}


void MongooseServer::SendWebsocketMessage(const Json::Value& jsMessage)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    m_qWsMessages.push(jsMessage);
}

void MongooseServer::SetLoopCallback(std::function<void(unsigned int)> func)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    m_loopCallback = func;
}
