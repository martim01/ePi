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
        ResourceManager();
        ~ResourceManager();

        void Init(const iniManager& iniConfig);

        response AddFiles(const Json::Value& jsData);
        response AddSchedule(const Json::Value& jsData);
        response AddPlaylist(const Json::Value& jsData);

        response ModifyFile(const std::string& sUid, const Json::Value& jsData);

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

        const std::string& GetAudioPath() { return m_sAudioFilePath; }

        std::map<std::string, std::shared_ptr<AudioFile> >::const_iterator GetFilesBegin() const;
        std::map<std::string, std::shared_ptr<AudioFile> >::const_iterator GetFilesEnd() const;

        std::map<std::string, std::shared_ptr<Resource> >::const_iterator GetSchedulesBegin() const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator GetSchedulesEnd() const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator GetPlaylistsBegin() const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator GetPlaylistsEnd() const;

        std::map<std::string, std::shared_ptr<AudioFile> >::const_iterator FindFile(const std::string& sUid) const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator FindSchedule(const std::string& sUid) const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator FindPlaylist(const std::string& sUid) const;

    private:
        bool LoadResources();
        bool SaveResources();

        response AddFile(const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription);
        response ModifyFileMeta(const std::map<std::string, std::shared_ptr<AudioFile> >::iterator& itFile, const Json::Value& jsData);

        response ParseFiles(const Json::Value& jsData);
        response ParseResource(const Json::Value& jsData);
        response ParseFile(const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription);
        response ParseSchedule(const Json::Value& jsData);
        response ParsePlaylist(const Json::Value& jsData);

        bool FileExists(const std::string& sLabel);
        bool ResourceExists(const std::string& sLabel, const std::map<std::string, std::shared_ptr<Resource> >& mResource);


        std::string m_sAudioFilePath;
        std::string m_sResourcePath;
        std::string m_sLogPath;

        std::map<std::string, std::shared_ptr<AudioFile> > m_mFiles;
        std::map<std::string, std::shared_ptr<Resource> > m_mSchedules;
        std::map<std::string, std::shared_ptr<Resource> > m_mPlaylists;

        std::mutex m_mutex;

        enum {WAV=250, MP3=251};
};
