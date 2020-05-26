#include "endpoints.h"
#include "resourcemanager.h"
#include <functional>
#include <vector>
#include "utils.h"
#include "json/json.h"
#include <sstream>
#include "log.h"

using namespace std::placeholders;
using namespace pml;

const std::string Endpoints::ROOT        = "/";          //GET
const std::string Endpoints::EPI         = "x-epi";          //GET
const std::string Endpoints::STATUS      = "status";         //GET PUT (Play or Stop)
const std::string Endpoints::POWER       = "power";          //PUT= set power
const std::string Endpoints::CONFIG      = "config";         //GET PUT
const std::string Endpoints::SCHEDULES   = "schedules";      //GET
const std::string Endpoints::PLAYLISTS   = "playlists";      //GET
const std::string Endpoints::FILES       = "files";          //GET
const std::string Endpoints::INFO        = "info";        //GET

const url Endpoints::EP_ROOT        = url(ROOT);
const url Endpoints::EP_EPI         = url(ROOT+EPI);
const url Endpoints::EP_STATUS      = url(EP_EPI.Get()+"/"+STATUS);
const url Endpoints::EP_POWER       = url(EP_EPI.Get()+"/"+POWER);
const url Endpoints::EP_CONFIG      = url(EP_EPI.Get()+"/"+CONFIG);
const url Endpoints::EP_SCHEDULES   = url(EP_EPI.Get()+"/"+SCHEDULES);
const url Endpoints::EP_PLAYLISTS   = url(EP_EPI.Get()+"/"+PLAYLISTS);
const url Endpoints::EP_FILES       = url(EP_EPI.Get()+"/"+FILES);
const url Endpoints::EP_INFO        = url(EP_EPI.Get()+"/"+INFO);


Endpoints::Endpoints(ResourceManager& manager, MongooseServer& server) :
    m_manager(manager),
    m_server(server)
{

}

bool Endpoints::CreateEndpoints()
{

    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "CreateEndpoints" << std::endl;

    //m_server.AddEndpoint(endpoint(MongooseServer::GET, url(EP_ROOT), std::bind(&Endpoints::GetRoot, this, _1,_2,_3));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_EPI), std::bind(&Endpoints::GetEpi, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_STATUS), std::bind(&Endpoints::GetStatus, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_POWER), std::bind(&Endpoints::GetPower, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_CONFIG), std::bind(&Endpoints::GetConfig, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_SCHEDULES), std::bind(&Endpoints::GetSchedules, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_PLAYLISTS), std::bind(&Endpoints::GetPlaylists, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_FILES), std::bind(&Endpoints::GetFiles, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_INFO), std::bind(&Endpoints::GetInfo, this, _1,_2,_3,_4));

    m_server.AddEndpoint(endpoint(MongooseServer::PUT, EP_STATUS), std::bind(&Endpoints::PutStatus, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::PUT, EP_POWER), std::bind(&Endpoints::PutPower, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::PUT, EP_CONFIG), std::bind(&Endpoints::PutConfig, this, _1,_2,_3,_4));

    m_server.AddEndpoint(endpoint(MongooseServer::POST, EP_SCHEDULES), std::bind(&Endpoints::PostSchedule, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::POST, EP_PLAYLISTS), std::bind(&Endpoints::PostPlaylist, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::POST, EP_FILES), std::bind(&Endpoints::PostFile, this, _1,_2,_3,_4));


    //now add all the dynamic endpoints
    for(auto itFile = m_manager.GetFilesBegin(); itFile != m_manager.GetFilesEnd(); ++itFile)
    {
        url theUrl(url(EP_FILES.Get()+"/"+itFile->first));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Endpoints::GetFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Endpoints::PutFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Endpoints::DeleteFile, this, _1,_2,_3,_4));
    }

    for(auto itPlaylist = m_manager.GetPlaylistsBegin(); itPlaylist != m_manager.GetPlaylistsEnd(); ++itPlaylist)
    {
        url theUrl(url(EP_PLAYLISTS.Get()+"/"+itPlaylist->first));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Endpoints::GetPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Endpoints::PutPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Endpoints::DeletePlaylist, this, _1,_2,_3,_4));
    }


    for(auto itSchedule = m_manager.GetSchedulesBegin(); itSchedule != m_manager.GetSchedulesEnd(); ++itSchedule)
    {
        url theUrl(url(EP_SCHEDULES.Get()+"/"+itSchedule->first));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Endpoints::GetSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Endpoints::PutSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Endpoints::DeleteSchedule, this, _1,_2,_3,_4));
    }

    return true;
}

response Endpoints::GetRoot(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetRoot" << std::endl;
    response theResponse;
    theResponse.jsonData.append(EPI);
    return theResponse;
}

response Endpoints::GetEpi(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetEpi" << std::endl;
    response theResponse;
    theResponse.jsonData.append(FILES);
    theResponse.jsonData.append(PLAYLISTS);
    theResponse.jsonData.append(SCHEDULES);
    theResponse.jsonData.append(POWER);
    theResponse.jsonData.append(CONFIG);
    theResponse.jsonData.append(INFO);
    return theResponse;
}

response Endpoints::GetFiles(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetFiles" << std::endl;
    return m_manager.GetFiles();
}

response Endpoints::GetPlaylists(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetPlaylists" << std::endl;
    return m_manager.GetPlaylists();
}

response Endpoints::GetSchedules(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetSchedules" << std::endl;
    return m_manager.GetSchedules();
}

response Endpoints::GetConfig(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetConfig" << std::endl;
    response theResponse;

    // @todo GetConfig

    return theResponse;
}

response Endpoints::GetStatus(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetStatus" << std::endl;
    response theResponse;

    // @todo GetStatus
    // Playing or not.
    // If playing then the file and time of playing and type of playing (file, schedule or playlist)
    return theResponse;
}


response Endpoints::GetInfo(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetInfo" << std::endl;
    response theResponse;

    // @todo GetInfo
    // version number of websever and player

    return theResponse;
}

response Endpoints::GetPower(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetPower" << std::endl;
    response theResponse;
    // @todo GetPower
    // uptime

    return theResponse;
}

response Endpoints::GetFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetFile" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));
    return m_manager.GetFile(vSplit.back());
}

response Endpoints::GetPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetPlaylist" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.GetPlaylist(vSplit.back());
}

response Endpoints::GetSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetSchedule" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.GetSchedule(vSplit.back());
}


response Endpoints::PutStatus(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutStatus" << std::endl;
    response theResponse(501);

    // @todo PutStatus
    // play, pause, stop
    // file/schedule playlist
    // uid

    return theResponse;
}

response Endpoints::PutPower(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutPower" << std::endl;
    response theResponse(501);

    // @todo PutPower
    // restart server, restart os, shutdown

    return theResponse;
}

response Endpoints::PutConfig(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutConfig" << std::endl;
    response theResponse(501);
    //@todo PutConfig
    // not sure yet

    return theResponse;
}

response Endpoints::PutFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutFile" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.ModifyFile(vSplit.back(), ConvertToJson(theData.Get()));

}

response Endpoints::PutPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutPlaylist" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.ModifyPlaylist(vSplit.back(), ConvertToJson(theData.Get()));
}

response Endpoints::PutSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutSchedule" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.ModifySchedule(vSplit.back(), ConvertToJson(theData.Get()));
}


response Endpoints::DeleteFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "DeleteFile" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    response theResponse(DeleteFile(vSplit.back()));
    if(theResponse.nHttpCode == 200)
    {
        url theUrl(url(EP_FILES.Get()+"/"+vSplit.back()));

        m_server.DeleteEndpoint(endpoint(MongooseServer::GET, theUrl));
        m_server.DeleteEndpoint(endpoint(MongooseServer::PUT, theUrl));
        m_server.DeleteEndpoint(endpoint(MongooseServer::DELETE, theUrl));
    }
    return theResponse;
}

response Endpoints::DeletePlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "DeletePlaylist" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    response theResponse(m_manager.DeletePlaylist(vSplit.back()));

    if(theResponse.nHttpCode == 200)
    {
        url theUrl(url(EP_PLAYLISTS.Get()+"/"+vSplit.back()));

        m_server.DeleteEndpoint(endpoint(MongooseServer::GET, theUrl));
        m_server.DeleteEndpoint(endpoint(MongooseServer::PUT, theUrl));
        m_server.DeleteEndpoint(endpoint(MongooseServer::DELETE, theUrl));
    }
    return theResponse;
}

response Endpoints::DeleteSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "DeleteSchedule" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    response theResponse(m_manager.DeleteSchedule(vSplit.back()));

    if(theResponse.nHttpCode == 200)
    {
        url theUrl(url(EP_SCHEDULES.Get()+"/"+vSplit.back()));

        m_server.DeleteEndpoint(endpoint(MongooseServer::GET, theUrl));
        m_server.DeleteEndpoint(endpoint(MongooseServer::PUT, theUrl));
        m_server.DeleteEndpoint(endpoint(MongooseServer::DELETE, theUrl));
    }
    return theResponse;
}


response Endpoints::PostFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    // @todo currently assuming only 1 file. Allow for multiples...
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PostFile" << std::endl;
    Json::Value jsonData(ConvertToJson(theData.Get()));

    response theResponse(m_manager.AddFiles(jsonData));
    if(theResponse.nHttpCode == 201)
    {
        url theUrl(url(EP_FILES.Get()+"/"+theResponse.jsonData["uid"].asString()));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Endpoints::GetFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Endpoints::PutFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Endpoints::DeleteFile, this, _1,_2,_3,_4));
    }


    return theResponse;
}

response Endpoints::PostPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PostPlaylist" << std::endl;
    response theResponse(m_manager.AddPlaylist(ConvertToJson(theData.Get())));
    if(theResponse.nHttpCode == 201)
    {
        url theUrl(url(EP_PLAYLISTS.Get()+"/"+theResponse.jsonData["uid"].asString()));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Endpoints::GetPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Endpoints::PutPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Endpoints::DeletePlaylist, this, _1,_2,_3,_4));
    }
    return theResponse;
}

response Endpoints::PostSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PostSchedule" << std::endl;
    response theResponse(m_manager.AddSchedule(ConvertToJson(theData.Get())));
    if(theResponse.nHttpCode == 201)
    {
        url theUrl(url(EP_SCHEDULES.Get()+"/"+theResponse.jsonData["uid"].asString()));
        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Endpoints::GetSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Endpoints::PutSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE,theUrl), std::bind(&Endpoints::DeleteSchedule, this, _1,_2,_3,_4));
    }
    return theResponse;
}


