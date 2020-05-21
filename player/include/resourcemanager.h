#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include "json/json.h"
#include "inimanager.h"
#include "response.h"

class AudioFile;
class Resource;


class ResourceManager
{
    public:
        ResourceManager(const iniManager& iniConfig);

        ~ResourceManager();

        response AddFile(const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription);
        response AddSchedule(const Json::Value& jsData);
        response AddPlaylist(const Json::Value& jsData);

        response ModifyFile(const std::string& sUid, const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription);
        response ModifySchedule(const std::string& sUid, const Json::Value& jsData);
        response ModifyPlaylist(const std::string& sUid, const Json::Value& jsData);

        response DeleteFile(const std::string& sUid);
        response DeleteSchedule(const std::string& sUid);
        response DeletePlaylist(const std::string& sUid);

        response GetFiles();
        response GetSchedules();
        response GetPlaylists();

        response GetFile(const std::string& sUid);
        response GetSchedule(const std::string& sUid);
        response GetPlaylist(const std::string& sUid);


    private:
        bool LoadResources();
        bool SaveResources();

        response ParseResource(const Json::Value& jsData);
        response ParseFile(const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription);
        response ParseSchedule(const Json::Value& jsData);
        response ParsePlaylist(const Json::Value& jsData);


        std::string m_sAudioFilePath;
        std::string m_sResourcePath;
        std::string m_sLogPath;

        std::map<std::string, std::shared_ptr<AudioFile> > m_mFiles;
        std::map<std::string, std::shared_ptr<Resource> > m_mSchedules;
        std::map<std::string, std::shared_ptr<Resource> > m_mPlaylists;

        std::mutex m_mutex;

        enum {WAV=250, MP3=251};
};
