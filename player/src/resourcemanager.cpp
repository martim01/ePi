#include "resourcemanager.h"
#include <iostream>
#include <fstream>
#include "utils.h"
#include "audiofile.h"
#include "wavfile.h"
#include "mp3file.h"
#include "sndfile.hh"
#include "log.h"


ResourceManager::ResourceManager(const iniManager& iniConfig)
{
    m_sAudioFilePath = CreatePath(iniConfig.GetIniString("Paths", "Audio", "/var/ePi/audio"));
    m_sResourcePath = CreatePath(iniConfig.GetIniString("Paths", "Resources", "/var/ePi/resources"));

    mkpath(m_sAudioFilePath, 0755);
    mkpath(m_sResourcePath, 0755);

    LoadResources();
}

ResourceManager::~ResourceManager()
{
    SaveResources();
}

response ResourceManager::AddFile( const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    pml::Log::Get() << "ResourceManager\tAddFile: ";
    response theResponse(ParseFile(sUploadName, sLabel, sDescription));

    if(theResponse.nHttpCode ==400)
    {
        pml::Log::Get(pml::Log::LOG_ERROR) << "failed - incorrect JSON" << std::endl;
        return theResponse;
    }

    std::string sUid(CreateGuid());
    //@todo copy the file across with the new name
    std::shared_ptr<AudioFile> pFile;

    if(theResponse.nHttpCode == WAV)
    {
        pFile = std::dynamic_pointer_cast<AudioFile>(std::make_shared<WavFile>(sUid, sLabel, sDescription));
    }
    else
    {
        //@todo mp3file
    }

    if(m_mFiles.insert(std::make_pair(sUid, pFile)).second)
    {
        SaveResources();
        theResponse.nHttpCode = 201;
        theResponse.jsonData["result"] = "Success";
        theResponse.jsonData["uid"] = sUid;
    }
    else
    {
        pml::Log::Get(pml::Log::LOG_ERROR) << "failed - could not create uid" << std::endl;
        theResponse.nHttpCode = 400;
        theResponse.jsonData["result"] = "Unable to create unique id for item";
    }
    pml::Log::Get() << "success" << std::endl;

    return theResponse;

}

response ResourceManager::AddSchedule(const Json::Value& jsData)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    pml::Log::Get() << "ResourceManager\tAddSchedule: ";

    response theResponse(ParseSchedule(jsData));
    if(theResponse.nHttpCode == 200)
    {
        //all valid so create the new schedule item
        std::string sUid(CreateGuid());
        if(m_mSchedules.insert(std::make_pair(sUid, std::make_shared<Resource>("Schedule", sUid, jsData))).second)
        {
            SaveResources();
            theResponse.nHttpCode = 201;
            theResponse.jsonData["result"] = "Success";
            theResponse.jsonData["uid"] = sUid;
            pml::Log::Get() << "success" << std::endl;
        }
        else
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["result"] = "Unable to create unique id for item";
            pml::Log::Get(pml::Log::LOG_ERROR) << "failed - unable to create uid" << std::endl;
        }
    }
    else
    {
        pml::Log::Get(pml::Log::LOG_ERROR) << "failed - incorrect JSON" << std::endl;
    }
    return theResponse;
}


response ResourceManager::AddPlaylist(const Json::Value& jsData)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    pml::Log::Get() << "ResourceManager\tAddPlaylist: ";

    response theResponse(ParsePlaylist(jsData));
    if(theResponse.nHttpCode == 200)
    {
        //all valid so create the new Playlist item
        std::string sUid(CreateGuid());
        if(m_mPlaylists.insert(std::make_pair(sUid, std::make_shared<Resource>("Playlist", sUid, jsData))).second)
        {
            SaveResources();
            theResponse.nHttpCode = 201;
            theResponse.jsonData["result"] = "Success";
            theResponse.jsonData["uid"] = sUid;
            pml::Log::Get() << "success" << std::endl;
        }
        else
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["result"] = "Unable to create unique id for item";

            pml::Log::Get(pml::Log::LOG_ERROR) << "failed - unable to create uid" << std::endl;
        }
    }
    else
    {
        pml::Log::Get(pml::Log::LOG_ERROR) << "failed - incorrect JSON" << std::endl;
    }
    return theResponse;
}

response ResourceManager::ModifyFile(const std::string& sUid, const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    pml::Log::Get() << "ResourceManager\tModifyFile: ";

    response theResponse;
    // @todo
    return theResponse;
}

response ResourceManager::ModifySchedule(const std::string& sUid, const Json::Value& jsData)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    pml::Log::Get() << "ResourceManager\tModifySchedule: ";

    response theResponse;
    auto itSchedule = m_mSchedules.find(sUid);
    if(itSchedule == m_mSchedules.end())
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["result"] = "Schedule with uid '"+sUid+"' not found.";
        pml::Log::Get(pml::Log::LOG_ERROR) << "failed - schedule '" << sUid << "'not found" << std::endl;
    }
    else
    {
        theResponse = ParseSchedule(jsData);
        if(theResponse.nHttpCode == 200)
        {
            itSchedule->second->UpdateJson(jsData);
            theResponse.jsonData["result"] = "Success";
            pml::Log::Get() << "success" << std::endl;
            SaveResources();
        }
        else
        {
            pml::Log::Get(pml::Log::LOG_ERROR) << "failed - incorrect JSON" << std::endl;
        }
    }
    return theResponse;
}

response ResourceManager::ModifyPlaylist(const std::string& sUid, const Json::Value& jsData)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    pml::Log::Get() << "ResourceManager\tModifyPlaylist: ";

    response theResponse;
    auto itPlaylist = m_mPlaylists.find(sUid);
    if(itPlaylist == m_mPlaylists.end())
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["result"] = "Playlist with uid '"+sUid+"' not found.";
        pml::Log::Get(pml::Log::LOG_ERROR) << "failed - playlist '" << sUid << "' not found" << std::endl;
    }
    else
    {
        theResponse = ParsePlaylist(jsData);
        if(theResponse.nHttpCode == 200)
        {
            itPlaylist->second->UpdateJson(jsData);
            theResponse.jsonData["result"] = "Success";
            pml::Log::Get() << "success" << std::endl;
            SaveResources();
        }
        else
        {
            pml::Log::Get(pml::Log::LOG_ERROR) << "failed - incorrect JSON" << std::endl;
        }
    }
    return theResponse;
}

response ResourceManager::DeleteFile(const std::string& sUid)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    pml::Log::Get() << "ResourceManager\tDeleteFile: ";

    //todo find file
    //delete file from disk
    //remove file from memeoy
    //save resources
    //return response
    response theResponse;
    //@todo
    return theResponse;
}

response ResourceManager::DeleteSchedule(const std::string& sUid)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    pml::Log::Get() << "ResourceManager\tDeleteSchedule: ";

    response theResponse;
    //@todo
    return theResponse;

}

response ResourceManager::DeletePlaylist(const std::string& sUid)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    pml::Log::Get() << "ResourceManager\tDeletePlaylist: ";

    response theResponse;
    //@todo
    return theResponse;
}

response ResourceManager::GetFiles()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    response theResponse;
    for(auto pairFile : m_mFiles)
    {
        Json::Value jsFile;
        jsFile["label"] = pairFile.second->GetLabel();
        jsFile["uid"] = pairFile.second->GetUid();
        theResponse.jsonData.append(jsFile);
    }
    return theResponse;
}

response ResourceManager::GetSchedules()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    response theResponse;
    for(auto pairSchedule : m_mSchedules)
    {
        Json::Value jsSchedule;
        jsSchedule["label"] = pairSchedule.second->GetLabel();
        jsSchedule["uid"] = pairSchedule.second->GetUid();
        theResponse.jsonData.append(jsSchedule);
    }
    return theResponse;
}

response ResourceManager::GetPlaylists()
{
    std::lock_guard<std::mutex> lg(m_mutex);

    response theResponse;
    for(auto pairPlaylist : m_mPlaylists)
    {
        Json::Value jsPlaylist;
        jsPlaylist["label"] = pairPlaylist.second->GetLabel();
        jsPlaylist["uid"] = pairPlaylist.second->GetUid();
        theResponse.jsonData.append(jsPlaylist);
    }
    return theResponse;
}

response ResourceManager::GetFile(const std::string& sUid)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    response theResponse;

    auto itFile = m_mFiles.find(sUid);
    if(itFile != m_mFiles.end())
    {
        theResponse.jsonData = itFile->second->GetJson();
    }
    else
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["result"] = "File with uid '"+sUid+"' not found.";
    }
    return theResponse;
}

response ResourceManager::GetSchedule(const std::string& sUid)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    response theResponse;

    auto itSchedule = m_mSchedules.find(sUid);
    if(itSchedule != m_mSchedules.end())
    {
        theResponse.jsonData = itSchedule->second->GetJson();
    }
    else
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["result"] = "Schedule with uid '"+sUid+"' not found.";
    }
    return theResponse;
}

response ResourceManager::GetPlaylist(const std::string& sUid)
{
    std::lock_guard<std::mutex> lg(m_mutex);

    response theResponse;

    auto itPlaylist = m_mPlaylists.find(sUid);
    if(itPlaylist != m_mPlaylists.end())
    {
        theResponse.jsonData = itPlaylist->second->GetJson();
    }
    else
    {
        theResponse.nHttpCode = 404;
        theResponse.jsonData["result"] = "Playlist with uid '"+sUid+"' not found.";
    }
    return theResponse;
}



response ResourceManager::ParseResource(const Json::Value& jsData)
{
    response theResponse;
    if(jsData["label"].isString() == false  || jsData["label"].asString().empty())
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["label"] = "No label sent or wrong type";
    }
    if(jsData["description"].isString() == false  || jsData["description"].asString().empty())
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["description"] = "No description sent or wrong type";
    }
    return theResponse;
}


response ResourceManager::ParseFile(const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription)
{
    response theResponse(201);
    if(sLabel.empty())
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["label"] = "Label is empty";
    }
    if(sDescription.empty())
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["description"] = "Description is empty";
    }

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
            theResponse.nHttpCode = 400;
            theResponse.jsonData["file"] = "System error";
         break;
        case SF_ERR_MALFORMED_FILE:
            theResponse.nHttpCode = 400;
            theResponse.jsonData["file"] = "Malformed file";
            break;
        case SF_ERR_UNSUPPORTED_ENCODING:
            theResponse.nHttpCode = 400;
            theResponse.jsonData["file"] = "Unsupported encoding";
            break;
    }
    return theResponse;

}

response ResourceManager::ParseSchedule(const Json::Value& jsData)
{
    response theResponse(ParseResource(jsData));

    if(jsData["schedule"].isArray() == false)
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["schedule"] = "No schedule sent or wrong type";
    }

    if(theResponse.nHttpCode == 400)    //got here and not okay
        return theResponse;

    //now check the schedule is valid
    for(size_t i=0; i < jsData["schedule"].size(); i++)
    {
        if(jsData["schedule"][i].isObject() == false ||
           jsData["schedule"][i]["offset"].isUInt64() == false || jsData["schedule"][i]["file"].isString() == false || jsData["schedule"][i]["loop"].isBool() == false)
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["schedule"]="Not all schedule entries are correct";
            return theResponse;
        }
        else if(m_mFiles.find(jsData["schedule"][i]["file"].asString()) == m_mFiles.end())
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["schedule"]="File '"+jsData["schedule"][i]["file"].asString()+"' does not exist";
            return theResponse;
        }
    }
    return theResponse;
}


response ResourceManager::ParsePlaylist(const Json::Value& jsData)
{
    response theResponse(ParseResource(jsData));

    if(jsData["playlist"].isArray() == false)
    {
        theResponse.nHttpCode = 400;
        theResponse.jsonData["playlist"] = "No playlist sent or wrong type";
    }

    if(theResponse.nHttpCode == 400)    //got here and not okay
        return theResponse;

    //now check the schedule is valid
    for(size_t i=0; i < jsData["playlist"].size(); i++)
    {
        if(jsData["playlist"][i].isString() == false)
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["playlist"]="Not all playlist entries are correct";
            return theResponse;
        }
        else if(m_mFiles.find(jsData["playlist"][i].asString()) == m_mFiles.end())
        {
            theResponse.nHttpCode = 400;
            theResponse.jsonData["playlist"]="File '"+jsData["playlist"][i].asString()+"' does not exist";
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
        pml::Log::Get(pml::Log::LOG_ERROR) << "ResourceManager\tCould not open resources file!" << std::endl;
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
        pml::Log::Get(pml::Log::LOG_CRITICAL) << "ResourceManager\tCould not open resources file!" << std::endl;
		return false;
	}
	Json::Value jsResources;
	ifs >> jsResources;

    m_mFiles.clear();
    m_mSchedules.clear();
    m_mPlaylists.clear();

    if(jsResources["files"].isArray())
    {
        for(size_t i = 0; i < jsResources["files"].size(); i++)
        {
            if(jsResources["files"][i]["type"].asString() == "wavfile")
            {
                m_mFiles.insert(std::make_pair(jsResources["files"][i]["uid"].asString(), std::make_shared<WavFile>(jsResources["files"][i])));
            }
            else
            {
                //@todo mp3
            }
        }
    }

    if(jsResources["schedules"].isArray())
    {
        for(size_t i = 0; i < jsResources["schedules"].size(); i++)
        {
            m_mSchedules.insert(std::make_pair(jsResources["schedules"][i]["uid"].asString(), std::make_shared<Resource>(jsResources["schedules"][i])));
        }
    }

    if(jsResources["playlists"].isArray())
    {
        for(size_t i = 0; i < jsResources["playlists"].size(); i++)
        {
            m_mPlaylists.insert(std::make_pair(jsResources["playlists"][i]["uid"].asString(), std::make_shared<Resource>(jsResources["schedules"][i])));
        }
    }

    return true;
}
