#pragma once
#include "response.h"
#include "resourcemanager.h"
#include "sysinfomanager.h"
#include "launcher.h"
#include "json/json.h"
#include "RestGoose.h"

using postData = std::vector<pml::restgoose::partData>;
class Core
{
    public:
        Core();

        void Run(const std::string& sConfigFile);



        pml::restgoose::response GetRoot(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetEpi(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);

        pml::restgoose::response GetFiles(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetPlaylists(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetSchedules(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetConfig(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetInfo(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetPower(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetPlaylist(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetSchedule(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetStatus(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetUpdate(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response GetOutputs(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);


        pml::restgoose::response PatchStatus(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response PatchConfig(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response PatchFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);

        pml::restgoose::response PutFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);

        pml::restgoose::response PutPlaylist(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response PutSchedule(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response PutPower(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response PutUpdate(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);


        pml::restgoose::response DeleteFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response DeletePlaylist(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response DeleteSchedule(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);

        pml::restgoose::response PostFile(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response PostPlaylist(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);
        pml::restgoose::response PostSchedule(const query& theQuery, const postData& vData, const endpoint& theEndpoint, const userName& theUser);


        void StatusCallback(const std::string& sData);
        void ExitCallback(int nPid);
        void LoopCallback(std::chrono::milliseconds durationSince);

        bool WebsocketAuthenticate(const endpoint& theEndpoint, const userName& theUser, const ipAddress& peer);
        bool WebsocketMessage(const endpoint& theEndpoint, const Json::Value& jsData);
        void WebsocketClosed(const endpoint& theEndpoint, const ipAddress& peer);


    private:
        Launcher m_launcher;
        ResourceManager m_manager;
        pml::restgoose::Server m_server;
        SysInfoManager m_info;
        iniManager m_iniConfig;

        void InitLogging();
        bool CreateEndpoints();

        pml::restgoose::response Reboot(int nCommand);

        void GetInitialPlayerStatus();

        void PatchServerConfig(const Json::Value& jsData);

        enum enumType{FILE, PLAYLIST, SCHEDULE};
        enum enumModification{MODIFIED, ADDED, DELETED};

        void ResourceModified(enumType eType, const std::string& sUid, enumModification eModification);

        Json::Value m_jsStatus;
        Json::Value m_jsResources;

         /**
        x-epi                               GET
        x-epi/status                        GET PUT
        x-epi/files                         GET     POST
        x-epi/files/xxx                     GET PUT
        x-epi/playlists                     GET     POST
        x-epi/playlists/xxx                 GET PUT
        x-epi/schedules                     GET     POST
        x-epi/schedules/xxx                 GET PUT
        x-epi/power                         GET PUT
        x-epi/config                        GET PUT
        x-epi/info                          GET
        **/

        static const endpoint EP_ROOT;
        static const endpoint EP_EPI;
        static const endpoint EP_STATUS;
        static const endpoint EP_POWER;
        static const endpoint EP_CONFIG;
        static const endpoint EP_SCHEDULES;
        static const endpoint EP_PLAYLISTS;
        static const endpoint EP_FILES;
        static const endpoint EP_INFO;
        static const endpoint EP_UPDATE;
        static const endpoint EP_OUTPUTS;
        static const endpoint EP_WS;

        static const std::string ROOT;
        static const std::string EPI;
        static const std::string STATUS;
        static const std::string POWER;
        static const std::string CONFIG;
        static const std::string SCHEDULES;
        static const std::string PLAYLISTS;
        static const std::string FILES;
        static const std::string INFO;
        static const std::string UPDATE;
        static const std::string OUTPUTS;
        static const std::string WS;

        std::mutex m_mutex;

        unsigned int m_nTimeSinceLastCall;

        size_t m_nLogToConsole;
        size_t m_nLogToFile;
        bool m_bLoggedThisHour;
};
