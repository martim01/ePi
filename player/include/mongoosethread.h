#pragma once

#include "mongoose.h"
#include <string>
#include <sstream>
#include <mutex>
#include <map>
#include <set>
#include <queue>
#include <vector>
#include <initializer_list>
#include "inimanager.h"
#include "response.h"

class ResourceManager;

class MongooseThread
{
    public:

        MongooseThread(const iniManager& iniConfig, ResourceManager& manager);

        ///< @brief Creates the thread that runs the webserver loop
        void Run();


        /** Handles a logout event
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void HandleEvent(mg_connection *pConnection, int nEvent, void* pData);


    private:



        ///< @brief the main mongoose loop. Called in a separate thread by Run()
        void Loop();

        /** @brief Handles all Mongoose websocket events
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void EventWebsocket(mg_connection *pConnection, int nEvent, void* pData);

        /** @brief Handles all Mongoose http request events
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void EventHttp(mg_connection *pConnection, int nEvent, void* pData);


        /** @brief Send a JSON encoded error message to the provided connection containing the provided error
        *   @param pConnection the mg_connection to send the data to
        *   @param sError the error message
        *   @param nCode the error code
        **/
        void SendError(mg_connection* pConnection, const std::string& sError, int nCode=-1);


        /** @brief Handle the browser sendin us an audio file
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void HandleFileUpload(mg_connection* pConnection, http_message* pMessage);

        /** @brief Called when a network source file us started to be uploaded. Updates version numbers and status of the ns ini file so no other files can be uploaded as weel

        **/
        bool UploadAllowed(mg_connection* pConnection, http_message* pMessage);

        /** @brief Called when the network source file has finished uploading. Copies the file to the working directory and executes the network source compiler
        **/
        void EndUpload(mg_connection *pConnection);

        void SendStatus();


        void DoReply(mg_connection* pConnection, const response& theResponse);

        void DoHttpGet(mg_connection* pConnection, const std::string& sUrl);
        void DoHttpPut(mg_connection* pConnection, const std::string& sUrl, http_message* pMessage);
        void DoHttpDelete(mg_connection* pConnection, const std::string& sUrl, http_message* pMessage);
        void DoHttpPost(mg_connection* pConnection, const std::string& sUrl, http_message* pMessage);


        void DoGetFiles(mg_connection* pConnection, std::queue<std::string>& qSplit);
        void DoGetPlaylists(mg_connection* pConnection, std::queue<std::string>& qSplit);
        void DoGetSchedules(mg_connection* pConnection, std::queue<std::string>& qSplit);
        void DoGetConfig(mg_connection* pConnection);
        void DoGetInfo(mg_connection* pConnection);
        void DoGetPower(mg_connection* pConnection);

        void StatusChange(mg_connection* pConnection, http_message* pMessage);
        void PowerChange(mg_connection* pConnection, http_message* pMessage);
        void ConfigChange(mg_connection* pConnection, http_message* pMessage);
        void FileMove(mg_connection* pConnection, http_message* pMessage, const std::string& sUid);
        void PlaylistModify(mg_connection* pConnection, http_message* pMessage, const std::string& sUid);
        void ScheduleModify(mg_connection* pConnection, http_message* pMessage, const std::string& sUid);


        void FileDelete(mg_connection* pConnection, http_message* pMessage, const std::string& sUid);
        void PlaylistDelete(mg_connection* pConnection, http_message* pMessage, const std::string& sUid);
        void ScheduleDelete(mg_connection* pConnection, http_message* pMessage, const std::string& sUid);

        void FileUpload(mg_connection* pConnection, http_message* pMessage);
        void PlaylistUpload(mg_connection* pConnection, http_message* pMessage);
        void ScheduleUpload(mg_connection* pConnection, http_message* pMessage);

        ResourceManager& m_manager;

        mg_connection* m_pConnection;
        std::string m_sIniPath;
        std::string m_sServerName;

        mg_mgr m_mgr;

        mg_serve_http_opts m_ServerOpts;

        std::mutex m_mutex;

        bool m_bUploadAllowed;

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

        static const std::string EP_ROOT;
        static const std::string EP_STATUS;
        static const std::string EP_POWER;
        static const std::string EP_CONFIG;
        static const std::string EP_SCHEDULES;
        static const std::string EP_PLAYLISTS;
        static const std::string EP_FILES;
        static const std::string EP_INFO;

};
