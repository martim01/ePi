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


class MongooseThread
{
    public:

        ///< @brief Get the singleton class
        static MongooseThread& Get();

        /** @brief Initialises the class
        **/
        void Init(iniManager& iniConfig);

        ///< @brief Creates the thread that runs the webserver loop
        void Run();

        /** @brief Sends the given message with the given id to any websocket that is interested in messages with that id
        *   @param sId the id of the message (made up of pipe+id)
        *   @param sMessage the JSON encoded message
        **/
        void SendWSMessage(const std::string& sId, std::string sMessage);

        /** Handles a logout event
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void HandleEvent(mg_connection *pConnection, int nEvent, void* pData);


    private:

        MongooseThread();

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

        /** @brief Sends all the current status to the passed connection. Called when pConnection first connects
        *   @param pConnection the mg_connection to send the data to
        **/
        void SendAll(mg_connection* pConnection);

        /** @brief Sends a message to pConnection telling the browser to redirect to the login page as its session has expired
        *   @param pConnection the mg_connection to send the data to
        **/
        void SendLogOut(mg_connection* pConnection);

        /** @brief Checks whether the passed username and password exist and match
        *   @param pConnection the mg_connection to send the data to
        *   @param sUsername the passed username
        *   @param sPassword the passed password
        *   @return <i>string</i> empty if the username exists and the password is correct, else error to report
        **/
        std::string CheckLogin(mg_connection* pConnection, const std::string& sUsername, const std::string& sPassword);



        /** @brief Sends dynamic data in response to javascript ajax requests
        *   @param sUser the username that corresponds to the connection
        *   @param vEndpoint the rest of the endpoint that the connection is targetting
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void DoAjax(const std::string& sUser, std::vector<std::string>& vEndpoint, mg_connection* pConnection, http_message* pMessage);

        /** @brief Sends dynamic data in response to javascript ajax requests
        *   @param sUser the username that corresponds to the connection
        *   @param vEndpoint the rest of the endpoint that the connection is targetting
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void DoAjaxUsers(const std::string& sUser, std::vector<std::string>& vEndpoint, mg_connection* pConnection, http_message* pMessage);

        /** @brief Sends dynamic data in response to javascript ajax requests
        *   @param sUser the username that corresponds to the connection
        *   @param vEndpoint the rest of the endpoint that the connection is targetting
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void DoAjaxNs(const std::string& sUser, std::vector<std::string>& vEndpoint, mg_connection* pConnection, http_message* pMessage);

        /** @brief Sends dynamic data in response to javascript ajax requests
        *   @param vEndpoint the rest of the endpoint that the connection is targetting
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void DoAjaxLogs(std::vector<std::string>& vEndpoint, mg_connection* pConnection, http_message* pMessage);

        std::string CreateLogs(std::vector<std::string>& vEndpoint);

        void SendLogs(const std::string& sLog, mg_connection* pConnection);

        void DownloadLogs(const std::vector<std::string>& vEndpoint, const std::string& sLog, mg_connection* pConnection);

        /** @brief Send a Json array containing all the user data to the provided connection
        *   @param sUser the username that corresponds to the connection
        *   @param pConnection the mg_connection to send the data to
        **/
        void SendUsers(const std::string& sUser, mg_connection* pConnection);


        /** @brief Attempt to create a user whose details are given in pMessage
        *   @param sUser the username that corresponds to the connection
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void CreateUser(const std::string& sUser, mg_connection* pConnection, http_message* pMessage);

        /** @brief Attempt to delete a user whose details are given in pMessage
        *   @param sUser the username that corresponds to the connection
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void DeleteUser(const std::string& sUser, mg_connection* pConnection, http_message* pMessage);



        /** @brief Attempt to change a user's password
        *   @param sUser the username that corresponds to the connection
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void ChangePassword(const std::string& sUser, mg_connection* pConnection, http_message* pMessage);


        /** @brief Send a JSON encoded error message to the provided connection containing the provided error
        *   @param pConnection the mg_connection to send the data to
        *   @param sError the error message
	*   @param nVersion the version of the file, if any, that caused the error
        **/
        void SendError(mg_connection* pConnection, const std::string& sError, int nVersion=-1);


        /** @brief Handle the browser sendin us an audio file
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void HandleFileUpload(mg_connection* pConnection, http_message* pMessage);

        /** @brief Called when a network source file us started to be uploaded. Updates version numbers and status of the ns ini file so no other files can be uploaded as weel

        **/
        void StartUpload(http_message* pMessage);

        /** @brief Called when the network source file has finished uploading. Copies the file to the working directory and executes the network source compiler
        **/
        void EndUpload(mg_connection *pConnection);

        void SendQueue();


        std::string GetFilteredLog(time_t nTimeFrom, time_t nTimeTo, const std::string& sPath, bool bTab, const std::vector<std::string>& vFilter, bool (*filter)(const std::string& sLine, const std::vector<std::string>& vFilter));

        mg_connection* m_pConnection;
        std::string m_sIniPath;
        std::string m_sNsFilePath;
        std::string m_sNsBinPath;
        std::string m_sLogPath;
        std::string m_sVsePath;
        std::string m_sServerName;

        mg_mgr m_mgr;

        mg_serve_http_opts m_ServerOpts;

        std::mutex m_mutex;
        std::map<std::string, std::string> m_mData;


        //std::set<std::string> m_setAllowed;
        std::set<mg_connection*> m_setAllowed;


        bool m_bUploadAllowed;

        std::queue<std::pair<std::string,std::string> > m_queueWS;


		std::map<std::string, unsigned long int> m_mMaxLogLength;
};
