#include "resourcemanager.h"
#include <iostream>
#include <fstream>
#include "inimanager.h"
#include "inisection.h"
#include "epiutils.h"
#include "uidutils.h"
#include "audiofile.h"
#include "wavfile.h"
#include "mp3file.h"
#include "sndfile.hh"
#include "log.h"
#include "epicron.h"
#include <functional>
#include "proccheck.h"


using namespace std::placeholders;

ResourceManager::ResourceManager(Launcher& launcher, iniManager& iniConfig) :
   m_launcher(launcher),
   m_iniConfig(iniConfig),
   m_pPlayingResource(nullptr)
{
}

void ResourceManager::Init()
{
    InitPaths();


    //remove any old application files
    iniSection* pSection = m_iniConfig.GetSection("remove");
    if(pSection)
    {
        for(auto itData = pSection->GetDataBegin(); itData != pSection->GetDataEnd(); ++itData)
        {
            remove(itData->second.c_str());
        }
        m_iniConfig.DeleteSection("remove");

        m_iniConfig.WriteIniFile();
    }
}

void ResourceManager::InitPaths()
{
    m_sAudioFilePath = CreatePath(m_iniConfig.GetIniString("paths", "audio", "/var/ePi/audio"));
    m_sResourcePath = CreatePath(m_iniConfig.GetIniString("paths", "resources", "/var/ePi/resources"));

    mkpath(m_sAudioFilePath, 0755);
    mkpath(m_sResourcePath, 0755);

    LoadResources();
}

ResourceManager::~ResourceManager()
{
    SaveResources();
}

pml::restgoose::response ResourceManager::AddFiles(const Json::Value& jsData)
{
    pmlLog() << "ResourceManager\tAddFile: " << jsData;

    pml::restgoose::response theResponse(ParseFileData(jsData));
    if(theResponse.nHttpCode ==400)
    {
        pmlLog(pml::LOG_ERROR) << "failed - incorrect multipart data";
    }
    else
    {
        theResponse.nHttpCode = 201;
        //check we have metadata for each of the uploaded files..
        theResponse = AddFile(jsData["files"]["location"].asString(), jsData["label"].asString(), jsData["description"].asString());

    }
    return theResponse;
}

pml::restgoose::response ResourceManager::AddFile( const std::string& sTmpPathName, const std::string& sLabel, const std::string& sDescription)
{

    pml::restgoose::response theResponse(ParseFile(sTmpPathName, sLabel, sDescription));

    if(theResponse.nHttpCode ==400)
    {
        pmlLog(pml::LOG_ERROR) << "failed - incorrect JSON" << std::endl;
        remove(sTmpPathName.c_str());
    }
    else if(m_iniConfig.GetIniInt("maximum", "files", -1) > 0 &&  m_iniConfig.GetIniInt("maximum", "files", -1) == m_mFiles.size())
    {
        theResponse.nHttpCode = 409;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Maximum number of files already uploaded.");
        pmlLog(pml::LOG_ERROR) << "failed - maximum file count" << std::endl;
        remove(sTmpPathName.c_str());
    }
    else if(FileExists(sLabel))
    {
        theResponse.nHttpCode = 409;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("File already exists with the given label.");
        pmlLog(pml::LOG_ERROR) << "failed - label already in use" << std::endl;
        remove(sTmpPathName.c_str());
    }
    else
    {
        std::string sUid(CreateGuid());
        std::string sNewName((m_sAudioFilePath+sUid));
        int nResult = rename(sTmpPathName.c_str(), sNewName.c_str());
        if(nResult != 0)
        {
            theResponse.nHttpCode = 500;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Could not copy file from upload directory");
            theResponse.jsonData["reason"].append(sTmpPathName);
            theResponse.jsonData["reason"].append(sNewName);
            theResponse.jsonData["reason"].append(strerror(errno));
            pmlLog(pml::LOG_ERROR) << "could not copy file from upload directory" << std::endl;
            //remove(sTmpPathName.c_str());
        }
        else
        {
            std::shared_ptr<AudioFile> pFile;

            if(theResponse.nHttpCode == WAV)
            {
                pFile = std::dynamic_pointer_cast<AudioFile>(std::make_shared<WavFile>(m_sAudioFilePath, sUid, sLabel, sDescription));
            }            else
            {
                // @todo(martim01) mp3file
            }

            if(m_mFiles.insert(std::make_pair(sUid, pFile)).second)
            {
                SaveResources();
                pFile->InitJson();

                theResponse.nHttpCode = 201;
                theResponse.jsonData["success"] = true;
                theResponse.jsonData["uid"] = sUid;
                theResponse.jsonData["details"] = pFile->GetJson();
            }
            else
            {
                pmlLog(pml::LOG_ERROR) << "failed - could not create uid" << std::endl;
                theResponse.nHttpCode = 409;
                theResponse.jsonData["success"] = false;
                theResponse.jsonData["reason"].append("Unable to create unique id for item");
                remove(sNewName.c_str());
            }
            pmlLog() << "success";
        }
    }
    return theResponse;

}

pml::restgoose::response ResourceManager::AddSchedule(const Json::Value& jsData)
{
    pmlLog() << "ResourceManager\tAddSchedule: ";

    pml::restgoose::response theResponse(ParseSchedule(jsData));
    if(theResponse.nHttpCode == 200)
    {
        if(ResourceExists(jsData["label"].asString(), m_mSchedules))
        {
            theResponse.nHttpCode = 409;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Schedule already exists with the given label.");
            pmlLog(pml::LOG_ERROR) << "failed - label already in use" ;
        }
        else if(m_iniConfig.GetIniInt("maximum", "schedules", -1) > 0 &&  m_iniConfig.GetIniInt("maximum", "schedules", -1) == m_mFiles.size())
        {
            theResponse.nHttpCode = 409;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Maximum number of schedules already uploaded.");
            pmlLog(pml::LOG_ERROR) << "failed - maximum schedule count" ;
        }
        else
        {
            //all valid so create the new schedule item
            std::string sUid(CreateGuid());
            if(m_mSchedules.insert(std::make_pair(sUid, std::make_shared<Resource>("Schedule", sUid, jsData))).second)
            {
                SaveResources();
                theResponse.nHttpCode = 201;
                theResponse.jsonData["success"] = true;
                theResponse.jsonData["uid"] = sUid;
                pmlLog() << "success" ;
            }
            else
            {
                theResponse.nHttpCode = 409;
                theResponse.jsonData["success"] = false;
                theResponse.jsonData["reason"].append("Unable to create unique id for item");
                pmlLog(pml::LOG_ERROR) << "failed - unable to create uid" ;
            }
        }
    }
    else
    {
        pmlLog(pml::LOG_ERROR) << "failed - incorrect JSON" ;
    }
    return theResponse;
}


pml::restgoose::response ResourceManager::AddPlaylist(const Json::Value& jsData)
{
    pml::LogStream lg;
    lg << "ResourceManager\tAddPlaylist: ";

    pml::restgoose::response theResponse(ParsePlaylist(jsData));
    if(theResponse.nHttpCode == 200)
    {
        if(ResourceExists(jsData["label"].asString(), m_mPlaylists))
        {
            theResponse.nHttpCode = 409;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Playlist already exists with the given label.");
            lg.SetLevel(pml::LOG_ERROR) << "failed - label already in use";
        }
        else if(m_iniConfig.GetIniInt("maximum", "playlists", -1) > 0 &&  m_iniConfig.GetIniInt("maximum", "playlists", -1) == m_mFiles.size())
        {
            theResponse.nHttpCode = 409;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Maximum number of playlists already uploaded.");
            lg.SetLevel(pml::LOG_ERROR) << "failed - maximum playlist count";
        }
        else
        {
            //all valid so create the new Playlist item
            std::string sUid(CreateGuid());
            if(m_mPlaylists.insert(std::make_pair(sUid, std::make_shared<Resource>("Playlist", sUid, jsData))).second)
            {
                SaveResources();
                theResponse.nHttpCode = 201;
                theResponse.jsonData["success"] = true;
                theResponse.jsonData["uid"] = sUid;
                lg << "success";
            }
            else
            {
                theResponse.nHttpCode = 409;
                theResponse.jsonData["success"] = false;
                theResponse.jsonData["reason"].append("Unable to create unique id for item");

                lg.SetLevel(pml::LOG_ERROR) << "failed - unable to create uid";
            }
        }
    }
    else
    {
        lg.SetLevel(pml::LOG_ERROR) << "failed - incorrect JSON";
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::ModifyFile(const std::string& sUid, const Json::Value& jsData)
{
    pml::LogStream lg;
    lg << "ResourceManager\tModifyFile: " << jsData;
    pml::restgoose::response theResponse;
    auto itFile = m_mFiles.find(sUid);
    if(itFile == m_mFiles.end())
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("File with uid '"+sUid+"' not found.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - file '" << sUid << "'not found";
        return theResponse;
    }
    if(itFile->second->IsLocked())
    {
        theResponse.nHttpCode = 423;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("File with uid '"+sUid+"' is locked.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - file '" << sUid << "'is locked";
        return theResponse;
    }

    if(jsData.isMember("files") == false)
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("No file uploaded");
        return theResponse;
    }

    std::string sNewName(m_sAudioFilePath+sUid);
    std::string sTemp(jsData["files"]["location"].asString());
    int nResult = rename(sTemp.c_str(), sNewName.c_str());
    if(nResult != 0)
    {
        theResponse.nHttpCode = 500;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Could not copy file from upload directory");
        theResponse.jsonData["reason"].append(sTemp);
        theResponse.jsonData["reason"].append(strerror(errno));
        lg.SetLevel(pml::LOG_ERROR) << "could not copy file from upload directory";
        remove(sTemp.c_str());
        return theResponse;
    }

    itFile->second->FileModified();

    Json::Value jsPatch;
    if(jsData.isMember("label") == true && FileExists(jsData["label"].asString(), sUid) == false)
    {
        jsPatch["label"] = jsData["label"].asString();
    }
    if(jsData.isMember("description") == true)
    {
        jsPatch["description"] = jsData["description"].asString();
    }
    itFile->second->UpdateJson(jsPatch);

    theResponse.nHttpCode = 200;
    theResponse.jsonData = itFile->second->GetJson();
    theResponse.jsonData["success"] = true;

    SaveResources();
    return theResponse;
}

pml::restgoose::response ResourceManager::ModifyFileMeta(const std::string& sUid, const Json::Value& jsData)
{
    pml::LogStream lg;
    lg << "ResourceManager\tModifyFileMeta: ";

    pml::restgoose::response theResponse;
    auto itFile = m_mFiles.find(sUid);
    if(itFile == m_mFiles.end())
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("File with uid '"+sUid+"' not found.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - file '" << sUid << "'not found";
    }
    else if(itFile->second->IsLocked())
    {
        theResponse.nHttpCode = 423;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("File with uid '"+sUid+"' is locked.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - file '" << sUid << "'is locked";
    }
    else
    {

        theResponse = ParseResource(jsData);

        if(theResponse.nHttpCode == 200)
        {
            if(FileExists(jsData["label"].asString(), sUid))
            {
                theResponse.nHttpCode = 409;
                theResponse.jsonData["success"] = false;
                theResponse.jsonData["reason"].append("File already exists with the given label.");
                lg.SetLevel(pml::LOG_ERROR) << "failed - label already in use";
            }
            else
            {
                itFile->second->UpdateJson(jsData);
                itFile->second->InitJson();
                theResponse.jsonData = itFile->second->GetJson();
                theResponse.jsonData["success"] = true;

                lg << "success";
                SaveResources();
            }
        }
        else
        {
            lg.SetLevel(pml::LOG_ERROR) << "failed - incorrect JSON";
        }
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::ModifySchedule(const std::string& sUid, const Json::Value& jsData)
{
    pml::LogStream lg;
    lg << "ResourceManager\tModifySchedule: ";

    pml::restgoose::response theResponse;
    auto itSchedule = m_mSchedules.find(sUid);
    if(itSchedule == m_mSchedules.end())
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Schedule with uid '"+sUid+"' not found.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - schedule '" << sUid << "'not found";
    }
    else if(itSchedule->second->IsLocked())
    {
        theResponse.nHttpCode = 423;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Schedule with uid '"+sUid+"' is locked.");
    }
    else
    {
        theResponse = ParseSchedule(jsData);
        if(theResponse.nHttpCode == 200)
        {
            if(ResourceExists(jsData["label"].asString(), m_mSchedules, sUid))
            {
                theResponse.nHttpCode = 409;
                theResponse.jsonData["success"] = false;
                theResponse.jsonData["reason"].append("Schedule already exists with the given label.");
                lg.SetLevel(pml::LOG_ERROR) << "failed - label already in use";
            }
            else
            {
                itSchedule->second->UpdateJson(jsData);
                theResponse.jsonData= itSchedule->second->GetJson();
                theResponse.jsonData["success"] = true;

                lg << "success";
                SaveResources();
            }
        }
        else
        {
            lg.SetLevel(pml::LOG_ERROR) << "failed - incorrect JSON";
        }
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::ModifyPlaylist(const std::string& sUid, const Json::Value& jsData)
{
    pml::LogStream lg;
    lg << "ResourceManager\tModifyPlaylist: ";

    pml::restgoose::response theResponse;
    auto itPlaylist = m_mPlaylists.find(sUid);
    if(itPlaylist == m_mPlaylists.end())
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Playlist with uid '"+sUid+"' not found.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - playlist '" << sUid << "' not found";
    }
    else if(itPlaylist->second->IsLocked())
    {
        theResponse.nHttpCode = 423;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Playlist with uid '"+sUid+"' is locked.");
    }
    else
    {
        theResponse = ParsePlaylist(jsData);
        if(theResponse.nHttpCode == 200)
        {
            if(ResourceExists(jsData["label"].asString(), m_mPlaylists, sUid))
            {
                theResponse.nHttpCode = 409;
                theResponse.jsonData["success"] = false;
                theResponse.jsonData["reason"].append("Playlist already exists with the given label.");
                lg.SetLevel(pml::LOG_ERROR) << "failed - label already in use";
            }
            else
            {
                itPlaylist->second->UpdateJson(jsData);
                theResponse.jsonData = itPlaylist->second->GetJson();
                theResponse.jsonData["success"] = true;

                lg << "success";
                SaveResources();
            }
        }
        else
        {
            lg.SetLevel(pml::LOG_ERROR) << "failed - incorrect JSON";
        }
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::DeleteFile(const std::string& sUid)
{
    pml::LogStream lg;
    lg << "ResourceManager\tDeleteFile: ";

    pml::restgoose::response theResponse;
    auto itFile  = m_mFiles.find(sUid);
    if(itFile == m_mFiles.end())
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("File with uid '"+sUid+"' not found.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - file '" << sUid << "' not found";
    }
    else if(itFile->second->IsLocked())
    {
        theResponse.nHttpCode = 423;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("File with uid '"+sUid+"' is locked.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - file '" << sUid << "' is locked";

    }
    else
    {
        pml::restgoose::response contains(GetSchedulesAndPlaylistsContainingFile(sUid));
        if(contains.nHttpCode == 423)
        {
            theResponse.nHttpCode = 423;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("File with uid '"+sUid+"' is referenced by one or more playlists or schedules.");
            theResponse.jsonData["details"].append(contains.jsonData);
            lg.SetLevel(pml::LOG_ERROR) << "failed - file '" << sUid << "' is referenced by one or more playlists or schedules";
        }
        else
        {
            m_mFiles.erase(sUid);
            remove(std::string(m_sAudioFilePath+sUid).c_str());  //delete the actual file as well

            SaveResources();
            theResponse.jsonData["success"] = true;
            lg << "success";
        }
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::DeleteSchedule(const std::string& sUid)
{
    pml::LogStream lg;
    lg << "ResourceManager\tDeleteSchedule: ";

    pml::restgoose::response theResponse;
    auto itSchedule  = m_mSchedules.find(sUid);
    if(itSchedule == m_mSchedules.end())
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Schedule with uid '"+sUid+"' not found.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - schedule '" << sUid << "' not found";
    }
    else if(itSchedule->second->IsLocked())
    {
        theResponse.nHttpCode = 423;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Schedule with uid '"+sUid+"' is locked.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - schedule '" << sUid << "' is locked";
    }
    else
    {
        m_mSchedules.erase(sUid);
        SaveResources();
        theResponse.jsonData["success"] = true;
        lg << "success";
    }
    return theResponse;

}

pml::restgoose::response ResourceManager::DeletePlaylist(const std::string& sUid)
{
    pml::LogStream lg;
    lg << "ResourceManager\tDeletePlaylist: ";

    pml::restgoose::response theResponse;
    auto itPlaylist  = m_mPlaylists.find(sUid);
    if(itPlaylist == m_mPlaylists.end())
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Playlist with uid '"+sUid+"' not found.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - playlist '" << sUid << "' not found";
    }
    else if(itPlaylist->second->IsLocked())
    {
        theResponse.nHttpCode = 423;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Playlist with uid '"+sUid+"' is locked.");
        lg.SetLevel(pml::LOG_ERROR) << "failed - playlist '" << sUid << "' is locked";
    }
    else
    {
        m_mPlaylists.erase(sUid);
        SaveResources();
        theResponse.jsonData["success"] = true;
        lg << "success";
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::GetFiles()
{

    pml::restgoose::response theResponse;
    theResponse.jsonData = Json::Value(Json::arrayValue);
    for(auto pairFile : m_mFiles)
    {
        Json::Value jsFile;
        jsFile["label"] = pairFile.second->GetLabel();
        jsFile["uid"] = pairFile.second->GetUid();
        theResponse.jsonData.append(jsFile);
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::GetSchedules()
{

    pml::restgoose::response theResponse;
    theResponse.jsonData = Json::Value(Json::arrayValue);
    for(auto pairSchedule : m_mSchedules)
    {
        Json::Value jsSchedule;
        jsSchedule["label"] = pairSchedule.second->GetLabel();
        jsSchedule["uid"] = pairSchedule.second->GetUid();
        theResponse.jsonData.append(jsSchedule);
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::GetPlaylists()
{

    pml::restgoose::response theResponse;
    theResponse.jsonData = Json::Value(Json::arrayValue);
    for(auto pairPlaylist : m_mPlaylists)
    {
        Json::Value jsPlaylist;
        jsPlaylist["label"] = pairPlaylist.second->GetLabel();
        jsPlaylist["uid"] = pairPlaylist.second->GetUid();
        theResponse.jsonData.append(jsPlaylist);
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::GetFile(const std::string& sUid)
{

    pml::restgoose::response theResponse;

    auto itFile = m_mFiles.find(sUid);
    if(itFile != m_mFiles.end())
    {
        theResponse.jsonData = itFile->second->GetJson();
    }
    else
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("File with uid '"+sUid+"' not found.");
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::GetSchedule(const std::string& sUid)
{
    pml::restgoose::response theResponse;

    auto itSchedule = m_mSchedules.find(sUid);
    if(itSchedule != m_mSchedules.end())
    {
        theResponse.jsonData = itSchedule->second->GetJson();
    }
    else
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Schedule with uid '"+sUid+"' not found.");
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::GetPlaylist(const std::string& sUid)
{
    pml::restgoose::response theResponse;

    auto itPlaylist = m_mPlaylists.find(sUid);
    if(itPlaylist != m_mPlaylists.end())
    {
        theResponse.jsonData = itPlaylist->second->GetJson();
    }
    else
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Playlist with uid '"+sUid+"' not found.");
    }
    return theResponse;
}



pml::restgoose::response ResourceManager::ParseResource(const Json::Value& jsData)
{
    pml::restgoose::response theResponse;
    if(jsData["label"].isString() == false  || jsData["label"].asString().empty())
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("No label sent or wrong type");
    }
    if(jsData["description"].isString() == false  || jsData["description"].asString().empty())
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("No description sent or wrong type");
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::ParseFileData(const Json::Value& jsData)
{
    pml::restgoose::response theResponse;

    if(jsData.isMember("files") == false || jsData["files"].asString().empty())
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("No files sent");
        return theResponse;

    }
    else
    {
        return ParseResource(jsData);
    }
}

pml::restgoose::response ResourceManager::ParseFile(const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription)
{
    pml::restgoose::response theResponse(201);

    //now work out the file type...
    switch(WavFile::IsWavFile(sUploadName))
    {
        case SF_ERR_NO_ERROR:
            theResponse.nHttpCode = WAV;
            break;
        case SF_ERR_UNRECOGNISED_FORMAT:
            //@tofo check if MP3 File
            break;
        case SF_ERR_SYSTEM:
            theResponse.nHttpCode = 500;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("System error");
         break;
        case SF_ERR_MALFORMED_FILE:
            theResponse.nHttpCode = 422;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Malformed file");
            break;
        case SF_ERR_UNSUPPORTED_ENCODING:
            theResponse.nHttpCode = 415;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Unsupported encoding");
            break;
        default:
            theResponse.nHttpCode = 500;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Unknown error");
    }
    return theResponse;

}

pml::restgoose::response ResourceManager::ParseSchedule(const Json::Value& jsData)
{
    pml::restgoose::response theResponse(ParseResource(jsData));

    /**
    {
        "files" : [
                        { "uid" : "", "times_to_play" : -1,....n, "cron" : "0 0 0 0 0 0 0" },
                    ],

        "playlists" : [
                        { "uid" : "", "times_to_play" : -1,....n, "cron" : "0 0 0 0 0 0 0", shuffle: true/false},
                    ],

    }
    **/
    if(jsData["files"].isArray() == false && jsData["playlists"].isArray() == false)
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("No schedule sent or wrong type");
        theResponse.jsonData["data"] = jsData;
        return theResponse;
    }


    if(jsData["files"].isArray())
    {
        theResponse = ParseScheduleItems(jsData["files"], std::bind(&ResourceManager::GetFile, this, _1), false);
    }
    if(theResponse.nHttpCode != 400 && jsData["playlists"].isArray())
    {
        theResponse = ParseScheduleItems(jsData["playlists"], std::bind(&ResourceManager::GetPlaylist, this, _1), true);
    }

    if(theResponse.nHttpCode == 400)    //got here and not okay
    {
        theResponse.jsonData["data"] = jsData;
    }

    return theResponse;
}

pml::restgoose::response ResourceManager::ParseScheduleItems(const Json::Value& jsItems, std::function<pml::restgoose::response(const std::string&)> pFind, bool bCheckShuffle)
{
    pml::restgoose::response theResponse;
    //now check the schedule is valid
    for(Json::ArrayIndex i=0; i < jsItems.size(); i++)
    {
        if(jsItems[i].isObject() == false ||
           jsItems[i]["cron"].isString() == false || jsItems[i]["uid"].isString() == false || jsItems[i]["times_to_play"].isInt() == false
           || (bCheckShuffle && jsItems[i]["shuffle"].isBool() == false))
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Not all schedule entries are correct");

            break;
        }
        else if(pFind(jsItems[i]["uid"].asString()).nHttpCode == 400)
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Resource '"+jsItems[i]["uid"].asString()+"' does not exist");
            break;
        }
        else
        {
            CronJob job;
            if(job.SetString(jsItems[i]["cron"].asString()) == false)
            {
                theResponse.nHttpCode = 400;
                theResponse.jsonData["success"] = false;
                theResponse.jsonData["reason"].append("Cron '"+jsItems[i]["cron"].asString()+"' is invalid");
                break;
            }
        }
    }
    return theResponse;
}


pml::restgoose::response ResourceManager::ParsePlaylist(const Json::Value& jsData)
{
    pml::restgoose::response theResponse(ParseResource(jsData));

    /**
    {
        "files" : [
                      { "uid" : "", "times_to_play": n },
                      { "uid" : "", "times_to_play": n }
                      ]
    }
    **/

    if(jsData["files"].isArray() == false)
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("No files sent or wrong type");
        theResponse.jsonData["data"] = jsData;
    }

    if(theResponse.nHttpCode == 400)    //got here and not okay
        return theResponse;

    //now check the schedule is valid
    for(Json::ArrayIndex i=0; i < jsData["files"].size(); i++)
    {
        if(jsData["files"][i].isObject() == false || jsData["files"][i]["uid"].isString() == false)
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Not all files entries are correct");
            theResponse.jsonData["data"] = jsData;
            return theResponse;
        }
        else if(m_mFiles.find(jsData["files"][i]["uid"].asString()) == m_mFiles.end())
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("File '"+jsData["files"][i].asString()+"' does not exist");
            theResponse.jsonData["data"] = jsData;
            return theResponse;
        }
    }
    return theResponse;
}



bool ResourceManager::SaveResources()
{

    //make one big JSON of everything and stream it to a files
    Json::Value jsResources;
    jsResources["created_date"] = GetCurrentTimeAsString(false);
    jsResources["files"] = Json::Value(Json::arrayValue);
    jsResources["schedules"] = Json::Value(Json::arrayValue);
    jsResources["playlists"] = Json::Value(Json::arrayValue);

    for(auto pairFile : m_mFiles)
    {
        jsResources["files"].append(pairFile.second->GetJson());
    }


    for(auto pairSchedule : m_mSchedules)
    {
        jsResources["schedules"].append(pairSchedule.second->GetJson());
    }

    for(auto pairPlaylist : m_mPlaylists)
    {
        jsResources["playlists"].append(pairPlaylist.second->GetJson());
    }


    //atttempt to save file files
    std::ofstream of;
	//attempt to open the file
	of.open(m_sResourcePath+"resources.json", std::ofstream::out | std::ofstream::trunc);
	if(!of.is_open())
	{
        pmlLog(pml::LOG_ERROR) << "ResourceManager\tCould not open resources file!";
		return false;
	}

	of << jsResources << std::endl;
	of.close();

	return true;
}


bool ResourceManager::LoadResources()
{
    std::ifstream ifs;
    ifs.open(m_sResourcePath+"resources.json", std::ifstream::in);
    if(!ifs.is_open())
    {
        pmlLog(pml::LOG_CRITICAL) << "ResourceManager\tCould not open resources file!";
		return false;
	}
	Json::Value jsResources;
	ifs >> jsResources;

    m_mFiles.clear();
    m_mSchedules.clear();
    m_mPlaylists.clear();

    if(jsResources["files"].isArray())
    {
        for(Json::ArrayIndex i = 0; i < jsResources["files"].size(); i++)
        {
            if(jsResources["files"][i]["type"].asString() == "wavfile")
            {
                std::shared_ptr<WavFile> pFile = std::make_shared<WavFile>(jsResources["files"][i]);
                pFile->InitJson();
                m_mFiles.insert(std::make_pair(jsResources["files"][i]["uid"].asString(), pFile));
            }
            else
            {
                // @todo(martim01) mp3
            }
        }
    }

    if(jsResources["schedules"].isArray())
    {
        for(Json::ArrayIndex i = 0; i < jsResources["schedules"].size(); i++)
        {
            m_mSchedules.insert(std::make_pair(jsResources["schedules"][i]["uid"].asString(), std::make_shared<Resource>(jsResources["schedules"][i])));
        }
    }

    if(jsResources["playlists"].isArray())
    {
        for(Json::ArrayIndex i = 0; i < jsResources["playlists"].size(); i++)
        {
            m_mPlaylists.insert(std::make_pair(jsResources["playlists"][i]["uid"].asString(), std::make_shared<Resource>(jsResources["playlists"][i])));
        }
    }

    return true;
}


bool ResourceManager::FileExists(const std::string& sLabel, const std::string& sUid)
{
    for(auto pairResource : m_mFiles)
    {
        if(sUid != pairResource.second->GetUid() && CmpNoCase(pairResource.second->GetLabel(), sLabel))
        {
            return true;
        }
    }
    return false;
}

bool ResourceManager::ResourceExists(const std::string& sLabel, const std::map<std::string, std::shared_ptr<Resource> >& mResource, const std::string& sUid)
{
    for(auto pairResource : mResource)
    {
        if(pairResource.second->GetUid() != sUid && CmpNoCase(pairResource.second->GetLabel(), sLabel))
        {
            return true;
        }
    }
    return false;
}


std::map<std::string, std::shared_ptr<AudioFile> >::const_iterator ResourceManager::GetFilesBegin() const
{
    return m_mFiles.begin();
}

std::map<std::string, std::shared_ptr<AudioFile> >::const_iterator ResourceManager::GetFilesEnd() const
{
    return m_mFiles.end();
}

std::map<std::string, std::shared_ptr<Resource> >::const_iterator ResourceManager::GetSchedulesBegin() const
{
    return m_mSchedules.begin();
}

std::map<std::string, std::shared_ptr<Resource> >::const_iterator ResourceManager::GetSchedulesEnd() const
{
    return m_mSchedules.end();
}

std::map<std::string, std::shared_ptr<Resource> >::const_iterator ResourceManager::GetPlaylistsBegin() const
{
    return m_mPlaylists.begin();
}

std::map<std::string, std::shared_ptr<Resource> >::const_iterator ResourceManager::GetPlaylistsEnd() const
{
    return m_mPlaylists.end();
}

std::map<std::string, std::shared_ptr<AudioFile> >::const_iterator ResourceManager::FindFile(const std::string& sUid) const
{
    return m_mFiles.find(sUid);
}

std::map<std::string, std::shared_ptr<Resource> >::const_iterator ResourceManager::FindSchedule(const std::string& sUid) const
{
    return m_mSchedules.find(sUid);
}

std::map<std::string, std::shared_ptr<Resource> >::const_iterator ResourceManager::FindPlaylist(const std::string& sUid) const
{
    return m_mPlaylists.find(sUid);
}


void ResourceManager::LockResource(const std::string& sUid, bool bLock)
{

    auto itFile = m_mFiles.find(sUid);
    if(itFile != m_mFiles.end())
    {
        itFile->second->Lock(bLock);

    }
    else
    {
        auto itPlaylist = m_mPlaylists.find(sUid);
        if(itPlaylist != m_mPlaylists.end())
        {
            itPlaylist->second->Lock(bLock);
            //lock all the files in the playlist as well....
            for(Json::ArrayIndex i = 0; i < itPlaylist->second->GetJson()["files"].size(); i++)
            {
                LockResource(itPlaylist->second->GetJson()["files"][i]["uid"].asString(), bLock);
            }
        }
        else
        {
            auto itSchedule = m_mSchedules.find(sUid);
            if(itSchedule != m_mSchedules.end())
            {
                itSchedule->second->Lock(bLock);
                for(Json::ArrayIndex i = 0; i < itSchedule->second->GetJson()["files"].size(); i++)
                {
                    LockResource(itSchedule->second->GetJson()["files"][i]["uid"].asString(), bLock);
                }
            }
        }
    }
}


pml::restgoose::response ResourceManager::ModifyStatus(const Json::Value& jsData)
{
    pml::LogStream lg;
    lg.SetLevel(pml::LOG_DEBUG) << "ResourceManager\t" << "ModifyStatus: ";
    if(jsData["command"].isString() == false)
    {
        pml::restgoose::response theResponse(400);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("No command sent");
        lg.SetLevel(pml::LOG_ERROR) << " no command sent";
        return theResponse;
    }

    if(CmpNoCase(jsData["command"].asString(), "play"))
    {
        lg.SetLevel(pml::LOG_INFO) << "play ";
        pml::restgoose::response tr(IsLocked());
        if(tr.nHttpCode == 423)
        {
            return tr;
        }

        return Play(jsData);
    }
    else if(CmpNoCase(jsData["command"].asString(), "pause"))
    {
        lg.SetLevel(pml::LOG_INFO) << "pause";
                pml::restgoose::response tr(IsLocked());
        if(tr.nHttpCode == 423)
        {
            return tr;
        }

        return Pause(jsData);
    }
    else if(CmpNoCase(jsData["command"].asString(), "stop"))
    {
        lg.SetLevel(pml::LOG_INFO) << "stop";
        pml::restgoose::response tr(IsLocked());
        if(tr.nHttpCode == 423)
        {
            return tr;
        }

        return  Stop(jsData);
    }
    else if(CmpNoCase(jsData["command"].asString(), "lock"))
    {
        lg.SetLevel(pml::LOG_INFO) << "lock";
        return  Lock(jsData);
    }
    else if(CmpNoCase(jsData["command"].asString(), "kill"))
    {
        lg.SetLevel(pml::LOG_INFO) << "kill" << std::endl;
        pml::restgoose::response tr(IsLocked());
        if(tr.nHttpCode == 423)
        {
            return tr;
        }

        return  Kill(jsData);
    }
    else
    {
        pml::restgoose::response theResponse(400);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Invalid command sent");
        lg.SetLevel(pml::LOG_ERROR) << "'" << jsData["command"].asString() <<"' is not a valid command" << std::endl;
        return theResponse;
    }

}

pml::restgoose::response ResourceManager::Play(const Json::Value& jsData)
{
    pml::LogStream lg;
    lg.SetLevel(pml::LOG_DEBUG) << "ResourceManager\tPlay: ";

    //check if already playing
    if(m_launcher.IsPlaying())
    {
        if(m_iniConfig.GetIniInt("playout", "autostop",0) == 0) //this means we have to manually stop before playing
        {
            pml::restgoose::response theResponse(409);
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Player already running");
            lg.SetLevel(pml::LOG_WARN) << "- Player already running" << std::endl;
            return theResponse;
        }
        else
        {
            pml::restgoose::response theResponse(Kill(jsData));
            if(theResponse.nHttpCode != 200)
            {
                return theResponse;
            }
        }
    }

    //check if data is valid
    if(jsData["type"].isString() == false)
    {
        pml::restgoose::response theResponse(400);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Type not set");
        lg.SetLevel(pml::LOG_ERROR) << "Type not set" << std::endl;
        return theResponse;
    }
    else if(jsData["uid"].isString() == false)
    {
        pml::restgoose::response theResponse(400);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("uid not set");
        lg.SetLevel(pml::LOG_ERROR) << "uid not set" << std::endl;
        return theResponse;
    }

    //play correct type
    pml::restgoose::response theResponse(400);
    if(CmpNoCase(jsData["type"].asString(), "file"))
    {
        theResponse = PlayFile(jsData);
    }
    else if(CmpNoCase(jsData["type"].asString(), "playlist"))
    {
        theResponse = PlayPlaylist(jsData);
    }
    else if(CmpNoCase(jsData["type"].asString(), "schedule"))
    {
        theResponse = PlaySchedule(jsData);
    }
    else
    {
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("Type not valid");
    }

    //Lock the resource if we managed to play it.
    if(theResponse.nHttpCode == 200)
    {
        m_sResourcePlaying = jsData["uid"].asString();
        m_pPlayingResource = GetResource(m_sResourcePlaying);
        LockResource(m_sResourcePlaying,true);
    }
    return theResponse;
}

pml::restgoose::response ResourceManager::PlayFile(const Json::Value& jsData)
{
    pml::LogStream lg;
    lg.SetLevel(pml::LOG_DEBUG) << "Launcher\tPlay: ";
    auto itFile = FindFile(jsData["uid"].asString());
    if(itFile == GetFilesEnd())
    {
        pml::restgoose::response theResponse(404);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("file not found");
        lg.SetLevel(pml::LOG_ERROR) << "file not found" << std::endl;
        return theResponse;
    }
    else if(jsData["times_to_play"].isInt() == false)
    {
        pml::restgoose::response theResponse(400);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("times_to_play not set");
        lg.SetLevel(pml::LOG_ERROR) << "times_to_play not set" << std::endl;
        return theResponse;
    }
    else
    {
        return m_launcher.LaunchPlayer(itFile->second->GetJson()["type"]=="wavfile" ? "w" : "m", itFile->first, jsData["times_to_play"].asInt());
    }

}

pml::restgoose::response ResourceManager::PlayPlaylist(const Json::Value& jsData)
{
    auto itPlaylist = FindPlaylist(jsData["uid"].asString());
    if(itPlaylist == GetPlaylistsEnd())
    {
        pml::restgoose::response theResponse(404);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("playlist not found");
        return theResponse;
    }
    else if(jsData["times_to_play"].isInt() == false)
    {
        pml::restgoose::response theResponse(400);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("times_to_play not set");
        pmlLog(pml::LOG_ERROR) << "times_to_play not set";
        return theResponse;
    }
    else if(jsData["shuffle"].isBool() == false)
    {
        pml::restgoose::response theResponse(400);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("shuffle not set");
        pmlLog(pml::LOG_ERROR) << "shuffle not set";
        return theResponse;
    }
    else
    {
        return m_launcher.LaunchPlayer("p", itPlaylist->first, jsData["times_to_play"].asInt(), jsData["shuffle"].asBool());
    }
}

pml::restgoose::response ResourceManager::PlaySchedule(const Json::Value& jsData)
{
    auto itSchedule = FindSchedule(jsData["uid"].asString());
    if(itSchedule == GetSchedulesEnd())
    {
        pml::restgoose::response theResponse(404);
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("schedule not found");
        return theResponse;
    }
    else
    {
        return m_launcher.LaunchPlayer("s", itSchedule->first);
    }
}

pml::restgoose::response ResourceManager::Pause(const Json::Value& jsData)
{
    return m_launcher.PausePlayer();
}

pml::restgoose::response ResourceManager::Stop(const Json::Value& jsData)
{
    return m_launcher.StopPlayer();
}

pml::restgoose::response ResourceManager::Kill(const Json::Value& jsData)
{
    pml::restgoose::response theResponse;
    theResponse.jsonData["success"] = true;

    int nError = KillProc("player3");
    if(nError != 0)
    {
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append(strerror(errno));
    }
    nError = KillProc("player67");
    if(nError != 0)
    {
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append(strerror(errno));
    }

    return theResponse;
}

pml::restgoose::response ResourceManager::Lock(const Json::Value& jsData)
{
    pml::restgoose::response theResponse(400);
    if(jsData["lock"].isBool() == false)
    {
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("lock not set");
    }
    if(jsData["password"].isString() == false)
    {
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("password not set");
    }
    bool bAlreadyLocked = (m_iniConfig.GetIniInt("restricted", "locked", 0)==1);
    if(jsData["lock"].asBool() == bAlreadyLocked)
    {
        theResponse.nHttpCode = 202;
        theResponse.jsonData["success"] = true;
        theResponse.jsonData["reason"].append("already in this state");
    }
    else if(jsData["lock"].asBool() == false)
    {
        if(jsData["password"].asString() != m_iniConfig.GetIniString("restricted", "password", ""))
        {
            theResponse.nHttpCode = 403;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("password is incorrect");
        }
        else
        {
            m_iniConfig.ReReadIniFile();    //make sure up to date
            m_iniConfig.SetSectionValue("restricted", "locked", 0);
            m_iniConfig.SetSectionValue("restricted", "password", "");
            m_iniConfig.WriteIniFile();
            theResponse.nHttpCode = 200;
            theResponse.jsonData["success"] = true;
        }
    }
    else
    {
        m_iniConfig.ReReadIniFile();    //make sure up to date
        m_iniConfig.SetSectionValue("restricted", "locked", 1);
        m_iniConfig.SetSectionValue("restricted", "password", jsData["password"].asString());
        m_iniConfig.WriteIniFile();
        theResponse.nHttpCode = 200;
        theResponse.jsonData["success"] = true;
    }
    return theResponse;

}

void ResourceManager::LockPlayingResource(bool bLock)
{
    LockResource(m_sResourcePlaying, bLock);
    if(bLock == false)
    {
        m_sResourcePlaying = "";
        m_pPlayingResource = nullptr;
    }
}


pml::restgoose::response ResourceManager::GetSchedulesAndPlaylistsContainingFile(const std::string& sUid)
{
    pml::restgoose::response theResponse;
    theResponse.jsonData["schedules"] = GetResourcesFileIn(sUid, m_mSchedules);
    theResponse.jsonData["playlists"] = GetResourcesFileIn(sUid, m_mPlaylists);

    if(theResponse.jsonData["schedules"].size() >0 || theResponse.jsonData["playlists"].size() >0)
    {
        theResponse.nHttpCode = 423;
    }
    return theResponse;
}

Json::Value ResourceManager::GetResourcesFileIn(const std::string& sUid, std::map<std::string, std::shared_ptr<Resource> >& mResource)
{
    Json::Value jsResources(Json::arrayValue);
    for(auto pairResource : mResource)
    {
        for(Json::ArrayIndex i = 0; i < pairResource.second->GetJson()["files"].size(); i++)
        {
            if(pairResource.second->GetJson()["files"][i]["uid"] == sUid)
            {
                Json::Value jsValue;
                jsValue["uid"] = pairResource.second->GetUid();
                jsValue["label"] = pairResource.second->GetLabel();
                jsResources.append(jsValue);
            }
        }
    }
    return jsResources;
}

std::shared_ptr<const Resource> ResourceManager::GetPlayingResource()
{
    return m_pPlayingResource;
}
std::shared_ptr<const Resource> ResourceManager::GetResource(const std::string& sUid)
{
    auto itFile = m_mFiles.find(sUid);
    if(itFile != m_mFiles.end())
    {
        return itFile->second;
    }
    auto itPlaylist = m_mPlaylists.find(sUid);
    if(itPlaylist != m_mPlaylists.end())
    {
        return itPlaylist->second;
    }

    auto itSchedule = m_mSchedules.find(sUid);
    if(itSchedule != m_mSchedules.end())
    {
        return itSchedule->second;
    }
    pmlLog(pml::LOG_WARN) << "'" << sUid << "' had no associated resource.";
    return nullptr;
}

pml::restgoose::response ResourceManager::UpdateApplication(const Json::Value& jsData)
{
    pml::LogStream lg;
    lg <<  "ResourceManager\tUpdateApplication: " << jsData << " ";
    return pml::restgoose::response(404, "not working again");
    /*
    pml::restgoose::response theResponse(ParseFiles(jsData));
    if(theResponse.nHttpCode ==400)
    {
        lg.SetLevel(pml::LOG_ERROR) << "failed - incorrect JSON";
    }
    else
    {
        theResponse.nHttpCode = 200;
        //check we have metadata for each of the uploaded files..
        if(jsData["multipart"]["files"]["file"].isString() == false)
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("No file sent ");
            lg.SetLevel(pml::LOG_WARN) << " file not sent.";
        }
        if(jsData["multipart"]["data"]["application"].isString() == false)
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("No metadata set for file: ");
            lg.SetLevel(pml::LOG_WARN) << " file has no metadata.";
            remove(jsData["multipart"]["files"]["file"].asString().c_str());
        }
        else if(jsData["multipart"]["data"]["application"].asString() == "episerver")
        {
            return Update("episerver", CreatePath(m_iniConfig.GetIniString("paths", "server",".")), jsData["multipart"]["files"]["file"].asString());
        }
        else if(jsData["multipart"]["data"]["application"].asString() == "player3" || jsData["multipart"]["data"]["application"].asString() == "player67")
        {
            return Update(jsData["multipart"]["data"]["application"].asString(),
                          CreatePath(m_iniConfig.GetIniString("paths", "player",".")), jsData["multipart"]["files"]["file"].asString());
        }
        else
        {
            theResponse.nHttpCode = 404;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Application not found ");
            lg.SetLevel(pml::LOG_WARN) << " application does not exist.";
            remove(jsData["multipart"]["files"]["file"].asString().c_str());
        }
    }
    lg.SetLevel(pml::LOG_DEBUG);
    return theResponse;
    */
}

pml::restgoose::response ResourceManager::Update(const std::string& sApplication, const std::string& sPath, const std::string& sUpdateFile)
{
    pml::restgoose::response theResponse;

    std::stringstream ssApp;
    ssApp << sPath << sApplication;

    //is the application running?
    if(IsProcRunning(sApplication))
    {   //if so rename the application
        std::stringstream ssOld;

        ssOld << sPath << sApplication << ".old";

        if(rename(ssApp.str().c_str(), ssOld.str().c_str()) != 0)
        {
            theResponse.nHttpCode = 500;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append(strerror(errno));
            theResponse.jsonData["reason"].append(ssApp.str());
            theResponse.jsonData["reason"].append(ssOld.str());
        }
        else
        {
            m_iniConfig.ReReadIniFile();    //make sure up to date
            m_iniConfig.SetSectionValue("remove", sApplication, ssOld.str());
            m_iniConfig.WriteIniFile();
            theResponse.jsonData["restart"] = true;
        }
    }

    if(theResponse.nHttpCode == 200)
    {
        //copy the update file across
        std::ifstream temp(sUpdateFile, std::ios::binary);
        std::ofstream app(ssApp.str(), std::ios::binary);
        if(!temp)
        {
            theResponse.nHttpCode = 500;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Could not open update file for reading");
        }
        else if(!app)
        {
            theResponse.nHttpCode = 500;
            theResponse.jsonData["success"] = false;
            theResponse.jsonData["reason"].append("Could not open application file for writing");
        }
        else
        {
            app << temp.rdbuf();
            app.close();
            temp.close();
            theResponse.jsonData["success"] = true;
            chmod(ssApp.str().c_str(), 0755);
        }
    }

    remove(sUpdateFile.c_str());

    return theResponse;
}

pml::restgoose::response ResourceManager::IsLocked()
{
    pml::restgoose::response theResponse(200);
    if(m_iniConfig.GetIniInt("restricted", "locked",0) == 1)
    {
        theResponse.nHttpCode = 423;
        theResponse.jsonData["success"] = false;
        theResponse.jsonData["reason"].append("The system has been locked");
    }
    return theResponse;
}
