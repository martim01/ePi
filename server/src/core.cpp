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
#include "resource.h"
#include "inimanager.h"
#include "inisection.h"
#include "version.h"
#include "logtofile.h"
#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include "jsonutils.h"
#include "proccheck.h"
#include <chrono>


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
const std::string Core::UPDATE      = "update";        //GET PUT
const std::string Core::OUTPUTS     = "outputs";        //GET PUT

const url Core::EP_ROOT        = url(ROOT);
const url Core::EP_EPI         = url(ROOT+EPI);
const url Core::EP_STATUS      = url(EP_EPI.Get()+"/"+STATUS);
const url Core::EP_POWER       = url(EP_EPI.Get()+"/"+POWER);
const url Core::EP_CONFIG      = url(EP_EPI.Get()+"/"+CONFIG);
const url Core::EP_SCHEDULES   = url(EP_EPI.Get()+"/"+SCHEDULES);
const url Core::EP_PLAYLISTS   = url(EP_EPI.Get()+"/"+PLAYLISTS);
const url Core::EP_FILES       = url(EP_EPI.Get()+"/"+FILES);
const url Core::EP_INFO        = url(EP_EPI.Get()+"/"+INFO);
const url Core::EP_UPDATE      = url(EP_EPI.Get()+"/"+UPDATE);
const url Core::EP_OUTPUTS     = url(EP_EPI.Get()+"/"+OUTPUTS);


Core::Core() : m_manager(m_launcher, m_iniConfig),
   m_jsResources(Json::arrayValue),
   m_nTimeSinceLastCall(0),
   m_nLogToConsole(0),
   m_nLogToFile(0),
   m_bLoggedThisHour(false)
{

    GetInitialPlayerStatus();
}


void Core::GetInitialPlayerStatus()
{
    if(IsProcRunning("player3") || IsProcRunning("player67"))
    {   //just started up so must be an orphaned player
        m_jsStatus["player"] = "Orphaned";
    }
    else
    {
        m_jsStatus["player"] ="Stopped";
    }
}

void Core::InitLogging()
{
    if(m_iniConfig.GetIniInt("logging", "console",0) == 1 && m_nLogToConsole == 0)
    {
        m_nLogToConsole = pml::Log::Get().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));
    }
    else if(m_nLogToConsole != 0)
    {
        pml::Log::Get().RemoveOutput(m_nLogToConsole);
        m_nLogToConsole = 0;
    }

    if(m_iniConfig.GetIniInt("logging", "file",0) == 1 &&  m_nLogToFile == 0)
    {
        m_nLogToFile = pml::Log::Get().AddOutput(std::unique_ptr<pml::LogOutput>(new LogToFile(CreatePath(m_iniConfig.GetIniString("paths","logs","."))+"episerver")));
    }
    else if(m_nLogToFile != 0)
    {
        pml::Log::Get().RemoveOutput(m_nLogToFile);
        m_nLogToFile = 0;
    }

}

void Core::Run(const std::string& sConfigFile)
{

    if(m_iniConfig.ReadIniFile(sConfigFile) == false)
    {
        pml::Log::Get().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));
        pml::Log::Get(pml::Log::LOG_CRITICAL) << "Could not open '" << sConfigFile << "' exiting.";
        return;
    }

    InitLogging();

    pml::Log::Get() << "Core\tStart" << std::endl;

    m_manager.Init();
    m_info.SetDiskPath(m_manager.GetAudioPath());

    if(m_server.Init(m_iniConfig))
    {
        //add server callbacks
        CreateEndpoints();

        //add luauncher callbacks
        m_launcher.AddCallbacks(std::bind(&Core::StatusCallback, this, _1), std::bind(&Core::ExitCallback, this, _1));
        m_launcher.SetPlayer(CreatePath(m_iniConfig.GetIniString("paths", "player", "/home/pi/ePi/bin")), m_iniConfig.GetIniString("playout", "app", "player3"),sConfigFile);
        //start the server loop
        m_server.Run(false,50);
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
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_UPDATE), std::bind(&Core::GetUpdate, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::GET, EP_OUTPUTS), std::bind(&Core::GetOutputs, this, _1,_2,_3,_4));

    m_server.AddEndpoint(endpoint(MongooseServer::PATCH, EP_STATUS), std::bind(&Core::PatchStatus, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::PATCH, EP_CONFIG), std::bind(&Core::PatchConfig, this, _1,_2,_3,_4));

    m_server.AddEndpoint(endpoint(MongooseServer::PUT, EP_POWER), std::bind(&Core::PutPower, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::PUT, EP_UPDATE), std::bind(&Core::PutUpdate, this, _1,_2,_3,_4));

    m_server.AddEndpoint(endpoint(MongooseServer::POST, EP_SCHEDULES), std::bind(&Core::PostSchedule, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::POST, EP_PLAYLISTS), std::bind(&Core::PostPlaylist, this, _1,_2,_3,_4));
    m_server.AddEndpoint(endpoint(MongooseServer::POST, EP_FILES), std::bind(&Core::PostFile, this, _1,_2,_3,_4));


    //now add all the dynamic endpoints
    for(auto itFile = m_manager.GetFilesBegin(); itFile != m_manager.GetFilesEnd(); ++itFile)
    {
        url theUrl(url(EP_FILES.Get()+"/"+itFile->first));

        m_server.AddEndpoint(endpoint(MongooseServer::GET, theUrl), std::bind(&Core::GetFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(endpoint(MongooseServer::PATCH, theUrl), std::bind(&Core::PatchFile, this, _1,_2,_3,_4));
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


    //Add the loop callback function
    m_server.SetLoopCallback(std::bind(&Core::LoopCallback, this, _1));

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
    theResponse.jsonData.append(UPDATE);
    theResponse.jsonData.append(OUTPUTS);
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

    char host[256];
    gethostname(host, 256);
    theResponse.jsonData["server"]["hostname"] = host;
    theResponse.jsonData["server"]["ip_address"]["eth0"] = GetIpAddress("eth0");
    theResponse.jsonData["server"]["ip_address"]["wlan0"] = GetIpAddress("wlan0");

    for(auto itSection = m_iniConfig.GetSectionBegin(); itSection != m_iniConfig.GetSectionEnd(); ++itSection)
    {
        if(itSection->first != "restricted")
        {
            theResponse.jsonData[itSection->first] = Json::Value(Json::objectValue);
            for(auto itData = itSection->second->GetDataBegin(); itData != itSection->second->GetDataEnd(); ++itData)
            {
                theResponse.jsonData[itSection->first][itData->first] = itData->second;
            }
        }
    }

    return theResponse;
}

response Core::GetUpdate(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    //get all the version numbers...
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetUpdate" << std::endl;
    response theResponse;
    std::stringstream ssVersion;
    ssVersion << version::MAJOR << "." << version::MINOR << "." << version::PATCH << "." << version::BUILD;
    theResponse.jsonData["server"]["version"] = ssVersion.str();
    theResponse.jsonData["server"]["date"] = ConvertTimeToIsoString(std::time_t(version::DATE));


    theResponse.jsonData["player3"] = ConvertToJson(exec(CreatePath(m_iniConfig.GetIniString("paths", "player","/home/pi/ePi/bin"))+"player3 -v"));


    //get versions of other applications...
    return theResponse;
}

response Core::GetOutputs(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    //get all the version numbers...
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetOutputs" << std::endl;
    response theResponse;

    //Log::Get(Log::LOG_DEBUG) << "
    theResponse.jsonData = ConvertToJson(exec(CreatePath(m_iniConfig.GetIniString("paths", "player","."))+"player3 -d"));


    //get versions of other applications...
    return theResponse;
}

response Core::GetStatus(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    //lock as jsStatus can be called by pipe thread and server thread
    std::lock_guard<std::mutex> lg(m_mutex);

    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "GetStatus" << std::endl;
    m_jsStatus["locked"] = (m_iniConfig.GetIniInt("restricted", "locked", 0)==1);

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


response Core::PatchStatus(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PatchStatus" << std::endl;

    Json::Value jsData(ConvertToJson(theData.Get()));
    response theResponse(m_manager.ModifyStatus(jsData));

    if(CmpNoCase(jsData["command"].asString(), "kill"))
    {
        GetInitialPlayerStatus();   //see if the players have been killed
    }
    return theResponse;
}

response Core::PutPower(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutPower: " << theData.Get() << std::endl;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode == 423)
    {
        return theResponse;
    }


    Json::Value jsData(ConvertToJson(theData.Get()));
    if(jsData["command"].isString() == false)
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["result"] = "No command sent";
        Log::Get(Log::LOG_ERROR) << " no command sent" << std::endl;
    }
    else if(CmpNoCase(jsData["command"].asString(), "restart server"))
    {
        // send a message to "main" to exit the loop
        Log::Get(Log::LOG_INFO) << " restart server" << std::endl;
        m_server.Stop();
    }
    else if(CmpNoCase(jsData["command"].asString(), "restart os"))
    {
        Log::Get(Log::LOG_INFO) << " restart os" << std::endl;

        theResponse = Reboot(LINUX_REBOOT_CMD_RESTART);

    }
    else if(CmpNoCase(jsData["command"].asString(), "shutdown"))
    {
        Log::Get(Log::LOG_INFO) << " shutdown" << std::endl;

        theResponse = Reboot(LINUX_REBOOT_CMD_POWER_OFF);
    }
    else
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["result"] = false;
        theResponse.jsonData["reason"].append("Invalid command sent");
        Log::Get(Log::LOG_ERROR) << "'" << jsData["command"].asString() <<"' is not a valid command" << std::endl;
    }

    return theResponse;
}

response Core::PatchConfig(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PatchConfig" << std::endl;

    Json::Value jsData(ConvertToJson(theData.Get()));

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        if(jsData.isObject() == false)
        {
            theResponse.nHttpCode  = 400;
            theResponse.jsonData["result"] = false;
            theResponse.jsonData["reason"].append("Invalid JSON");
        }
        else
        {
            m_iniConfig.ReReadIniFile();    //sync with on disk version

            for(auto const& sSection : jsData.getMemberNames())
            {
                if(jsData[sSection].isObject())
                {
                    if(sSection == "server")
                    {
                        PatchServerConfig(jsData[sSection]);
                    }
                    else if(sSection != "restricted" && sSection != "remove")
                    {
                        iniSection* pSection = m_iniConfig.GetSection(sSection);
                        if(pSection)
                        {
                            for(auto const& sKey : jsData[sSection].getMemberNames())
                            {
                                if(pSection->FindData(sKey) != pSection->GetDataEnd() && jsData[sSection][sKey].isConvertibleTo(Json::stringValue))
                                {
                                    pSection->SetValue(sKey, jsData[sSection][sKey].asString());
                                }
                            }
                        }
                    }
                }
            }
            m_iniConfig.WriteIniFile();
            InitLogging();  //change logging if necessary
            m_manager.InitPaths();  //change paths if necessary
            theResponse = GetConfig(pConnection, theQuery, theData, theUrl);
            theResponse.jsonData["result"] = true;
        }
    }
    return theResponse;
}


void Core::PatchServerConfig(const Json::Value& jsData)
{
    for(auto const& sKey : jsData.getMemberNames())
    {
        if(sKey == "hostname" && jsData[sKey].isString())
        {
            sethostname(jsData[sKey].asString().c_str(), jsData[sKey].asString().length());
        }
    }
}

void Core::ResourceModified(enumType eType, const std::string& sUid, enumModification eModification)
{
    Json::Value jsValue;
    switch(eType)
    {
        case FILE:
            jsValue["type"] = "file";
            break;
        case PLAYLIST:
            jsValue["type"] = "playlist";
            break;
        case SCHEDULE:
            jsValue["type"] = "schedule";
            break;
        default:
            jsValue["type"] = "Unknown";
            break;
    }

    jsValue["uid"] = sUid;

    switch(eModification)
    {
        case MODIFIED:
            jsValue["modification"] = "modified";
            break;
        case ADDED:
            jsValue["modification"] = "added";
            break;
        case DELETED:
            jsValue["modification"] = "deleted";
            break;
    }
    m_jsResources.append(jsValue);
}

response Core::PatchFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PatchFile" << std::endl;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {

        std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

        theResponse = m_manager.ModifyFileMeta(vSplit.back(), ConvertToJson(theData.Get()));

        if(theResponse.nHttpCode == 200)
        {
            ResourceModified(FILE, vSplit.back(), MODIFIED);
        }
    }
    return theResponse;
}

response Core::PutFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PatchFile" << std::endl;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {

        std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

        theResponse = m_manager.ModifyFile(vSplit.back(), ConvertToJson(theData.Get()));

        if(theResponse.nHttpCode == 200)
        {
            ResourceModified(FILE, vSplit.back(), MODIFIED);
        }
    }
    return theResponse;
}

response Core::PutPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutPlaylist" << std::endl;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

        theResponse = m_manager.ModifyPlaylist(vSplit.back(), ConvertToJson(theData.Get()));
        if(theResponse.nHttpCode == 200)
        {
            ResourceModified(PLAYLIST, vSplit.back(), MODIFIED);
        }
    }
    return theResponse;
}

response Core::PutSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutSchedule" << std::endl;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

        theResponse = m_manager.ModifySchedule(vSplit.back(), ConvertToJson(theData.Get()));
        if(theResponse.nHttpCode == 200)
        {
            ResourceModified(SCHEDULE, vSplit.back(), MODIFIED);
        }
    }
    return theResponse;
}


response Core::DeleteFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "DeleteFile" << std::endl;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

        theResponse = m_manager.DeleteFile(vSplit.back());
        if(theResponse.nHttpCode == 200)
        {
            url aUrl(url(EP_FILES.Get()+"/"+vSplit.back()));

            m_server.DeleteEndpoint(endpoint(MongooseServer::GET, aUrl));
            m_server.DeleteEndpoint(endpoint(MongooseServer::PATCH, aUrl));
            m_server.DeleteEndpoint(endpoint(MongooseServer::DELETE, aUrl));

            ResourceModified(FILE, vSplit.back(), DELETED);
        }

    }
    return theResponse;
}

response Core::DeletePlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "DeletePlaylist" << std::endl;
    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

        theResponse = m_manager.DeletePlaylist(vSplit.back());

        if(theResponse.nHttpCode == 200)
        {
            url aUrl(url(EP_PLAYLISTS.Get()+"/"+vSplit.back()));

            m_server.DeleteEndpoint(endpoint(MongooseServer::GET, aUrl));
            m_server.DeleteEndpoint(endpoint(MongooseServer::PATCH, aUrl));
            m_server.DeleteEndpoint(endpoint(MongooseServer::DELETE, aUrl));

            ResourceModified(PLAYLIST, vSplit.back(), DELETED);
        }
    }
    return theResponse;
}

response Core::DeleteSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "DeleteSchedule" << std::endl;
    std::vector<std::string> vSplit(SplitString(theUrl.Get(), '/'));

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        theResponse = m_manager.DeleteSchedule(vSplit.back());

        if(theResponse.nHttpCode == 200)
        {
            url aUrl(url(EP_SCHEDULES.Get()+"/"+vSplit.back()));

            m_server.DeleteEndpoint(endpoint(MongooseServer::GET, aUrl));
            m_server.DeleteEndpoint(endpoint(MongooseServer::PATCH, aUrl));
            m_server.DeleteEndpoint(endpoint(MongooseServer::DELETE, aUrl));

            ResourceModified(SCHEDULE, vSplit.back(), DELETED);
        }
    }
    return theResponse;
}


response Core::PostFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PostFile" << std::endl;
    Json::Value jsonData(ConvertToJson(theData.Get()));

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        theResponse = m_manager.AddFiles(jsonData);
        Log::Get() << theResponse.jsonData << std::endl;
        Log::Get() << theResponse.nHttpCode << std::endl;

        if(theResponse.nHttpCode == 201)
        {
            url aUrl(url(EP_FILES.Get()+"/"+theResponse.jsonData["uid"].asString()));

            m_server.AddEndpoint(endpoint(MongooseServer::GET, aUrl), std::bind(&Core::GetFile, this, _1,_2,_3,_4));
            m_server.AddEndpoint(endpoint(MongooseServer::PATCH, aUrl), std::bind(&Core::PatchFile, this, _1,_2,_3,_4));
            m_server.AddEndpoint(endpoint(MongooseServer::PUT, aUrl), std::bind(&Core::PutFile, this, _1,_2,_3,_4));
            m_server.AddEndpoint(endpoint(MongooseServer::DELETE, aUrl), std::bind(&Core::DeleteFile, this, _1,_2,_3,_4));

            ResourceModified(FILE, theResponse.jsonData["uid"].asString(), ADDED);
        }
    }
    return theResponse;
}

response Core::PostPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PostPlaylist" << std::endl;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {

        theResponse = m_manager.AddPlaylist(ConvertToJson(theData.Get()));
        if(theResponse.nHttpCode == 201)
        {
            url aUrl(url(EP_PLAYLISTS.Get()+"/"+theResponse.jsonData["uid"].asString()));

            m_server.AddEndpoint(endpoint(MongooseServer::GET, aUrl), std::bind(&Core::GetPlaylist, this, _1,_2,_3,_4));
            m_server.AddEndpoint(endpoint(MongooseServer::PATCH, aUrl), std::bind(&Core::PutPlaylist, this, _1,_2,_3,_4));
            m_server.AddEndpoint(endpoint(MongooseServer::DELETE, aUrl), std::bind(&Core::DeletePlaylist, this, _1,_2,_3,_4));

            ResourceModified(PLAYLIST, theResponse.jsonData["uid"].asString(), ADDED);
        }
    }
    return theResponse;
}

response Core::PostSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PostSchedule" << std::endl;
    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        theResponse = m_manager.AddSchedule(ConvertToJson(theData.Get()));
        if(theResponse.nHttpCode == 201)
        {
            url aUrl(url(EP_SCHEDULES.Get()+"/"+theResponse.jsonData["uid"].asString()));
            m_server.AddEndpoint(endpoint(MongooseServer::GET, aUrl), std::bind(&Core::GetSchedule, this, _1,_2,_3,_4));
            m_server.AddEndpoint(endpoint(MongooseServer::PUT, aUrl), std::bind(&Core::PutSchedule, this, _1,_2,_3,_4));
            m_server.AddEndpoint(endpoint(MongooseServer::DELETE,aUrl), std::bind(&Core::DeleteSchedule, this, _1,_2,_3,_4));

            ResourceModified(SCHEDULE, theResponse.jsonData["uid"].asString(), ADDED);
        }
    }
    return theResponse;
}


void Core::StatusCallback(const std::string& sData)
{
    //lock as jsStatus can be called by pipe thread and server thread
    std::lock_guard<std::mutex> lg(m_mutex);

    m_jsStatus = Json::Value(Json::objectValue);    //reset
    m_jsStatus["locked"] = (m_iniConfig.GetIniInt("restricted", "locked", 0)==1);

    auto pResource = m_manager.GetPlayingResource();
    if(pResource)
    {
        m_jsStatus["resource"]["type"] = pResource->GetType();
        m_jsStatus["resource"]["uid"] = pResource->GetUid();
        m_jsStatus["resource"]["label"] = pResource->GetLabel();

    }

    m_jsStatus["player"] ="Running";
    m_jsStatus["status"] = ConvertToJson(sData);
    m_jsStatus["current_time"] = GetCurrentTimeAsIsoString();
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

    m_jsStatus["player"] = "Stopped";
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


void Core::LoopCallback(int nTook)
{
    m_nTimeSinceLastCall += nTook;

    if(m_jsResources.size() > 0)
    {
        m_server.SendWebsocketMessage(m_jsResources);
        m_jsResources.clear();
    }

    if(m_nTimeSinceLastCall > 2000)
    {
        m_server.SendWebsocketMessage(m_info.GetInfo());
        if(m_jsStatus["player"].asString() != "Playing")
        {
            m_jsStatus["current_time"] = GetCurrentTimeAsIsoString();
            m_server.SendWebsocketMessage(m_jsStatus);
        }
        m_nTimeSinceLastCall = 0;
    }

    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm local_time = *localtime(&now);
    if(local_time.tm_min == 0 && local_time.tm_sec == 0)
    {
        if(m_bLoggedThisHour == false)
        {
            m_bLoggedThisHour = true;
            pml::Log::Get() << GetCurrentTimeAsIsoString() << std::endl;
        }
    }
    else
    {
        m_bLoggedThisHour = false;
    }
}


response Core::PutUpdate(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl)
{
    Log::Get(Log::LOG_DEBUG) << "Endpoints\t" << "PutUpdate" << std::endl;
    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode == 423)
    {
        return theResponse;
    }


    Json::Value jsonData(ConvertToJson(theData.Get()));
    return m_manager.UpdateApplication(jsonData);

}



response Core::Reboot(int nCommand)
{
    response theResponse;
    sync(); //make sure filesystem is synced
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int nError = reboot(nCommand);
    if(nError == -1)
    {
        theResponse.nHttpCode = 500;
        theResponse.jsonData["result"] = false;
        theResponse.jsonData["reason"].append(strerror(errno));
    }
    else
    {
        theResponse.jsonData["result"] = true;
    }
    return theResponse;
}
