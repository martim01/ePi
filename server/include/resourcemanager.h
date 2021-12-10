#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include "json/json.h"
#include "inimanager.h"
#include "response.h"
#include "launcher.h"
#include <functional>


class AudioFile;
class Resource;


class ResourceManager
{
    public:
        ResourceManager(Launcher& launcher, iniManager& iniConfig);
        ~ResourceManager();

        void Init();

        response AddFiles(const Json::Value& jsData);
        response AddSchedule(const Json::Value& jsData);
        response AddPlaylist(const Json::Value& jsData);

        response ModifyFile(const std::string& sUid, const Json::Value& jsData);
        response ModifyFileMeta(const std::string& sUid, const Json::Value& jsData);

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

        response ModifyStatus(const Json::Value& jsData);

        response UpdateApplication(const Json::Value& jsData);


        std::map<std::string, std::shared_ptr<AudioFile> >::const_iterator GetFilesBegin() const;
        std::map<std::string, std::shared_ptr<AudioFile> >::const_iterator GetFilesEnd() const;

        std::map<std::string, std::shared_ptr<Resource> >::const_iterator GetSchedulesBegin() const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator GetSchedulesEnd() const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator GetPlaylistsBegin() const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator GetPlaylistsEnd() const;

        std::map<std::string, std::shared_ptr<AudioFile> >::const_iterator FindFile(const std::string& sUid) const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator FindSchedule(const std::string& sUid) const;
        std::map<std::string, std::shared_ptr<Resource> >::const_iterator FindPlaylist(const std::string& sUid) const;

        void LockPlayingResource(bool bLock);
        const std::string& GetAudioPath() { return m_sAudioFilePath; }

        std::shared_ptr<const Resource> GetPlayingResource();

        response IsLocked();
        void InitPaths();

    private:
        bool LoadResources();
        bool SaveResources();

        void LockResource(const std::string& sUid, bool bLock);

        response AddFile(const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription);



        response ParseFileData(const Json::Value& jsData);
        response ParseResource(const Json::Value& jsData);
        response ParseFile(const std::string& sUploadName, const std::string& sLabel, const std::string& sDescription);
        response ParseSchedule(const Json::Value& jsData);

        response ParseScheduleItems(const Json::Value& jsItems, std::function<response(const std::string&)> pFind, bool bCheckShuffle);
        response ParsePlaylist(const Json::Value& jsData);


        bool FileExists(const std::string& sLabel, const std::string& sUid="");
        bool ResourceExists(const std::string& sLabel, const std::map<std::string, std::shared_ptr<Resource> >& mResource, const std::string& sUid="");


        response Play(const Json::Value& jsData);
        response Pause(const Json::Value& jsData);
        response Stop(const Json::Value& jsData);
        response Lock(const Json::Value& jsData);
        response Kill(const Json::Value& jsData);

        response PlayFile(const Json::Value& jsData);
        response PlayPlaylist(const Json::Value& jsData);
        response PlaySchedule(const Json::Value& jsData);

        response GetSchedulesAndPlaylistsContainingFile(const std::string& sUid);
        Json::Value GetResourcesFileIn(const std::string& sUid, std::map<std::string, std::shared_ptr<Resource> >& mResource);

        response Update(const std::string& sApplication, const std::string& sPath, const std::string& sUpdateFile);



        std::shared_ptr<const Resource> GetResource(const std::string& sUid);

        std::string m_sAudioFilePath;
        std::string m_sResourcePath;
        std::string m_sLogPath;

        std::map<std::string, std::shared_ptr<AudioFile> > m_mFiles;
        std::map<std::string, std::shared_ptr<Resource> > m_mSchedules;
        std::map<std::string, std::shared_ptr<Resource> > m_mPlaylists;

        std::mutex m_mutex;

        Launcher& m_launcher;
        iniManager& m_iniConfig;
        std::string m_sResourcePlaying;
        std::shared_ptr<const Resource> m_pPlayingResource;

        enum {WAV=250, MP3=251};
};
