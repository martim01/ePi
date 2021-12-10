#include "core.h"
#include "resourcemanager.h"
#include "sysinfomanager.h"
#include <functional>
#include <vector>
#include "epiutils.h"
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
const std::string Core::WS          = "ws";        //GET PUT

const endpoint Core::EP_ROOT        = endpoint(ROOT);
const endpoint Core::EP_EPI         = endpoint(ROOT+EPI);
const endpoint Core::EP_STATUS      = endpoint(EP_EPI.Get()+"/"+STATUS);
const endpoint Core::EP_POWER       = endpoint(EP_EPI.Get()+"/"+POWER);
const endpoint Core::EP_CONFIG      = endpoint(EP_EPI.Get()+"/"+CONFIG);
const endpoint Core::EP_SCHEDULES   = endpoint(EP_EPI.Get()+"/"+SCHEDULES);
const endpoint Core::EP_PLAYLISTS   = endpoint(EP_EPI.Get()+"/"+PLAYLISTS);
const endpoint Core::EP_FILES       = endpoint(EP_EPI.Get()+"/"+FILES);
const endpoint Core::EP_INFO        = endpoint(EP_EPI.Get()+"/"+INFO);
const endpoint Core::EP_UPDATE      = endpoint(EP_EPI.Get()+"/"+UPDATE);
const endpoint Core::EP_OUTPUTS     = endpoint(EP_EPI.Get()+"/"+OUTPUTS);
const endpoint Core::EP_WS          = endpoint(ROOT+WS);

response ConvertPostDataToJson(const postData& vData)
{
    response resp(404, "No data sent or incorrect data sent");
    if(vData.size() == 1)
    {
        resp.nHttpCode = 200;
        resp.jsonData = ConvertToJson(vData[0].sData);
    }
    else if(vData.size() > 1)
    {
        resp.nHttpCode = 200;
        for(size_t i = 0; i < vData.size(); i++)
        {
            if(vData[i].sName.empty() == false)
            {
                resp.jsonData[vData[i].sName] = vData[i].sData;
            }
        }
    }
    return resp;
}


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
        m_nLogToConsole = pmlLog().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));
    }
    else if(m_nLogToConsole != 0)
    {
        pmlLog().RemoveOutput(m_nLogToConsole);
        m_nLogToConsole = 0;
    }

    if(m_iniConfig.GetIniInt("logging", "file",0) == 1 &&  m_nLogToFile == 0)
    {
        m_nLogToFile = pmlLog().AddOutput(std::unique_ptr<pml::LogOutput>(new LogToFile(CreatePath(m_iniConfig.GetIniString("paths","logs","."))+"episerver")));
    }
    else if(m_nLogToFile != 0)
    {
        pmlLog().RemoveOutput(m_nLogToFile);
        m_nLogToFile = 0;
    }

}

void Core::Run(const std::string& sConfigFile)
{

    if(m_iniConfig.ReadIniFile(sConfigFile) == false)
    {
        pmlLog().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));
        pmlLog(pml::LOG_CRITICAL) << "Could not open '" << sConfigFile << "' exiting.";
        return;
    }

    InitLogging();

    pmlLog() << "Core\tStart" ;

    m_manager.Init();
    m_info.SetDiskPath(m_manager.GetAudioPath());

    if(m_server.Init(m_iniConfig.GetIniString("api", "sslCert", ""), m_iniConfig.GetIniString("api", "ssKey", ""), m_iniConfig.GetIniInt("api", "port", 8080), "", true))
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

    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "CreateEndpoints" ;

    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_ROOT), std::bind(&Core::GetRoot, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_EPI), std::bind(&Core::GetEpi, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_STATUS), std::bind(&Core::GetStatus, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_POWER), std::bind(&Core::GetPower, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_CONFIG), std::bind(&Core::GetConfig, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_SCHEDULES), std::bind(&Core::GetSchedules, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_PLAYLISTS), std::bind(&Core::GetPlaylists, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_FILES), std::bind(&Core::GetFiles, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_INFO), std::bind(&Core::GetInfo, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_UPDATE), std::bind(&Core::GetUpdate, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::GET, EP_OUTPUTS), std::bind(&Core::GetOutputs, this, _1,_2,_3,_4));

    m_server.AddEndpoint(methodpoint(RestGoose::PATCH, EP_STATUS), std::bind(&Core::PatchStatus, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::PATCH, EP_CONFIG), std::bind(&Core::PatchConfig, this, _1,_2,_3,_4));

    m_server.AddEndpoint(methodpoint(RestGoose::PUT, EP_POWER), std::bind(&Core::PutPower, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::PUT, EP_UPDATE), std::bind(&Core::PutUpdate, this, _1,_2,_3,_4));

    m_server.AddEndpoint(methodpoint(RestGoose::POST, EP_SCHEDULES), std::bind(&Core::PostSchedule, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::POST, EP_PLAYLISTS), std::bind(&Core::PostPlaylist, this, _1,_2,_3,_4));
    m_server.AddEndpoint(methodpoint(RestGoose::POST, EP_FILES), std::bind(&Core::PostFile, this, _1,_2,_3,_4));


    //now add all the dynamic methodpoints
    for(auto itFile = m_manager.GetFilesBegin(); itFile != m_manager.GetFilesEnd(); ++itFile)
    {
        endpoint theEndpoint(endpoint(EP_FILES.Get()+"/"+itFile->first));

        m_server.AddEndpoint(methodpoint(RestGoose::GET, theEndpoint), std::bind(&Core::GetFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(methodpoint(RestGoose::PATCH, theEndpoint), std::bind(&Core::PatchFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(methodpoint(RestGoose::PUT, theEndpoint), std::bind(&Core::PutFile, this, _1,_2,_3,_4));
        m_server.AddEndpoint(methodpoint(RestGoose::HTTP_DELETE, theEndpoint), std::bind(&Core::DeleteFile, this, _1,_2,_3,_4));
    }

    for(auto itPlaylist = m_manager.GetPlaylistsBegin(); itPlaylist != m_manager.GetPlaylistsEnd(); ++itPlaylist)
    {
        endpoint theEndpoint(endpoint(EP_PLAYLISTS.Get()+"/"+itPlaylist->first));

        m_server.AddEndpoint(methodpoint(RestGoose::GET, theEndpoint), std::bind(&Core::GetPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(methodpoint(RestGoose::PUT, theEndpoint), std::bind(&Core::PutPlaylist, this, _1,_2,_3,_4));
        m_server.AddEndpoint(methodpoint(RestGoose::HTTP_DELETE, theEndpoint), std::bind(&Core::DeletePlaylist, this, _1,_2,_3,_4));
    }


    for(auto itSchedule = m_manager.GetSchedulesBegin(); itSchedule != m_manager.GetSchedulesEnd(); ++itSchedule)
    {
        endpoint theEndpoint(endpoint(EP_SCHEDULES.Get()+"/"+itSchedule->first));

        m_server.AddEndpoint(methodpoint(RestGoose::GET, theEndpoint), std::bind(&Core::GetSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(methodpoint(RestGoose::PUT, theEndpoint), std::bind(&Core::PutSchedule, this, _1,_2,_3,_4));
        m_server.AddEndpoint(methodpoint(RestGoose::HTTP_DELETE, theEndpoint), std::bind(&Core::DeleteSchedule, this, _1,_2,_3,_4));
    }

    if(m_server.AddWebsocketEndpoint(EP_WS, std::bind(&Core::WebsocketAuthenticate, this, _1,_2,_3), std::bind(&Core::WebsocketMessage, this, _1,_2),
                                  std::bind(&Core::WebsocketClosed, this, _1,_2)) == false)
    {
        pmlLog(pml::LOG_ERROR) << "Failed to add websocket endpoints";
    }

    //Add the loop callback function
    m_server.SetLoopCallback(std::bind(&Core::LoopCallback, this, _1));

    return true;
}

response Core::GetRoot(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetRoot" ;
    response theResponse;
    theResponse.jsonData.append(EPI);
    return theResponse;
}

response Core::GetEpi(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetEpi" ;
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

response Core::GetFiles(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetFiles" ;
    return m_manager.GetFiles();
}

response Core::GetPlaylists(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetPlaylists" ;
    return m_manager.GetPlaylists();
}

response Core::GetSchedules(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetSchedules" ;
    return m_manager.GetSchedules();
}

response Core::GetConfig(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetConfig" ;
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

response Core::GetUpdate(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    //get all the version numbers...
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetUpdate" ;
    response theResponse;
    std::stringstream ssVersion;
    ssVersion << version::MAJOR << "." << version::MINOR << "." << version::PATCH << "." << version::BUILD;
    theResponse.jsonData["server"]["version"] = ssVersion.str();
    theResponse.jsonData["server"]["date"] = ConvertTimeToIsoString(std::time_t(version::DATE));


    theResponse.jsonData["player3"] = ConvertToJson(Exec(CreatePath(m_iniConfig.GetIniString("paths", "player","/home/pi/ePi/bin"))+"player3 -v"));


    //get versions of other applications...
    return theResponse;
}

response Core::GetOutputs(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    //get all the version numbers...
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetOutputs" ;
    response theResponse;

    //pmlLog(pml::LOG_DEBUG) << "
    theResponse.jsonData = ConvertToJson(Exec(CreatePath(m_iniConfig.GetIniString("paths", "player","."))+"player3 -d"));


    //get versions of other applications...
    return theResponse;
}

response Core::GetStatus(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    //lock as jsStatus can be called by pipe thread and server thread
    std::lock_guard<std::mutex> lg(m_mutex);

    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetStatus" ;
    m_jsStatus["locked"] = (m_iniConfig.GetIniInt("restricted", "locked", 0)==1);

    response theResponse;
    theResponse.jsonData = m_jsStatus;
    return theResponse;
}


response Core::GetInfo(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetInfo" ;

    response theResponse;
    theResponse.jsonData = m_info.GetInfo();


    return theResponse;
}

response Core::GetPower(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetPower" ;
    response theResponse;
    theResponse.jsonData["status"] = "On";

    return theResponse;
}

response Core::GetFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetFile" ;
    std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));
    return m_manager.GetFile(vSplit.back());
}

response Core::GetPlaylist(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetPlaylist" ;
    std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));

    return m_manager.GetPlaylist(vSplit.back());
}

response Core::GetSchedule(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "GetSchedule" ;
    std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));

    return m_manager.GetSchedule(vSplit.back());
}


response Core::PatchStatus(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PatchStatus" ;

    auto theResponse = ConvertPostDataToJson(vData);
    if(theResponse.nHttpCode == 200)
    {
        theResponse = m_manager.ModifyStatus(theResponse.jsonData);

        if(theResponse.jsonData.isMember("commad") && CmpNoCase(theResponse.jsonData["command"].asString(), "kill"))
        {
            GetInitialPlayerStatus();   //see if the players have been killed
        }
    }
    return theResponse;
}

response Core::PutPower(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PutPower: ";

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode == 423)
    {
        return theResponse;
    }

    theResponse = ConvertPostDataToJson(vData);
    if(theResponse.nHttpCode != 200)
    {
        return theResponse;
    }

    if(theResponse.jsonData.isMember("command") == false || theResponse.jsonData["command"].isString() == false)
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["result"] = "No command sent";
        pmlLog(pml::LOG_ERROR) << " no command sent" ;
    }
    else if(CmpNoCase(theResponse.jsonData["command"].asString(), "restart server"))
    {
        // send a message to "main" to exit the loop
        pmlLog(pml::LOG_INFO) << " restart server" ;
        m_server.Stop();
    }
    else if(CmpNoCase(theResponse.jsonData["command"].asString(), "restart os"))
    {
        pmlLog(pml::LOG_INFO) << " restart os" ;

        theResponse = Reboot(LINUX_REBOOT_CMD_RESTART);

    }
    else if(CmpNoCase(theResponse.jsonData["command"].asString(), "shutdown"))
    {
        pmlLog(pml::LOG_INFO) << " shutdown" ;

        theResponse = Reboot(LINUX_REBOOT_CMD_POWER_OFF);
    }
    else
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["result"] = false;
        theResponse.jsonData["reason"].append("Invalid command sent");
        pmlLog(pml::LOG_ERROR) << "'" << theResponse.jsonData["command"].asString() <<"' is not a valid command" ;
    }

    return theResponse;
}

response Core::PatchConfig(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PatchConfig" ;

    auto theResponse = ConvertPostDataToJson(vData);
    if(theResponse.nHttpCode != 200)
    {
        return theResponse;
    }

    theResponse = m_manager.IsLocked();
    if(theResponse.nHttpCode != 423)
    {
        if(theResponse.jsonData.isObject() == false)
        {
            theResponse.nHttpCode  = 400;
            theResponse.jsonData["result"] = false;
            theResponse.jsonData["reason"].append("Invalid JSON");
        }
        else
        {
            m_iniConfig.ReReadIniFile();    //sync with on disk version

            for(auto const& sSection : theResponse.jsonData.getMemberNames())
            {
                if(theResponse.jsonData[sSection].isObject())
                {
                    if(sSection == "server")
                    {
                        PatchServerConfig(theResponse.jsonData[sSection]);
                    }
                    else if(sSection != "restricted" && sSection != "remove")
                    {
                        iniSection* pSection = m_iniConfig.GetSection(sSection);
                        if(pSection)
                        {
                            for(auto const& sKey : theResponse.jsonData[sSection].getMemberNames())
                            {
                                if(pSection->FindData(sKey) != pSection->GetDataEnd() && theResponse.jsonData[sSection][sKey].isConvertibleTo(Json::stringValue))
                                {
                                    pSection->SetValue(sKey, theResponse.jsonData[sSection][sKey].asString());
                                }
                            }
                        }
                    }
                }
            }
            m_iniConfig.WriteIniFile();
            InitLogging();  //change logging if necessary
            m_manager.InitPaths();  //change paths if necessary
            theResponse = GetConfig(theQuery, vData, theEndpoint, theUser);
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

response Core::PatchFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PatchFile" ;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));

        theResponse = ConvertPostDataToJson(vData);
        if(theResponse.nHttpCode == 200)
        {
            theResponse = m_manager.ModifyFileMeta(vSplit.back(), theResponse.jsonData);

            if(theResponse.nHttpCode == 200)
            {
                ResourceModified(FILE, vSplit.back(), MODIFIED);
            }
        }
    }
    return theResponse;
}

response Core::PutFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PutFile" ;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));

        theResponse = ConvertPostDataToJson(vData);
        if(theResponse.nHttpCode == 200)
        {
            theResponse = m_manager.ModifyFile(vSplit.back(), theResponse.jsonData);

            if(theResponse.nHttpCode == 200)
            {
                ResourceModified(FILE, vSplit.back(), MODIFIED);
            }
        }
    }
    return theResponse;
}

response Core::PutPlaylist(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PutPlaylist" ;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));
        theResponse = ConvertPostDataToJson(vData);
        if(theResponse.nHttpCode == 200)
        {
            theResponse = m_manager.ModifyPlaylist(vSplit.back(), theResponse.jsonData);
            if(theResponse.nHttpCode == 200)
            {
                ResourceModified(PLAYLIST, vSplit.back(), MODIFIED);
            }
        }
    }
    return theResponse;
}

response Core::PutSchedule(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PutSchedule" ;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));
        theResponse = ConvertPostDataToJson(vData);
        if(theResponse.nHttpCode == 200)
        {
            theResponse = m_manager.ModifySchedule(vSplit.back(), theResponse.jsonData);
            if(theResponse.nHttpCode == 200)
            {
                ResourceModified(SCHEDULE, vSplit.back(), MODIFIED);
            }
        }
    }
    return theResponse;
}


response Core::DeleteFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "DeleteFile" ;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));

        theResponse = m_manager.DeleteFile(vSplit.back());
        if(theResponse.nHttpCode == 200)
        {
            endpoint aUrl(endpoint(EP_FILES.Get()+"/"+vSplit.back()));

            m_server.DeleteEndpoint(methodpoint(RestGoose::GET, aUrl));
            m_server.DeleteEndpoint(methodpoint(RestGoose::PATCH, aUrl));
            m_server.DeleteEndpoint(methodpoint(RestGoose::HTTP_DELETE, aUrl));

            ResourceModified(FILE, vSplit.back(), DELETED);
        }

    }
    return theResponse;
}

response Core::DeletePlaylist(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "DeletePlaylist" ;
    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));

        theResponse = m_manager.DeletePlaylist(vSplit.back());

        if(theResponse.nHttpCode == 200)
        {
            endpoint aUrl(endpoint(EP_PLAYLISTS.Get()+"/"+vSplit.back()));

            m_server.DeleteEndpoint(methodpoint(RestGoose::GET, aUrl));
            m_server.DeleteEndpoint(methodpoint(RestGoose::PATCH, aUrl));
            m_server.DeleteEndpoint(methodpoint(RestGoose::HTTP_DELETE, aUrl));

            ResourceModified(PLAYLIST, vSplit.back(), DELETED);
        }
    }
    return theResponse;
}

response Core::DeleteSchedule(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "DeleteSchedule" ;
    std::vector<std::string> vSplit(SplitString(theEndpoint.Get(), '/'));

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        theResponse = m_manager.DeleteSchedule(vSplit.back());

        if(theResponse.nHttpCode == 200)
        {
            endpoint aUrl(endpoint(EP_SCHEDULES.Get()+"/"+vSplit.back()));

            m_server.DeleteEndpoint(methodpoint(RestGoose::GET, aUrl));
            m_server.DeleteEndpoint(methodpoint(RestGoose::PATCH, aUrl));
            m_server.DeleteEndpoint(methodpoint(RestGoose::HTTP_DELETE, aUrl));

            ResourceModified(SCHEDULE, vSplit.back(), DELETED);
        }
    }
    return theResponse;
}


response Core::PostFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PostFile ";

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        theResponse = m_manager.AddFiles(ConvertPostDataToJson(vData).jsonData);
        pmlLog() << theResponse.jsonData ;
        pmlLog() << theResponse.nHttpCode ;

        if(theResponse.nHttpCode == 201)
        {
            endpoint aUrl(endpoint(EP_FILES.Get()+"/"+theResponse.jsonData["uid"].asString()));

            m_server.AddEndpoint(methodpoint(RestGoose::GET, aUrl), std::bind(&Core::GetFile, this, _1,_2,_3,_4));
            m_server.AddEndpoint(methodpoint(RestGoose::PATCH, aUrl), std::bind(&Core::PatchFile, this, _1,_2,_3,_4));
            m_server.AddEndpoint(methodpoint(RestGoose::PUT, aUrl), std::bind(&Core::PutFile, this, _1,_2,_3,_4));
            m_server.AddEndpoint(methodpoint(RestGoose::HTTP_DELETE, aUrl), std::bind(&Core::DeleteFile, this, _1,_2,_3,_4));

            ResourceModified(FILE, theResponse.jsonData["uid"].asString(), ADDED);
        }
    }
    return theResponse;
}

response Core::PostPlaylist(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PostPlaylist" ;

    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        theResponse = ConvertPostDataToJson(vData);
        if(theResponse.nHttpCode == 200)
        {
            theResponse = m_manager.AddPlaylist(theResponse.jsonData);
            if(theResponse.nHttpCode == 201)
            {
                endpoint aUrl(endpoint(EP_PLAYLISTS.Get()+"/"+theResponse.jsonData["uid"].asString()));

                m_server.AddEndpoint(methodpoint(RestGoose::GET, aUrl), std::bind(&Core::GetPlaylist, this, _1,_2,_3,_4));
                m_server.AddEndpoint(methodpoint(RestGoose::PATCH, aUrl), std::bind(&Core::PutPlaylist, this, _1,_2,_3,_4));
                m_server.AddEndpoint(methodpoint(RestGoose::HTTP_DELETE, aUrl), std::bind(&Core::DeletePlaylist, this, _1,_2,_3,_4));

                ResourceModified(PLAYLIST, theResponse.jsonData["uid"].asString(), ADDED);
            }
        }
    }
    return theResponse;
}

response Core::PostSchedule(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PostSchedule" ;
    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode != 423)
    {
        theResponse = ConvertPostDataToJson(vData);
        if(theResponse.nHttpCode == 200)
        {
            theResponse = m_manager.AddSchedule(theResponse.jsonData);
            if(theResponse.nHttpCode == 201)
            {
                endpoint aUrl(endpoint(EP_SCHEDULES.Get()+"/"+theResponse.jsonData["uid"].asString()));
                m_server.AddEndpoint(methodpoint(RestGoose::GET, aUrl), std::bind(&Core::GetSchedule, this, _1,_2,_3,_4));
                m_server.AddEndpoint(methodpoint(RestGoose::PUT, aUrl), std::bind(&Core::PutSchedule, this, _1,_2,_3,_4));
                m_server.AddEndpoint(methodpoint(RestGoose::HTTP_DELETE,aUrl), std::bind(&Core::DeleteSchedule, this, _1,_2,_3,_4));

                ResourceModified(SCHEDULE, theResponse.jsonData["uid"].asString(), ADDED);
            }
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
    m_server.SendWebsocketMessage({EP_WS}, m_jsStatus);
}

void Core::ExitCallback(int nExit)
{
    pmlLog() << "Player exited" ;
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

    m_server.SendWebsocketMessage({EP_WS}, m_jsStatus);
}


void Core::LoopCallback(int nTook)
{

    m_nTimeSinceLastCall += nTook;

    if(m_jsResources.size() > 0)
    {
        m_server.SendWebsocketMessage({EP_WS}, m_jsResources);
        m_jsResources.clear();
    }

    if(m_nTimeSinceLastCall > 2000)
    {
        m_server.SendWebsocketMessage({EP_WS}, m_info.GetInfo());

        //lock as jsStatus can be called by pipe thread and server thread
        std::lock_guard<std::mutex> lg(m_mutex);

        if(m_jsStatus["player"].asString() != "Playing")
        {
            m_jsStatus["current_time"] = GetCurrentTimeAsIsoString();
            m_server.SendWebsocketMessage({EP_WS}, m_jsStatus);
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
            pmlLog() << GetCurrentTimeAsIsoString() ;
        }
    }
    else
    {
        m_bLoggedThisHour = false;
    }
}


response Core::PutUpdate(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser)
{
    pmlLog(pml::LOG_DEBUG) << "Endpoints\t" << "PutUpdate" ;
    response theResponse(m_manager.IsLocked());
    if(theResponse.nHttpCode == 423)
    {
        return theResponse;
    }

    theResponse = ConvertPostDataToJson(vData);
    if(theResponse.nHttpCode == 200)
    {
        return m_manager.UpdateApplication(theResponse.jsonData);
    }
    else
    {
        return theResponse;
    }
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

bool Core::WebsocketAuthenticate(const endpoint& theEndpoint, const userName& theUser, const ipAddress& peer)
{
    pmlLog() << "Websocket connection request from " << peer;
    return true;
}

bool Core::WebsocketMessage(const endpoint& theEndpoint, const Json::Value& jsData)
{
    pmlLog() << "Websocket message '" << jsData << "'";
    return true;
}

void Core::WebsocketClosed(const endpoint& theEndpoint, const ipAddress& peer)
{
    pmlLog() << "Websocket closed from " << peer;
}

