#include "core.h"
#include "resourcemanager.h"
#include "sysinfomanager.h"
#include <functional>
#include <vector>
#include "utils.h"
#include "json/json.h"
#include <sstream>
#include "log.h"
#include <spawn.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std::placeholders;
using namespace pml;

const std::string Core::ROOT        = "/";          //GET
const std::string Core::EPI         = "x-epi";          //GET
const std::string Core::STATUS      = "status";         //GET PUT (Play or Stop)
const std::string Core::POWER       = "power";          //PUT= set power
const std::string Core::CONFIG      = "config";         //GET PUT
const std::string Core::SCHEDULES   = "schedules";      //GET
const std::string Core::PLAYLISTS   = "playlists";      //GET
const std::string Core::FILES       = "files";          //GET
const std::string Core::INFO        = "info";        //GET

const url Core::EP_ROOT        = url(ROOT);
const url Core::EP_EPI         = url(ROOT+EPI);
const url Core::EP_STATUS      = url(EP_EPI.Get()+"/"+STATUS);
const url Core::EP_POWER       = url(EP_EPI.Get()+"/"+POWER);
const url Core::EP_CONFIG      = url(EP_EPI.Get()+"/"+CONFIG);
const url Core::EP_SCHEDULES   = url(EP_EPI.Get()+"/"+SCHEDULES);
const url Core::EP_PLAYLISTS   = url(EP_EPI.Get()+"/"+PLAYLISTS);
const url Core::EP_FILES       = url(EP_EPI.Get()+"/"+FILES);
const url Core::EP_INFO        = url(EP_EPI.Get()+"/"+INFO);


Core::Core() : m_manager(m_launcher)
{
    pml::Log::Get().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));
    pml::Log::Get() << "Core\tStart" << std::endl;
}

void Core::Run()
{
    iniManager ini;
    // @todo(martim01) load the config file properly


    m_manager.Init(ini);
    m_info.SetDiskPath(m_manager.GetAudioPath());

    if(m_server.Init(ini))
    {
        CreateEndpoints();

        m_launcher.AddCallbacks(std::bind(&Core::StatusCallback, this, _1), std::bind(&Core::ExitCallback, this, _1));

        m_server.Run(true,50);   //start the webserver

        //loop forever
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            m_server.SendWebsocketMessage(m_info.GetInfo());
        }
    }
}

bool Core::CreateEndpoints()
{

    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "CreateEndpoints" << std::endl;

    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_ROOT), std::bind(&Core::GetRoot, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_EPI), std::bind(&Core::GetEpi, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_STATUS), std::bind(&Core::GetStatus, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_POWER), std::bind(&Core::GetPower, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_CONFIG), std::bind(&Core::GetConfig, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_SCHEDULES), std::bind(&Core::GetSchedules, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_PLAYLISTS), std::bind(&Core::GetPlaylists, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_FILES), std::bind(&Core::GetFiles, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_INFO), std::bind(&Core::GetInfo, this, _1,_2,_3,_4));

    m_server.AddEndpoint(endpoint(MongooseServer::PUT, EP_STATUS), std::bind(&Core::PutStatus, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::PUT, EP_POWER), std::bind(&Core::PutPower, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::PUT, EP_CONFIG), std::bind(&Core::PutConfig, this, _1,_2,_3,_4));

    m_server.AddEndpoint(endpoint(MongooseServer::POST, EP_SCHEDULES), std::bind(&Core::PostSchedule, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::POST, EP_PLAYLISTS), std::bind(&Core::PostPlaylist, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::POST, EP_FILES), std::bind(&Core::PostFile, this, _1,_2,_3,_4));


    //now add all the dynamic endpoints
    for(auto itFile = m_manager.GetFilesBegin(); itFile != m_manager.GetFilesEnd(); ++itFile)
    {
        url theUrl(url(EP_FILES.Get()+"/"+itFile->first));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Core::GetFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Core::PutFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Core::DeleteFile, this, _1,_2,_3,_4));
    }

    for(auto itPlaylist = m_manager.GetPlaylistsBegin(); itPlaylist != m_manager.GetPlaylistsEnd(); ++itPlaylist)
    {
        url theUrl(url(EP_PLAYLISTS.Get()+"/"+itPlaylist->first));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Core::GetPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Core::PutPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Core::DeletePlaylist, this, _1,_2,_3,_4));
    }


    for(auto itSchedule = m_manager.GetSchedulesBegin(); itSchedule != m_manager.GetSchedulesEnd(); ++itSchedule)
    {
        url theUrl(url(EP_SCHEDULES.Get()+"/"+itSchedule->first));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Core::GetSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Core::PutSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Core::DeleteSchedule, this, _1,_2,_3,_4));
    }



    return true;
}

response Core::GetRoot(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetRoot" << std::endl;
    response theResponse;
    theResponse.jsonData.append(EPI);
    return theResponse;
}

response Core::GetEpi(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetEpi" << std::endl;
    response theResponse;
    theResponse.jsonData.append(FILES);
    theResponse.jsonData.append(PLAYLISTS);
    theResponse.jsonData.append(SCHEDULES);
    theResponse.jsonData.append(POWER);
    theResponse.jsonData.append(CONFIG);
    theResponse.jsonData.append(INFO);
    theResponse.jsonData.append(STATUS);
    return theResponse;
}

response Core::GetFiles(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetFiles" << std::endl;
    return m_manager.GetFiles();
}

response Core::GetPlaylists(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetPlaylists" << std::endl;
    return m_manager.GetPlaylists();
}

response Core::GetSchedules(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetSchedules" << std::endl;
    return m_manager.GetSchedules();
}

response Core::GetConfig(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetConfig" << std::endl;
    response theResponse;

    // @todo(martim01) GetConfig

    return theResponse;
}

response Core::GetStatus(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    //lock as jsStatus can be called by pipe thread and server thread
    std::lock_guard<std::mutex> lg(m_mutex);

    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetStatus" << std::endl;
    response theResponse;
    theResponse.jsonData = m_jsStatus;
    return theResponse;
}


response Core::GetInfo(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetInfo" << std::endl;

    response theResponse;
    theResponse.jsonData = m_info.GetInfo();


    return theResponse;
}

response Core::GetPower(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetPower" << std::endl;
    response theResponse;
    theResponse.jsonData["status"] = "On";

    return theResponse;
}

response Core::GetFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetFile" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));
    return m_manager.GetFile(vSplit.back());
}

response Core::GetPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetPlaylist" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.GetPlaylist(vSplit.back());
}

response Core::GetSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetSchedule" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.GetSchedule(vSplit.back());
}


response Core::PutStatus(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutStatus" << std::endl;
    Json::Value jsData(ConvertToJson(theData.Get()));

    return m_manager.ModifyStatus(jsData);
}

response Core::PutPower(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutPower" << std::endl;
    response theResponse;

    Json::Value jsData(ConvertToJson(theData.Get()));
    if(jsData["command"].isString() == false)
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["result"] = "No command sent";
        Log::Get(Log::LOG_ERROR) << " no command sent" << std::endl;
    }
    else if(CmpNoCase(jsData["command"].asString(), "restart server"))
    {
        // @todo(martim01) restart the server
        // send a message to "main" to exit the loop
        Log::Get(Log::LOG_INFO) << " restart server" << std::endl;
    }
    else if(CmpNoCase(jsData["command"].asString(), "restart os"))
    {
        // @todo(martim01) restart the os
        // send a message to main to exit the loop and restart the os
        Log::Get(Log::LOG_INFO) << " restart os" << std::endl;
    }
    else if(CmpNoCase(jsData["command"].asString(), "shutdown"))
    {
        // @todo(martim01) shutdown
        // send a message to main to exit the loop and shutdown the os
        Log::Get(Log::LOG_INFO) << " shutdown" << std::endl;
    }
    else
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["result"] = "Invalid command sent";
        Log::Get(Log::LOG_ERROR) << "'" << jsData["command"].asString() <<"' is not a valid command" << std::endl;
    }

    return theResponse;
}

response Core::PutConfig(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutConfig" << std::endl;
    response theResponse(501);
    // @todo(martim01) PutConfig
    // not sure yet

    return theResponse;
}

response Core::PutFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutFile" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.ModifyFile(vSplit.back(), ConvertToJson(theData.Get()));

}

response Core::PutPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutPlaylist" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.ModifyPlaylist(vSplit.back(), ConvertToJson(theData.Get()));
}

response Core::PutSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutSchedule" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    return m_manager.ModifySchedule(vSplit.back(), ConvertToJson(theData.Get()));
}


response Core::DeleteFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "DeleteFile" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    response theResponse(m_manager.DeleteFile(vSplit.back()));
    if(theResponse.nHttpCode == 200)
    {
        url theUrl(url(EP_FILES.Get()+"/"+vSplit.back()));

        m_server.DeleteEndpoint(endpoint(MongooseServer::GET, theUrl));
        m_server.DeleteEndpoint(endpoint(MongooseServer::PUT, theUrl));
        m_server.DeleteEndpoint(endpoint(MongooseServer::DELETE, theUrl));
    }
    return theResponse;
}

response Core::DeletePlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
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

response Core::DeleteSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
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


response Core::PostFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PostFile" << std::endl;
    Json::Value jsonData(ConvertToJson(theData.Get()));

    response theResponse(m_manager.AddFiles(jsonData));
    if(theResponse.nHttpCode == 201)
    {
        url theUrl(url(EP_FILES.Get()+"/"+theResponse.jsonData["uid"].asString()));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Core::GetFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Core::PutFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Core::DeleteFile, this, _1,_2,_3,_4));
    }


    return theResponse;
}

response Core::PostPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PostPlaylist" << std::endl;
    response theResponse(m_manager.AddPlaylist(ConvertToJson(theData.Get())));
    if(theResponse.nHttpCode == 201)
    {
        url theUrl(url(EP_PLAYLISTS.Get()+"/"+theResponse.jsonData["uid"].asString()));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Core::GetPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Core::PutPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE, theUrl), std::bind(&Core::DeletePlaylist, this, _1,_2,_3,_4));
    }
    return theResponse;
}

response Core::PostSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PostSchedule" << std::endl;
    response theResponse(m_manager.AddSchedule(ConvertToJson(theData.Get())));
    if(theResponse.nHttpCode == 201)
    {
        url theUrl(url(EP_SCHEDULES.Get()+"/"+theResponse.jsonData["uid"].asString()));
        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Core::GetSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PUT, theUrl), std::bind(&Core::PutSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::DELETE,theUrl), std::bind(&Core::DeleteSchedule, this, _1,_2,_3,_4));
    }
    return theResponse;
}


void Core::StatusCallback(const std::string& sData)
{
    //lock as jsStatus can be called by pipe thread and server thread
    std::lock_guard<std::mutex> lg(m_mutex);

    m_jsStatus = Json::Value(Json::objectValue);    //reset

    m_jsStatus["player"] ="running";
    m_jsStatus["status"] = ConvertToJson(sData);
    m_server.SendWebsocketMessage(m_jsStatus);
}

void Core::ExitCallback(int nExit)
{
    pml::Log::Get() << "Player exited" << std::endl;
    // unlock resources
    m_manager.LockPlayingResource(false);

    //lock as jsStatus can be called by pipe thread and server thread
    std::lock_guard<std::mutex> lg(m_mutex);

    m_jsStatus = Json::Value(Json::objectValue);    //reset

    m_jsStatus["player"] = "stopped";
    if(WIFEXITED(nExit))
    {
        m_jsStatus["exit"]["code"] = WEXITSTATUS(nExit);
    }
    if(WIFSIGNALED(nExit))
    {
        m_jsStatus["exit"]["signal"]["code"] = WTERMSIG(nExit);
        m_jsStatus["exit"]["signal"]["description"] = strsignal(WTERMSIG(nExit));
        if(WCOREDUMP(nExit))
        {
            m_jsStatus["exit"]["core_dump"] = true;
        }
    }
    if(WIFSTOPPED(nExit))
    {
        m_jsStatus["stopped"]["signal"] = WSTOPSIG(nExit);
    }
    if(WIFCONTINUED(nExit))
    {
        m_jsStatus["resumed"]["signal"] = WSTOPSIG(nExit);
    }

    m_server.SendWebsocketMessage(m_jsStatus);
}
