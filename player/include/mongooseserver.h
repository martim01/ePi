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
#include <functional>
#include "namedtype.h"

using method = NamedType<std::string, struct MethodParameter>;
using url = NamedType<std::string, struct UrlParameter>;
using endpoint = std::pair<method, url>;

extern bool operator<(const endpoint e1, const endpoint& e2);


class MongooseServer
{
    public:

        MongooseServer(const iniManager& iniConfig);

        ///< @brief Creates the thread that runs the webserver loop
        void Run();


        /** Handles an event
        *   @param pConnection the mg_connection that caused the event
        *   @param nEvent the event type
        *   @param pData any associated data
        **/
        void HandleEvent(mg_connection *pConnection, int nEvent, void* pData);

        /** Adds a callback handler for an endpoint
        *   @param theEndpoint a pair definining the HTTP method and endpoint address
        *   @param func std::function that defines the callback function
        *   @return <i>bool</i> true on success
        **/
        bool AddEndpoint(const endpoint& theEndpoint, std::function<response(mg_connection*, http_message*, const std::string&)> func);

        /** Removes a callback handler for an endpoint
        *   @param theEndpoint a pair definining the HTTP method and endpoint address
        *   @return <i>bool</i> true on success
        **/
        bool DeleteEndpoint(const endpoint& theEndpoint);

        static const method GET;
        static const method POST;
        static const method PUT;
        static const method PATCH;
        static const method DELETE;
        static const method OPTIONS;

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


        bool UploadAllowed(mg_connection* pConnection, http_message* pMessage);
        /** @brief Handle the browser sending a multiparf ile
        *   @param pConnection the mg_connection to send the data to
        *   @param pMessage the http_message that we've been sent
        **/
        void HandleFileUpload(mg_connection* pConnection, http_message* pMessage);


        /** @brief Called when a file has finished uploading.
        **/
        void EndUpload(mg_connection *pConnection, void* pData);


        void DoReply(mg_connection* pConnection, const response& theResponse);

        void SendOptions(mg_connection* pConnection, const std::string& sUrl);


        mg_connection* m_pConnection;
        std::string m_sIniPath;
        std::string m_sServerName;

        mg_mgr m_mgr;

        mg_serve_http_opts m_ServerOpts;

        std::map<endpoint, std::function<response(mg_connection*, http_message*, const std::string&)>> m_mEndpoints;
        std::multimap<std::string, method> m_mmOptions;
};
