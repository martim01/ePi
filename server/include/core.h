#pragma once
#include "response.h"
#include "mongooseserver.h"
#include "resourcemanager.h"
#include "sysinfomanager.h"
#include "launcher.h"
#include "json/json.h"


class Core
{
    public:
        Core();

        void Run(const std::string& sConfigFile);



        response GetRoot(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetEpi(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);

        response GetFiles(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetPlaylists(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetSchedules(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetConfig(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetInfo(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetPower(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetStatus(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetUpdate(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response GetOutputs(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);


        response PatchStatus(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response PatchConfig(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response PatchFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);

        response PutFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);

        response PutPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response PutSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response PutPower(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response PutUpdate(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);


        response DeleteFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response DeletePlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response DeleteSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);

        response PostFile(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response PostPlaylist(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);
        response PostSchedule(mg_connection* pConnection, const query& theQuery, const postData& theData, const url& theUrl);


        void StatusCallback(const std::string& sData);
        void ExitCallback(int nPid);
        void LoopCallback(int nData);


    private:
        Launcher m_launcher;
        ResourceManager m_manager;
        MongooseServer m_server;
        SysInfoManager m_info;
        iniManager m_iniConfig;

        bool CreateEndpoints();

        response Reboot(int nCommand);


        Json::Value m_jsStatus;

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

        static const url EP_ROOT;
        static const url EP_EPI;
        static const url EP_STATUS;
        static const url EP_POWER;
        static const url EP_CONFIG;
        static const url EP_SCHEDULES;
        static const url EP_PLAYLISTS;
        static const url EP_FILES;
        static const url EP_INFO;
        static const url EP_UPDATE;
        static const url EP_OUTPUTS;

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

        std::mutex m_mutex;

        unsigned int m_nTimeSinceLastCall;
};
