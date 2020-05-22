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
#include "resourcemanager.h"


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

static mg_str file_upload(mg_connection*, mg_str file_name)
{
    std::cout <<"File_upload: " << file_name.len << std::endl;
    // @todo need to get the endpoint to     see what the file type is...
    std::string strFile("/tmp/"+CreateGuid());

    char* filename= reinterpret_cast<char*>(malloc(strFile.length()+1));
    strcpy(filename, strFile.c_str());
    return mg_mk_str(filename);
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
        //find the callback function assigned to the method and url
        auto itCallback = m_mEndpoints.find(endpoint(method(sMethod),url(sUri)));
        if(itCallback != m_mEndpoints.end())
        {
            DoReply(pConnection, itCallback->second(pConnection, pMessage, sUri));
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
                pConnection->user_data = 0;
            }
            Log::Get(Log::LOG_DEBUG) << "MongooseServer\tDone" << endl;
            break;
        case MG_EV_HTTP_MULTIPART_REQUEST:
            if(UploadAllowed(pConnection, reinterpret_cast<http_message*>(pData)))
            {
                //@todo store endpoint for later
            }
            else
            {
                // @todo send a not allowed to upload to this endpoint error
            }
            break;
        case MG_EV_HTTP_PART_BEGIN:
            mg_file_upload_handler(pConnection, nEvent, pData, file_upload);
            break;
        case MG_EV_HTTP_PART_DATA:
            mg_file_upload_handler(pConnection, nEvent, pData, file_upload);
            break;
        case MG_EV_HTTP_PART_END:
            mg_file_upload_handler(pConnection, nEvent, pData, file_upload);
            EndUpload(pConnection, pData);
            break;
        case MG_EV_HTTP_MULTIPART_REQUEST_END:
            Log::Get() << "Request end" << std::endl;
            //@todo process request;
            break;

        case 0:
            break;
        }
}



MongooseServer::MongooseServer(const iniManager& iniConfig) :
    m_pConnection(nullptr)
{

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

void MongooseServer::Run()
{
    if(m_pConnection)
    {
        thread th(&MongooseServer::Loop, this);
        th.detach();
    }
}

void MongooseServer::Loop()
{
    if(m_pConnection)
    {
        mg_set_protocol_http_websocket(m_pConnection);
        while (true)
        {
            mg_mgr_poll(&m_mgr, 250);
        }
        mg_mgr_free(&m_mgr);
    }
}


bool MongooseServer::AddEndpoint(const endpoint& theEndpoint, std::function<response(mg_connection*, http_message*, const std::string& )> func)
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
    theResponse.jsonData["error"] = sError;
    theResponse.jsonData["code"] = nCode;

    DoReply(pConnection, theResponse);
}


bool MongooseServer::UploadAllowed(mg_connection* pConnection, http_message* pMessage)
{
    // @todo check not already uploading/checking a file
    // @todo Check the endpoint is allowed

    Log::Get(Log::LOG_INFO) << "Starting upload" << endl;
    string sUri;
    sUri.assign(pMessage->uri.p, pMessage->uri.len);

    string sMethod(pMessage->method.p);
    Log::Get() << sMethod << std::endl;
    size_t nSpace = sMethod.find(' ');
    sMethod = sMethod.substr(0, nSpace);

    Log::Get() << "MongooseServer\tEndpoint: <" << sMethod << ", " << sUri << ">" << std::endl;

    return true;
}

void MongooseServer::EndUpload(mg_connection *pConnection, void* pData)
{
    // @todo  get the endpoint so we know what should have been uploaded
    Log::Get(Log::LOG_INFO) << "Finished upload" << endl;

    mg_http_multipart_part *mp = (mg_http_multipart_part *) pData;
    std::cout << mp->file_name << " : " << mp->var_name << std::endl;

    //copy the file...
//    stringstream ssFile;
//    ssFile << m_sNsFilePath << "network_" << m_iniNs.GetIniInt("pending", "version",0) << ".src";
 //   ifstream src("/tmp/ns_temp.src", ios::binary);
//    ofstream dst(ssFile.str(), ios::binary);
//    dst << src.rdbuf();
//    dst.close();
//    src.close();
//    remove("/tmp/ns_temp.src");

}

void MongooseServer::DoReply(mg_connection* pConnection,const response& theResponse)
{
    Log::Get() << "MongooseServer::DoReply " << theResponse.nHttpCode << endl;
    Log::Get() << "MongooseServer::DoReply " << theResponse.jsonData << endl;

    stringstream ssHeaders;
    ssHeaders << "HTTP/1.1 " << theResponse.nHttpCode << "\r\n"
              << "Transfer-Encoding: chunked\r\n"
              << "Content-Type: " << "application/json" << "\r\n"
              << "Access-Control-Allow-Origin:*\r\n"
              << "Access-Control-Allow-Methods:GET, PUT, POST, HEAD, OPTIONS, DELETE\r\n"
              << "Access-Control-Allow-Headers:Content-Type, Accept\r\n"
              << "Access-Control-Max-AgeL3600\r\n\r\n";
    mg_printf(pConnection, "%s", ssHeaders.str().c_str());

    std::stringstream ssJson;
    ssJson << theResponse.jsonData;
    mg_printf_http_chunk(pConnection, ssJson.str().c_str());
    mg_send_http_chunk(pConnection, "", 0); //send empty chunk to inidicate end
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
            ssHeaders << " " << itOption->second.Get();
        }
        ssHeaders << "\r\n"
                  << "Access-Control-Allow-Headers:Content-Type, Accept\r\n"
                  << "Access-Control-Max-AgeL3600\r\n\r\n";
        mg_printf(pConnection, "%s", ssHeaders.str().c_str());

        mg_send_http_chunk(pConnection, "", 0); //send empty chunk to inidicate end
    }
}
