#include "mongoosethread.h"
#include <iostream>
#include <thread>
#include <initializer_list>
#include "json/json.h"
#include "sha256.h"
#include <sstream>
#include "inisection.h"
#include <fstream>
#include <chrono>
#include <iomanip>
#include <map>
#include "log.h"
#include <chrono>


using namespace std;
using namespace pml;

const string MongooseThread::SESSION_COOKIE_NAME = "ePi Player";

static mg_serve_http_opts s_ServerOpts;


string GetNowAsIso()
{
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

map<string, string> DecodeQueryString(http_message* pMessage)
{
    map<string, string> mDecode;

    vector<string> vQuery;
    SplitString(vQuery, pMessage->query_string.p, '&');
    for(size_t i = 0; i < vQuery.size(); i++)
    {
        vector<string> vValue;
        SplitString(vValue, vQuery[i], '=');
        if(vValue.size() == 2)
        {
            mDecode.insert(make_pair(vValue[0], vValue[1]));
        }
        else if(vValue.size() == 1)
        {
            mDecode.insert(make_pair(vValue[0], ""));
        }
    }
    return mDecode;
}


static int is_websocket(const struct mg_connection *nc)
{
    return nc->flags & MG_F_IS_WEBSOCKET;
}



static void ev_handler(mg_connection *pConnection, int nEvent, void* pData)
{
    if(nEvent == 0)
    {
        return;
    }

    MongooseThread* pThread = reinterpret_cast<MongooseThread*>(pConnection->user_data);
    pThread->HandleEvent(pConnection, nEvent, pData);
}

static mg_str file_upload(mg_connection*, mg_str file_name)
{
    Log::Get(Log::LOG_DEBUG) << "upload" << endl;
    char* filename= reinterpret_cast<char*>(malloc(strlen("/tmp/audio.wav")+1));
    strcpy(filename, "/tmp/audio.wav");
    return mg_mk_str(filename);
}




void MongooseThread::EventWebsocket(mg_connection *pConnection, int nEvent, void* pData)
{

    http_message* pMessage = reinterpret_cast<http_message*>(pData);

    Log::Get(Log::LOG_INFO) << "MG_EV_WEBSOCKET_HANDSHAKE_DONE" << endl;

    string sUri;
    sUri.assign(pMessage->uri.p, pMessage->uri.len);

    SendAll(pConnection);

}

void MongooseThread::EventHttp(mg_connection *pConnection, int nEvent, void* pData)
{

    //lock_guard<mutex> lg(m_mutex);
    http_message* pMessage = reinterpret_cast<http_message*>(pData);
    string sMethd(pMessage->method.p);
    string sUri;
    sUri.assign(pMessage->uri.p, pMessage->uri.len);

    /** Endpoints
    POST
        PLAY/FILE ? id=
        PLAY/FOLDER ? foldername=
        PLAY/SCHEDULE ? id=

        UPLOAD/FILE ?folder= & file=
        UPLOAD/SOFTWARE
        UPLOAD/SCHEDULE

        DELETE/FILE ? fileid=
        DELETE/FOLDER ? folder=
        DELETE/SCHEDULE ? id=
        MOVE/FILE ? fileid= & folder=
        COMMAND/POWER ?option= shutdown or reboot
    GET
        SCHEDULES
        SCHEDULE/id
        FOLDERS
        FOLDER/id
        FILES
        FILE/id
        VERSION
    **/
    vector<string> vSplit;
    SplitString(vSplit, sUri, '/');
    if(CmpNoCase(vSplit[0], "PLAY"))
    {
    }
    else if(CmpNoCase(vSplit[0], "UPLOAD"))
    {
    }
    else if(CmpNoCase(vSplit[0], "COMMAND"))
    {
    }
    else if(CmpNoCase(vSplit[0], "DELETE"))
    {
    }
    else if(CmpNoCase(vSplit[0], "MOVE"))
    {
    }
    else if(CmpNoCase(vSplit[0], "SCHEDULES"))
    {
    }
    else if(CmpNoCase(vSplit[0], "SCHEDULE"))
    {
    }
    else if(CmpNoCase(vSplit[0], "FOLDERS"))
    {
    }
    else if(CmpNoCase(vSplit[0], "FOLDER"))
    {
    }
    else if(CmpNoCase(vSplit[0], "FILES"))
    {
    }
    else if(CmpNoCase(vSplit[0], "FILE"))
    {
    }
    else if(CmpNoCase(vSplit[0], "VERSION"))
    {
    }
    else
    {
        //send 404
    }
}

void MongooseThread::HandleEvent(mg_connection *pConnection, int nEvent, void* pData)
{
    //lock_guard<recursive_mutex> lg(m_mutex);
    if(nEvent != 4 && nEvent != 6)
        Log::Get(Log::LOG_DEBUG) << "HandleEvent: " << nEvent << endl;
    switch (nEvent)
    {
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    {
        Log::Get(Log::LOG_DEBUG) << "Event Websocket" << endl;
        //lock_guard<recursive_mutex> lg(m_mutex);
        EventWebsocket(pConnection, nEvent, pData);
        Log::Get(Log::LOG_DEBUG) << "Done" << endl;
    }
    break;
    case MG_EV_HTTP_REQUEST:
    {
        Log::Get(Log::LOG_DEBUG) << "Event HTTP" << endl;
        //lock_guard<recursive_mutex> lg(m_mutex);
        EventHttp(pConnection, nEvent, pData);
        Log::Get(Log::LOG_DEBUG) << "Done" << endl;
    }
    break;
    case MG_EV_TIMER:
        /* Perform session maintenance. */
    {
        //lock_guard<recursive_mutex> lg(m_mutex);
        //CheckSessions();
        mg_set_timer(pConnection, mg_time() + SESSION_CHECK_INTERVAL);
    }
    break;
    case MG_EV_CLOSE:
    {
        Log::Get(Log::LOG_DEBUG) << "Event Close" << endl;
        //lock_guard<recursive_mutex> lg(m_mutex);
        if (is_websocket(pConnection))
        {
            pConnection->user_data = 0;
        }
        Log::Get(Log::LOG_DEBUG) << "Done" << endl;
    }
    break;
    case MG_EV_HTTP_MULTIPART_REQUEST:
        StartUpload(reinterpret_cast<http_message*>(pData));
        break;
    case MG_EV_HTTP_PART_BEGIN:
        Log::Get(Log::LOG_DEBUG) << "MG_EV_HTTP_PART_BEGIN" << endl;
        mg_file_upload_handler(pConnection, nEvent, pData, file_upload);
        break;
    case MG_EV_HTTP_PART_DATA:
        Log::Get(Log::LOG_DEBUG) << "MG_EV_HTTP_PART_DATA" << endl;
        mg_file_upload_handler(pConnection, nEvent, pData, file_upload);
        break;
    case MG_EV_HTTP_PART_END:
        Log::Get(Log::LOG_DEBUG) << "MG_EV_HTTP_PART_END" << endl;
        mg_file_upload_handler(pConnection, nEvent, pData, file_upload);
        EndUpload(pConnection);
    case 0:
        break;
    }
}


MongooseThread::MongooseThread() : m_pConnection(0)
{

}

MongooseThread& MongooseThread::Get()
{
    static MongooseThread mong;
    return mong;
}

void MongooseThread::Init(iniManager& iniConfig)
{

    m_sIniPath = CreatePath(iniConfig.GetIniString("paths", "inifiles","."));

    //check for ssl
    string sCert = iniConfig.GetIniString("ssl", "cert", "");
    string sKey = iniConfig.GetIniString("ssl", "key", "");


    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    stringstream ssRewrite;
    ssRewrite << "%80=https://" << hostname;

    s_ServerOpts.document_root = iniConfig.GetIniString("paths", "webpages","html").c_str();
    s_ServerOpts.enable_directory_listing = "yes";
    s_ServerOpts.url_rewrites=ssRewrite.str().c_str();
    s_ServerOpts.extra_headers="X-Frame-Options: sameorigin\r\nCache-Control: no-cache\r\nStrict-Transport-Security: max-age=31536000; includeSubDomains\r\nX-Content-Type-Options: nosniff\r\nReferrer-Policy: no-referrer\r\nServer: unknown";

    mg_mgr_init(&m_mgr, NULL);

    stringstream ss;
    ss << iniConfig.GetIniInt("webserver", "port", 8080);

    if(!sCert.empty() && !sKey.empty())
    {
        const char* err;
        struct mg_bind_opts bind_opts;
        memset(&bind_opts, 0, sizeof(bind_opts));
        bind_opts.ssl_cert = sCert.c_str();
        bind_opts.ssl_key = sKey.c_str();
        bind_opts.error_string = &err;
        bind_opts.ssl_cipher_suites = "ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256";


        m_pConnection = mg_bind_opt(&m_mgr, ss.str().c_str(), ev_handler, bind_opts);
        if(m_pConnection == NULL)
        {
            Log::Get(Log::LOG_ERROR) << err << endl;
        }
    }
    else
    {
        m_pConnection = mg_bind(&m_mgr, ss.str().c_str(), ev_handler);
    }



    if(m_pConnection)
    {
        m_pConnection->user_data = reinterpret_cast<void*>(this);

        //mg_register_http_endpoint(m_pConnection, "/", login_handler);
        //mg_register_http_endpoint(m_pConnection, "/logout", logout_handler);
        mg_set_timer(m_pConnection, mg_time() + SESSION_CHECK_INTERVAL);

        Log::Get(Log::LOG_INFO) << m_pConnection->user_data << endl;
        Log::Get(Log::LOG_INFO) << s_ServerOpts.document_root << endl;
        Log::Get(Log::LOG_INFO) << "--------------------------" << endl;
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << "Could not start webserver" << endl;
    }
}

void MongooseThread::Run()
{
    if(m_pConnection)
    {
        thread th(&MongooseThread::Loop, this);
        th.detach();
    }
}

void MongooseThread::Loop()
{
    if(m_pConnection)
    {
        mg_set_protocol_http_websocket(m_pConnection);
        while (true)
        {
            mg_mgr_poll(&m_mgr, 50);
            SendQueue();
        }
        mg_mgr_free(&m_mgr);
    }
}

void MongooseThread::SendWSMessage(const string& sId, string sMessage)
{
    lock_guard<mutex> lg(m_mutex);
    m_queueWS.push(make_pair(sId.substr(0,1), sMessage));
}

void MongooseThread::SendQueue()
{
    lock_guard<mutex> lg(m_mutex);
    if(m_pConnection)
    {
        while(m_queueWS.empty() == false)
        {
            //turn message into array
            string sMessage = "["+m_queueWS.front().second+"]";
            char *cstr = new char[sMessage.length() + 1];
            strcpy(cstr, sMessage.c_str());


            for (mg_connection* pConnection = mg_next(m_pConnection->mgr, NULL); pConnection != NULL; pConnection = mg_next(m_pConnection->mgr, pConnection))
            {
                if(is_websocket(pConnection))
                {
                    size_t nSlot = reinterpret_cast<size_t>(pConnection->user_data);
                    map<size_t, wssession>::iterator itws = m_mWsSession.find(nSlot);
                    if(itws != m_mWsSession.end())
                    {
                        if(m_mSessions.find(itws->second.nId) != m_mSessions.end())
                        {
                            map<int, interest>::const_iterator itInterest = m_mPage.find(itws->second.nPage);
                            if(itInterest != m_mPage.end() && itInterest->second.setPipe.find(m_queueWS.front().first) != itInterest->second.setPipe.end())
                            {
                                mg_send_websocket_frame(pConnection, WEBSOCKET_OP_TEXT, cstr, strlen(cstr));
                            }
                        }
                        else
                        {
                            SendLogOut(pConnection);
                        }
                    }
                }
            }
            delete[] cstr;
            m_queueWS.pop();
        }
    }
}

void MongooseThread::SendAll(mg_connection* pConnection)
{
    //lock_guard<mutex> lg(m_mutex);

    map<size_t, wssession>::iterator itws = m_mWsSession.find(reinterpret_cast<size_t>(pConnection->user_data));
    if(itws != m_mWsSession.end() && m_mSessions.find(itws->second.nId) != m_mSessions.end())
    {
        map<int, interest>::const_iterator itInterest = m_mPage.find(itws->second.nPage);
        if(itInterest != m_mPage.end())
        {
            string sOut;

            for(map<string, string>::const_iterator itData = m_mData.begin(); itData != m_mData.end(); ++itData)
            {
                if(itInterest->second.setPipe.find(itData->first.substr(0,1)) != itInterest->second.setPipe.end())
                {
                    if(sOut.empty() == false)
                    {
                        sOut += ",";
                    }
                    sOut += itData->second;
                }
            }
            sOut = "["+sOut+"]";
		Log::Get(Log::LOG_DEBUG) << sOut << endl;
            mg_send_websocket_frame(pConnection, WEBSOCKET_OP_TEXT, sOut.c_str(), sOut.length());
        }
    }
}

void MongooseThread::SendLogOut(mg_connection* pConnection)
{
    Json::Value jsValue;
    jsValue["action"] = "logout";
    Json::FastWriter ftw;
    string sOut(ftw.write(jsValue));
    mg_send_websocket_frame(pConnection, WEBSOCKET_OP_TEXT, sOut.c_str(), sOut.length());
}

string MongooseThread::CheckLogin(mg_connection* pConnection, const string& sUsername, const string& sPassword)
{
    //check number of attempts from this account
    char addr[32];
    mg_sock_addr_to_str(&pConnection->sa, addr, sizeof(addr),MG_SOCK_STRINGIFY_IP);

    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    map<login, pair<unsigned int, time_t> >::iterator itLogin = m_mLoginAttempts.insert(make_pair(login(addr, sUsername), make_pair(0, in_time_t))).first;

    //find out when last attempted to login

    if(in_time_t - itLogin->second.second > m_nLoginLockTime)
    {   //over half an hour ago. Reset the attempts to 0  - this will unlock any locked accounts
        itLogin->second.first = 0;
    }
    itLogin->second.second = in_time_t;

    itLogin->second.first++;

    if(itLogin->second.first > m_nMaximumLoginAttempts)
    {
        return "Too many failed attempts. Account login locked!";
    }

    SHA256 sha256;
    Log::Get(Log::LOG_INFO) << "CheckLogin=" << sha256(sPassword) << endl;

    if(m_iniUsers.GetIniString(sUsername, "password", "") == sha256(sPassword))
    {
        itLogin->second.first = 0;    //success so reset attempts to zero
        return "";
    }
    return "User does not exist or password incorrect!";
}

void MongooseThread::LoadUsers()
{
    if(m_iniUsers.ReadIniFile(m_sIniPath+"users.ini") == true)
    {
        Log::Get(Log::LOG_INFO) << "users.ini loaded" << endl;
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << "failed to load users.ini" << endl;
        SHA256 sha256;
        m_iniUsers.SetSectionValue("admin", "real_name", "Default Admin");
        m_iniUsers.SetSectionValue("admin", "account", "Admin");
        m_iniUsers.SetSectionValue("admin", "password", sha256("password"));
    }
}


size_t MongooseThread::SaveWsSession(uint64_t nId, int nPage)
{
    size_t nCount(0);
    bool bAdded = false;
    do
    {
        ++nCount;
        bAdded = m_mWsSession.insert(make_pair(nCount, wssession(nId, nPage))).second;

    }
    while(bAdded == false);
    Log::Get(Log::LOG_INFO) << "Session :" << nId << " saved in slot " << nCount << endl;
    return nCount;
}


void MongooseThread::DoAjax(const string& sUser, vector<string>& vEndpoint, mg_connection* pConnection, http_message* pMessage)
{
    if(vEndpoint[0] == "users" && vEndpoint.size() > 1)
    {
        vEndpoint.erase(vEndpoint.begin());
        DoAjaxUsers(sUser, vEndpoint, pConnection, pMessage);
    }
    else if(vEndpoint[0] == "ns"  && vEndpoint.size() > 1)
    {
        vEndpoint.erase(vEndpoint.begin());
        DoAjaxNs(sUser, vEndpoint, pConnection, pMessage);
    }
    else if(vEndpoint[0] == "logs" && vEndpoint.size() > 1)
    {
        vEndpoint.erase(vEndpoint.begin());
        DoAjaxLogs(vEndpoint, pConnection, pMessage);
    }
    else if(vEndpoint[0] == "service")
    {
        DoAjaxService(sUser, pConnection, pMessage);
    }
    else if(vEndpoint[0] == "concentrator")
    {
        DoAjaxConcentrator(sUser, pConnection, pMessage);
    }
}

void MongooseThread::DoAjaxUsers(const string& sUser, vector<string>& vEndpoint, mg_connection* pConnection, http_message* pMessage)
{
    if(vEndpoint[0] == "load")
    {
        SendUsers(sUser, pConnection);
    }
    else if(vEndpoint[0] == "create" || vEndpoint[0] == "edit")
    {
        if (mg_vcmp(&pMessage->method, "POST") == 0)
        {
            CreateUser(sUser, pConnection, pMessage);
        }
        else
        {
            SendError(pConnection, "Method not allowed");
        }
    }
    else if(vEndpoint[0] == "delete")
    {
        if (mg_vcmp(&pMessage->method, "POST") == 0)
        {
            DeleteUser(sUser, pConnection, pMessage);
        }
        else
        {
            SendError(pConnection, "Method not allowed");
        }
    }
    else if(vEndpoint[0] == "change_password")
    {
        if (mg_vcmp(&pMessage->method, "POST") == 0)
        {
            ChangePassword(sUser, pConnection, pMessage);
        }
        else
        {
            SendError(pConnection, "Method not allowed");
        }
    }

}

void MongooseThread::DoAjaxNs(const string& sUser, vector<string>& vEndpoint, mg_connection* pConnection, http_message* pMessage)
{
    Log::Get(Log::LOG_DEBUG) << "DoAjaxNs" << endl;
    if(vEndpoint[0] == "load")
    {
        bool bAdmin = (m_iniUsers.GetIniString(sUser, "account", "User")=="Admin");
        SendNetworkSourceInfo(bAdmin, pConnection);
    }
    else if(vEndpoint[0] == "make_live" && vEndpoint.size() > 1)
    {
        MakeLive(sUser, vEndpoint[1], pConnection, pMessage);
    }
    else if(vEndpoint[0] == "delete" && vEndpoint.size() > 1)
    {
        DeleteNetworkSource(sUser, vEndpoint[1], pConnection, pMessage);
    }
    else
    {
        SendError(pConnection, "Endpoint not found");
    }
}

void MongooseThread::DoAjaxLogs(vector<string>& vEndpoint, mg_connection* pConnection, http_message* pMessage)
{
    Log::Get(Log::LOG_DEBUG) << "DoAjaxLogs" << endl;
    if(vEndpoint[0] == "view" && vEndpoint.size() > 4)
    {
        vEndpoint.erase(vEndpoint.begin());
        SendLogs(CreateLogs(vEndpoint), pConnection);
    }
    else if(vEndpoint[0] == "download" && vEndpoint.size() > 4)
    {
        vEndpoint.erase(vEndpoint.begin());
        DownloadLogs(vEndpoint, CreateLogs(vEndpoint), pConnection);
    }
    else
    {
        SendError(pConnection, "Endpoint not found");
    }
}

string MongooseThread::CreateLogs(vector<string>& vEndpoint)
{
    string sLog;
    try
    {
        unsigned long nTimeFrom(stol(vEndpoint[1]));
        unsigned long nTimeTo(stol(vEndpoint[2]));

	//make sure the times asked for are not too long...
	if(nTimeFrom > nTimeTo)
	{
		sLog =  "Start Time is later than End Time!";
	}
	else if(nTimeTo - nTimeFrom > m_mMaxLogLength[vEndpoint[0]])
	{
		sLog = "Filter timespan is too long!. Must be less than one day.";
	}
	else if(vEndpoint[0] == "rapid")
	{
		vector<string> vFilter;
		SplitString(vFilter, vEndpoint[3], '-');

		sLog = GetFilteredLog(nTimeFrom, nTimeTo, vEndpoint[0], true, vFilter, RapidFilter);

	}
	else if(vEndpoint[0] == "reports")
	{
		vector<string> vFilter;
		SplitString(vFilter, vEndpoint[3], '-');

		sLog = GetFilteredLog(nTimeFrom, nTimeTo, vEndpoint[0], false, vFilter, ReportFilter);

	}
	else if(vEndpoint[0] == "raw" && vEndpoint.size()>5)
	{
		stringstream ss;
		ss << "raw/" << vEndpoint[3] << "/" << vEndpoint[4] << "/";
		vector<string> vFilter;
		SplitString(vFilter, vEndpoint[5], '-');

		sLog = GetFilteredLog(nTimeFrom, nTimeTo, ss.str(), true, vFilter, RawFilter);

	}
	else if(vEndpoint[0] == "event" && vEndpoint.size() > 4)
	{
		stringstream ss;
		ss << "events/" << vEndpoint[3] << "/";
		vector<string> vFilter;


		sLog = GetFilteredLog(nTimeFrom, nTimeTo, ss.str(), true, vFilter, EventFilter);
	}
	else if(vEndpoint[0] == "concentrator")
	{
	    vector<string> vFilter;
            SplitString(vFilter, vEndpoint[3], '-');

            sLog = GetFilteredLog(nTimeFrom, nTimeTo, vEndpoint[0], true, vFilter, ConcentratorFilter);
	}
        else
        {
            return "Endpoint not found";
        }

    }
    catch(invalid_argument& e)
    {
        return "Endpoint incorrect";
    }
    if(sLog.empty())
    {
        sLog = "No log entries found that match the filter!";
    }
    return sLog;
}

void MongooseThread::SendLogs(const std::string& sLog, mg_connection* pConnection)
{
    mg_printf(pConnection, "%s", "HTTP/1.1 200 OK\r\nCache: no-cache\r\nContent-Type: application/text\r\n\r\n");
    mg_printf(pConnection, sLog.c_str());
    pConnection->flags |= MG_F_SEND_AND_CLOSE;
}

void MongooseThread::DownloadLogs(const vector<string>& vEndpoint, const std::string& sLog, mg_connection* pConnection)
{
    stringstream ssFile;
    for(size_t i=0; i < vEndpoint.size(); i++)
    {
        if(i != 0)
        {
            ssFile << "_";
        }
        ssFile << vEndpoint[i];
    }
    ssFile << ".log";

    stringstream ssContent;
    ssContent << "Content-Disposition: attachment; filename=\"" << ssFile.str() << "\"";

    mg_send_head(pConnection, 200, sLog.length(), ssContent.str().c_str());
    mg_printf(pConnection, sLog.c_str());
    pConnection->flags |= MG_F_SEND_AND_CLOSE;
}


string MongooseThread::GetFilteredLog(time_t nTimeFrom, time_t nTimeTo, const string& sPath, bool bTab, const vector<string>& vFilter, bool (*filter)(const string& sLine, const vector<string>& vFilter))
{
    stringstream ssLog;
    //get an array of the logs we need to open
    for(time_t nTime = nTimeFrom; (nTime/3600) <= (nTimeTo/3600); nTime+=3600)
    {
        stringstream ssPath;
        ssPath << m_sLogPath << sPath << "/" << put_time(localtime(&nTime), "%Y/%m/%d/%H") << ".log";


        ifstream inFile;
        inFile.open(ssPath.str(),ios::in);
        if(!inFile.is_open())
        {
            ssLog << put_time(localtime(&nTime), "%Y-%m-%d") << "\t" << put_time(localtime(&nTime), "%H:00");
            ssLog <<  "!!!!! NO LOG FILE FOUND FOR THIS HOUR !!!!! " << endl;
        }
        else
        {
	    Log::Get(Log::LOG_DEBUG) << ssPath.str() << " opened" << endl;
            inFile.clear();
            string sLine;
            while(!inFile.eof())
            {
                getline(inFile,sLine,'\n');
                if(sLine.empty() == false)
                {
                    time_t timeMessage(0);
                    std::tm atm= {};
                    atm.tm_isdst = -1;
                    vector<string> vSplit;

                    if(bTab)
                    {
                        SplitString(vSplit, sLine, '\t', 3);
                        if(vSplit.size() > 2)
                        {
                            // @TODO what to about MS??
			    vector<string> vTime;
			    SplitString(vTime, vSplit[1], ':');
			    if(vTime.size() > 2)
                            {
				istringstream ss(vSplit[0]+" "+vTime[0]+":"+vTime[1]+":"+vTime[2]);
	                        ss >> get_time(&atm, "%d/%m/%y %H:%M:%S");
        	                if(ss.fail())
                	        {
  	                      	    	Log::Get(Log::LOG_DEBUG) << "failed to parse date time: " << vSplit[0] << " " << vSplit[1] << endl;
	                        }
        	                else
                	        {
					if(atm.tm_year < 100)
					{
						atm.tm_year += 100;
					}
					Log::Get(Log::LOG_DEBUG) << atm.tm_year << "-" << (atm.tm_mon+1) <<"-"<<atm.tm_mday << " " <<atm.tm_hour <<":" <<atm.tm_min <<":"<<atm.tm_sec<< endl;
                                	timeMessage = mktime(&atm);
                            	}
			    }
                        }
                    }
                    else
                    {
                        SplitString(vSplit, sLine, ' ',3);
                        if(vSplit.size() > 2)
                        {
                            stringstream ssDate;
                            ssDate << put_time(localtime(&nTime), "%Y-%m-%d");
                            istringstream ss(ssDate.str()+" "+vSplit[1]);
                            ss >> get_time(&atm, "%Y-%m-%d %H:%M:%S");
                            if(ss.fail())
                            {
                                Log::Get(Log::LOG_DEBUG) << "failed to parse date time " << endl;
                            }
                            else
                            {
                                timeMessage = mktime(&atm);
                            }
                        }
                    }
                    if(timeMessage >= nTimeFrom && timeMessage <= nTimeTo)
                    {
                        if(filter(vSplit[2], vFilter))
                        {
                            ssLog << put_time(localtime(&nTime), "%Y-%m-%d") << "\t";
                            ssLog << vSplit[1] << "\t" << vSplit[2] << endl;
                        }
                    }
		    else if(vSplit.size() > 1)
 		   {
			Log::Get(Log::LOG_DEBUG) << vSplit[0] << " " << vSplit[1] << "=" << timeMessage << " " << nTimeFrom << "-" << nTimeTo << endl;
		   }
                }
            }
            inFile.close();
        }
    }
    return ssLog.str();
}



void MongooseThread::SendUsers(const string& sUser, mg_connection* pConnection)
{
    Json::Value jsArray = Json::arrayValue;
    for(map<string,iniSection*>::iterator itSection = m_iniUsers.GetSectionBegin(); itSection != m_iniUsers.GetSectionEnd(); ++itSection)
    {
        Json::Value jsValue;
        jsValue["username"] = itSection->first;
        jsValue["real_name"] = itSection->second->GetString("real_name", "");
//        jsValue["password"] = itSection->second->GetString("password", "");
        jsValue["account"] = itSection->second->GetString("account", "");

        if(itSection->first == sUser)
        {
            jsValue["active"] = true;
        }
        else
        {
            jsValue["active"] = false;
        }

        jsArray.append(jsValue);
    }
    Json::FastWriter ftw;
    mg_printf(pConnection, "%s", "HTTP/1.1 200 OK\r\nCache: no-cache\r\nContent-Type: application/x-javascript\r\n\r\n");
    mg_printf(pConnection, ftw.write(jsArray).c_str());
    pConnection->flags |= MG_F_SEND_AND_CLOSE;
}

void MongooseThread::SendError(mg_connection* pConnection, const string& sError, int nVersion)
{
    Json::Value jsValue;
    jsValue["error"] = sError;
    if(nVersion >= 0)
    {
        jsValue["version"] = nVersion;
    }
    Json::FastWriter ftw;
    mg_printf(pConnection, "%s", "HTTP/1.1 200 OK\r\nCache: no-cache\r\nContent-Type: application/x-javascript\r\n\r\n");
    mg_printf(pConnection, ftw.write(jsValue).c_str());
    pConnection->flags |= MG_F_SEND_AND_CLOSE;
}

void MongooseThread::SendServerName(mg_connection* pConnection, http_message* pMessage)
{
    Json::Value jsValue;
    jsValue["server"] = m_sServerName;

    Json::FastWriter ftw;
    mg_printf(pConnection, "%s", "HTTP/1.1 200 OK\r\nCache: no-cache\r\nContent-Type: application/x-javascript\r\n\r\n");
    mg_printf(pConnection, ftw.write(jsValue).c_str());
    pConnection->flags |= MG_F_SEND_AND_CLOSE;
}

void MongooseThread::CreateUser(const string& sUser, mg_connection* pConnection, http_message* pMessage)
{
    //check the user is actually an admin
    if(m_iniUsers.GetIniString(sUser, "account", "User") == "Admin")
    {
        Log::Get(Log::LOG_INFO) << "Create user" << endl;
        char user[50], pass[50], account[20], real_name[255];
        int ul = mg_get_http_var(&pMessage->body, "username", user, sizeof(user));
        int pl = mg_get_http_var(&pMessage->body, "password", pass, sizeof(pass));
        int al = mg_get_http_var(&pMessage->body, "account", account, sizeof(account));
        int rl = mg_get_http_var(&pMessage->body, "real_name", real_name, sizeof(real_name));

        if(ul > 0 && al > 0 && rl > 0)
        {
            Log::Get(Log::LOG_INFO) << "Username=" << user << endl;
            Log::Get(Log::LOG_INFO) << "real_name=" << real_name << endl;
            Log::Get(Log::LOG_INFO) << "password=" << pass << endl;
            Log::Get(Log::LOG_INFO) << "account=" << account << endl;

            Log::Get(Log::LOG_MSG) << sUser << " has created a new " << account << " user: " << real_name << " with username= " << user << endl;

            //save the user
            iniSection* pSection = m_iniUsers.CreateSection(user);
            if(pSection)
            {
                SHA256 sha256;
                pSection->SetValue("real_name", real_name);
                pSection->SetValue("account", account);
		if(pl > 0)
		{
	                pSection->SetValue("password", sha256(pass));
		}
            }
            m_iniUsers.WriteIniFile(m_sIniPath+"users.ini");
            SendUsers(sUser, pConnection);
        }
        else
        {
            Log::Get(Log::LOG_ERROR) << sUser << " attempted to create a new user but sent malformed data." << endl;
            SendError(pConnection, "POST malformed");
        }
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << sUser << " attempted to create a new user but has no admin rights." << endl;
        SendError(pConnection, "User has no admin rights.");
    }
}

void MongooseThread::DeleteUser(const string& sUser, mg_connection* pConnection, http_message* pMessage)
{
    //check the user is actually an admin
    if(m_iniUsers.GetIniString(sUser, "account", "User") == "Admin")
    {
        Log::Get(Log::LOG_INFO) << "Delete user" << endl;
        char user[50];
        int ul = mg_get_http_var(&pMessage->body, "username", user, sizeof(user));

        if(ul > 0)
        {
            Log::Get(Log::LOG_INFO) << "Username=" << user << endl;
            if(m_iniUsers.DeleteSection(user))
            {
                Log::Get(Log::LOG_MSG) << sUser << " has delete the user with username= " << user << endl;
                m_iniUsers.WriteIniFile(m_sIniPath+"users.ini");
                SendUsers(sUser, pConnection);
            }
            else
            {
                Log::Get(Log::LOG_ERROR) << sUser << " attempted to delete a user but the file system failed." << endl;
                SendError(pConnection, "Could not delete user");
            }
        }
        else
        {
            Log::Get(Log::LOG_ERROR) << sUser << " attempted to delete a user but sent malformed data." << endl;
            SendError(pConnection, "POST malformed");
        }
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << sUser << " attempted to delete a user but has no admin rights." << endl;
        SendError(pConnection, "User has no admin rights.");
    }
}

void MongooseThread::ChangePassword(const string& sUser, mg_connection* pConnection, http_message* pMessage)
{
    //check the user is actually an admin
    Log::Get(Log::LOG_INFO) << "Change password" << endl;
    char user[50], pass[50];
    int ul = mg_get_http_var(&pMessage->body, "username", user, sizeof(user));
    int pl = mg_get_http_var(&pMessage->body, "password", pass, sizeof(pass));

    if(ul > 0 && pl > 0)
    {
        Log::Get(Log::LOG_INFO) << "Username=" << user << endl;
        Log::Get(Log::LOG_INFO) << "password=" << pass << endl;

        if(sUser == user || m_iniUsers.GetIniString(sUser, "account", "User") == "Admin")
        {
            Log::Get(Log::LOG_MSG) << sUser << " changed the password of the user with username= " << user << endl;
            //save the user
            SHA256 sha256;
            m_iniUsers.SetSectionValue(user, "password", sha256(pass));
            m_iniUsers.WriteIniFile(m_sIniPath+"users.ini");
            Json::Value jsValue;
            jsValue["result"] = "Password Changed";
            Json::FastWriter ftw;
            mg_printf(pConnection, "%s", "HTTP/1.1 200 OK\r\nCache: no-cache\r\nContent-Type: application/x-javascript\r\n\r\n");
            mg_printf(pConnection, ftw.write(jsValue).c_str());
            pConnection->flags |= MG_F_SEND_AND_CLOSE;
        }
        else
        {
            Log::Get(Log::LOG_ERROR) << sUser << " attempted to delete another user but has no admin rights." << endl;
            SendError(pConnection, "User has no admin rights and is not the given user.");

        }
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << sUser << " attempted to change the password of a user but sent malformed data." << endl;
        SendError(pConnection, "POST malformed");
    }
}

void MongooseThread::DoAjaxService(const string& sUser, mg_connection* pConnection, http_message* pMessage)
{
    //check the user is actually an admin
    if(m_iniUsers.GetIniString(sUser, "account", "User") == "Admin")
    {
        Log::Get(Log::LOG_INFO) << "Service Admin" << endl;
        char action[50];
        int nAction = mg_get_http_var(&pMessage->body, "action", action, sizeof(action));

        if(nAction > 0)
        {
            string sAction(action);
            if(sAction == "goactive" || sAction == "gopassive")
            {

                Log::Get(Log::LOG_MSG) << sUser << " performed action '" << action << "'" << endl;
                RdsDataManager::Get().QueueMessage(sAction);

                Json::Value jsValue;
                jsValue["result"] = sAction;
                Json::FastWriter ftw;
                mg_printf(pConnection, "%s", "HTTP/1.1 200 OK\r\nCache: no-cache\r\nContent-Type: application/x-javascript\r\n\r\n");
                mg_printf(pConnection, ftw.write(jsValue).c_str());
                pConnection->flags |= MG_F_SEND_AND_CLOSE;
            }
            else if(sAction == "start" || sAction == "stop")
            {
                Log::Get(Log::LOG_MSG) << sUser << "  change rdsexec state to: '" << action << "'" << endl;
                StartStopExec(sAction);
            }
            else
            {
                Log::Get(Log::LOG_ERROR) << sUser << " attempted an action that does not exist: " << sAction << endl;
                SendError(pConnection, sAction+" is not a valid action");

            }
        }
        else
        {
            Log::Get(Log::LOG_ERROR) << sUser << " attempted to perform a service action but sent malformed data." << endl;
            SendError(pConnection, "POST malformed");
        }
    }
}

void MongooseThread::DoAjaxConcentrator(const string& sUser, mg_connection* pConnection, http_message* pMessage)
{
    //check the user is actually an admin
    if(m_iniUsers.GetIniString(sUser, "account", "User") == "Admin")
    {
        Log::Get(Log::LOG_INFO) << "Concentrator Admin" << endl;
        char command[128];
        char service[50];
        char time[50];

        int nCommand = mg_get_http_var(&pMessage->body, "command", command, sizeof(command));
        int nService = mg_get_http_var(&pMessage->body, "service", service, sizeof(service));


        if(nCommand > 0 && nService > 0)
        {
            string sCommand(command);
            string sService(service);
            try
            {
                nService = stoi(sService);
                if(nService == -1 || (nService > 0 && nService < 256))
                {
                    stringstream ssWrite;
                    if(sCommand == "disconnect" || sCommand == "clearlastcommserror" || sCommand == "clearlastrx" || sCommand == "clearlasttx" || sCommand == "cleartimeout" || sCommand == "clearcrc" || sCommand == "clearsn")
                    {
                        ssWrite << sCommand << " " << nService;
                        RdsDataManager::Get().QueueMessage(ssWrite.str());
                        Log::Get(Log::LOG_MSG) << sUser << " performed concentrator action '" << ssWrite.str() << "'" << endl;

                        Json::Value jsValue;
                        jsValue["result"] = ssWrite.str();
                        Json::FastWriter ftw;
                        mg_printf(pConnection, "%s", "HTTP/1.1 200 OK\r\nCache: no-cache\r\nContent-Type: application/x-javascript\r\n\r\n");
                        mg_printf(pConnection, ftw.write(jsValue).c_str());
                        pConnection->flags |= MG_F_SEND_AND_CLOSE;
                    }
                    else if(sCommand == "holdoff")
                    {
                        int nTime = mg_get_http_var(&pMessage->body, "time", time, sizeof(time));
                        if(nTime > 0)
                        {
                            ssWrite << sCommand << " " << nService << " " << time;
                            RdsDataManager::Get().QueueMessage(ssWrite.str());
                            Log::Get(Log::LOG_MSG) << sUser << " performed concentrator action '" << ssWrite.str() << "'" << endl;

                            Json::Value jsValue;
                            jsValue["result"] = ssWrite.str();
                            Json::FastWriter ftw;
                            mg_printf(pConnection, "%s", "HTTP/1.1 200 OK\r\nCache: no-cache\r\nContent-Type: application/x-javascript\r\n\r\n");
                            mg_printf(pConnection, ftw.write(jsValue).c_str());
                            pConnection->flags |= MG_F_SEND_AND_CLOSE;
                        }
                        else
                        {
                            ssWrite << sCommand << " " << nService << ": timeout not sent!";
                            Log::Get(Log::LOG_MSG) << sUser << "error concentrator action '" << ssWrite.str() << "'" << endl;
                            SendError(pConnection, ssWrite.str());
                        }
                    }
                    else
                    {
                        SendError(pConnection, sCommand+" is not a valid action");
                        Log::Get(Log::LOG_MSG) << sUser << "error concentrator action '" << sCommand << "' not valid" << endl;
                    }

                }
                else
                {
                    SendError(pConnection, sService+" is not a valid service");
                    Log::Get(Log::LOG_MSG) << sUser << "error concentrator service '" << sService << "' not valid" << endl;
                }
            }
            catch(const invalid_argument& e)
            {
                SendError(pConnection, sService+" is not a valid service");
                Log::Get(Log::LOG_MSG) << sUser << "error concentrator service '" << sService << "' not valid" << endl;
            }
        }
        else
        {
            Log::Get(Log::LOG_ERROR) << sUser << " attempted to perform a concentrator action but sent malformed data." << endl;
            SendError(pConnection, "POST malformed");
        }
    }
}

void MongooseThread::StartStopExec(const std::string& sAction)
{

    std::stringstream ssCommand;
    ssCommand << "sudo systemctl " << sAction << " rdsexec.service";
    system(ssCommand.str().c_str());
    RdsDataManager::Get().CheckProcs();
}

void MongooseThread::SendNetworkSourceInfo(bool bAdmin, mg_connection* pConnection)
{
    Json::Value jsObject;
    Json::Value jsCurrent;
    Json::Value jsPending;
    Json::Value jsPrevious;


    if(m_iniNs.ReadIniFile(m_sIniPath+"networksource.ini") == true)
    {
        Log::Get(Log::LOG_INFO) << "++++ READ INI FILE +++++" << endl;
        jsCurrent["version"] = m_iniNs.GetIniInt("current", "version",0);
        jsCurrent["uploaded_date"] = m_iniNs.GetIniString("current", "uploaded_date","");
        jsCurrent["uploaded_by"] = m_iniNs.GetIniString("current", "uploaded_by","");
        jsCurrent["golive_date"] = m_iniNs.GetIniString("current", "golive_date","");
        jsCurrent["golive_by"] = m_iniNs.GetIniString("current", "golive_by","");

        jsPending["version"] = m_iniNs.GetIniInt("pending", "version",0);
        jsPending["uploaded_date"] = m_iniNs.GetIniString("pending", "uploaded_date","");
        jsPending["uploaded_by"] = m_iniNs.GetIniString("pending", "uploaded_by","");
        jsPending["status"] = m_iniNs.GetIniString("pending", "status","");

        jsPrevious["version"] = m_iniNs.GetIniInt("previous", "version",0);
        jsPrevious["uploaded_date"] = m_iniNs.GetIniString("previous", "uploaded_date","");
        jsPrevious["uploaded_by"] = m_iniNs.GetIniString("previous", "uploaded_by","");
        jsPrevious["golive_date"] = m_iniNs.GetIniString("previous", "golive_date","");
        jsPrevious["golive_by"] = m_iniNs.GetIniString("previous", "golive_by","");

    }
    else
    {
        jsCurrent["version"] = 0;
        jsCurrent["uploaded_date"] = "";
        jsCurrent["uploaded_by"] = "";
        jsCurrent["golive_date"] = "";
        jsCurrent["golive_by"] = "";

        jsPending["version"] = 0;
        jsPending["uploaded_date"] = "";
        jsPending["uploaded_by"] = "";

        jsPrevious["version"] = 0;
        jsPrevious["uploaded_date"] = "";
        jsPrevious["uploaded_by"] = "";
        jsPrevious["golive_date"] = "";
        jsPrevious["golive_by"] = "";

    }
    jsObject["current"] = jsCurrent;
    jsObject["pending"] = jsPending;
    jsObject["previous"] = jsPrevious;
    jsObject["path"] = m_sNsFilePath;
    jsObject["admin"] = bAdmin;

    Json::FastWriter ftw;
    mg_printf(pConnection, "%s", "HTTP/1.1 200 OK\r\nCache: no-cache\r\nContent-Type: application/x-javascript\r\n\r\n");
    mg_printf(pConnection, ftw.write(jsObject).c_str());
    pConnection->flags |= MG_F_SEND_AND_CLOSE;

}


void MongooseThread::ServeNetworkSource(mg_connection* pConnection, const string& sFile, http_message* pMessage)
{
    stringstream ssFile, ssContent;
    ssFile << m_sNsFilePath << sFile;
    ssContent << "Content-Disposition: attachment; filename=\"" << sFile << "\"";

    Log::Get(Log::LOG_INFO) << ssFile.str() << endl;
    //find the file
    if(ifstream(ssFile.str()).good())
    {
        //serve the file
        mg_http_serve_file(pConnection, pMessage, ssFile.str().c_str(), mg_mk_str("application/octet-stream"), mg_mk_str(ssContent.str().c_str()));
    }
}

void MongooseThread::ServeEpromSource(mg_connection* pConnection, const string& sVersion, bool bChangedOnly, http_message* pMessage)
{
    stringstream ssFile, ssContent, ssPath;
    ssFile <<  "all_";
    if(bChangedOnly)
    {
        ssFile << "changed_";
    }

    ssFile << "hex_files_" << sVersion << ".zip";
    ssPath << m_sNsFilePath << ssFile.str();

    ssContent << "Content-Disposition: attachment; filename=\"" << ssFile.str() << "\"";

    Log::Get(Log::LOG_INFO) << ssPath.str() << endl;
    //find the file
    if(ifstream(ssPath.str()).good())
    {
        //serve the file
        mg_http_serve_file(pConnection, pMessage, ssPath.str().c_str(), mg_mk_str("application/octet-stream"), mg_mk_str(ssContent.str().c_str()));
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << "Cannot find file" << endl;
    }

}




void MongooseThread::StartUpload(http_message* pMessage)
{
    map<uint64_t, session>::iterator itSession = GetSession(pMessage);
    if(itSession == m_mSessions.end())
    {
        Log::Get(Log::LOG_WARN) << "Start upload failed: not logged in" << endl;
        return;
    }
    // check not already uploading/checking a file
    if(m_iniNs.GetIniInt("pending", "version", 0) != 0)
    {
        Log::Get(Log::LOG_WARN) << "Start upload failed: file already being uploaded" << endl;
        return;

    }

    Log::Get(Log::LOG_INFO) << "Starting upload" << endl;
    m_iniNs.SetSectionValue("pending", "uploaded_by", itSession->second.sUser);
    m_iniNs.SetSectionValue("pending", "version", m_iniNs.GetIniInt("pending", "next",1));
    m_iniNs.SetSectionValue("pending", "status", "uploading");
    m_iniNs.WriteIniFile(m_sIniPath+"networksource.ini");

}

void MongooseThread::EndUpload(mg_connection *pConnection)
{

    Log::Get(Log::LOG_INFO) << "Finished upload" << endl;
    m_iniNs.SetSectionValue("pending", "status", "uploaded");


    //copy the file...
    stringstream ssFile;
    ssFile << m_sNsFilePath << "network_" << m_iniNs.GetIniInt("pending", "version",0) << ".src";
    ifstream src("/tmp/ns_temp.src", ios::binary);
    ofstream dst(ssFile.str(), ios::binary);
    dst << src.rdbuf();
    dst.close();
    src.close();
    remove("/tmp/ns_temp.src");

    // now call the network compiler
    stringstream ssBin;
    ssBin << "cd " << m_sNsBinPath << " && ./nwcompile " << m_iniNs.GetIniInt("pending", "version",0);

    bool bFileOk = (system(ssBin.str().c_str()) == 0);

    if(bFileOk)
    {
        Log::Get(Log::LOG_MSG) << m_iniNs.GetIniString("pending", "uploaded_by", "unknown") << " uploaded network source file version: "<< m_iniNs.GetIniInt("pending", "version", 0) << endl;

        // if succesfull the set the ini files correct
        m_iniNs.SetSectionValue("pending", "status", "pending");
        m_iniNs.SetSectionValue("pending", "uploaded_date", GetNowAsIso());
        m_iniNs.WriteIniFile(m_sIniPath+"networksource.ini");

        bool bAdmin = (m_iniUsers.GetIniString(m_iniNs.GetIniString("pending", "uploaded_by", "unknown"), "account", "User")=="Admin");
        SendNetworkSourceInfo(bAdmin, pConnection);
    }
    else
    {
        Log::Get(Log::LOG_ERROR) << m_iniNs.GetIniString("pending", "uploaded_by", "unknown") << " uploaded a network source file with errors." << endl;
        // If not then send error message back and write 0 to the pendind to show that it is
        int nError = m_iniNs.GetIniInt("pending", "version",0);
        m_iniNs.SetSectionValue("pending", "version", 0);
        m_iniNs.SetSectionValue("pending", "uploaded_date", "");
        m_iniNs.SetSectionValue("pending", "uploaded_by", "");
        m_iniNs.SetSectionValue("pending", "status", "");
        m_iniNs.WriteIniFile(m_sIniPath+"networksource.ini");
        SendError(pConnection, "The uploaded file has errors!",nError);
    }
}

void MongooseThread::DeleteNetworkSource(const std::string& sUser, const string& sVersion, mg_connection* pConnection, http_message* pMessage)
{

    bool bAdmin = (m_iniUsers.GetIniString(sUser, "account", "User")=="Admin");

    Log::Get(Log::LOG_INFO) << pMessage->query_string.p << endl;

    RemoveFiles(m_iniNs.GetIniString(sVersion, "version", "0"));

    m_iniNs.SetSectionValue(sVersion, "version", 0);
    m_iniNs.SetSectionValue(sVersion, "uploaded_date", "");
    m_iniNs.SetSectionValue(sVersion, "uploaded_by", "");
    m_iniNs.SetSectionValue(sVersion, "status", "");
    m_iniNs.WriteIniFile(m_sIniPath+"networksource.ini");


    SendNetworkSourceInfo(bAdmin, pConnection);
    Log::Get(Log::LOG_MSG) << sUser << " deleted " << sVersion << " network source file version: "<< m_iniNs.GetIniInt(sVersion, "version", 0) << endl;
}

void MongooseThread::MakeLive(const string& sUser, const string& sVersion, mg_connection* pConnection, http_message* pMessage)
{
    bool bAdmin = (m_iniUsers.GetIniString(sUser, "account", "User")=="Admin");
    if(bAdmin)
    {
        Log::Get(Log::LOG_INFO) << "MakLive" << endl;
        //get the file and copy rename it to network.src
        string sVersionNumber(m_iniNs.GetIniString(sVersion, "version", "0"));
        string sPreviousNumber(m_iniNs.GetIniString("previous", "version", "0"));

        stringstream ssFileVse;
        stringstream ssFileCrc;

        ssFileVse << m_sNsFilePath << "network_" << sVersionNumber << ".vse";
        ssFileCrc << m_sNsFilePath << "promcrc_" << sVersionNumber << ".crc";

        //copy network_xx.vse to vseinit.vse
        ifstream src(ssFileVse.str(), ios::binary);
        ofstream dst(m_sVsePath+"vseinit.vse", ios::binary);
        ifstream src_crc(ssFileCrc.str(), ios::binary);
        ofstream dst_crc(m_sNsFilePath+"promcrc_live.crc", ios::binary);
        if(src.is_open() == false)
        {
            SendError(pConnection, "Could not open "+ssFileVse.str());
            return;
        }
        if(dst.is_open() == false)
        {
            SendError(pConnection, "Could not overwrite "+m_sVsePath+"vseinit.vse");
            return;
        }
        if(src_crc.is_open() == false)
        {
            SendError(pConnection, "Could not open "+ssFileCrc.str());
            return;
        }
        if(dst_crc.is_open() == false)
        {
            SendError(pConnection, "Could not overwrite "+m_sNsFilePath+"promcrc_live.crc");
            return;
        }

        dst << src.rdbuf();
        dst.close();
        src.close();
        Log::Get(Log::LOG_INFO) << ssFileVse.str() << " copied to " << m_sVsePath << "vseinit.vse" << endl;

        dst_crc << src_crc.rdbuf();
        dst_crc.close();
        src_crc.close();
        Log::Get(Log::LOG_INFO) << ssFileCrc.str() << " copied to " << m_sNsFilePath << "promcrc_live.crc" << endl;

        // send command
        RdsDataManager::Get().QueueMessage("makelive");

        Log::Get(Log::LOG_MSG) << sUser << " made " << sVersion << " network source file version: "<< sVersionNumber << " LIVE!" << endl;
        Log::Get(Log::LOG_DEBUG) << "Update ini files" << endl;

	//store the uploaded info
	string sUploadedDate = m_iniNs.GetIniString(sVersion, "uploaded_date", "");
	string sUploadedBy = m_iniNs.GetIniString(sVersion, "uploaded_by", "");

        //change previous version to what was current
        m_iniNs.SetSectionValue("previous", "version", m_iniNs.GetIniString("current", "version","0"));
        m_iniNs.SetSectionValue("previous", "golive_date", m_iniNs.GetIniString("current", "golive_date",""));
        m_iniNs.SetSectionValue("previous", "golive_by", m_iniNs.GetIniString("current", "golive_by",""));
        m_iniNs.SetSectionValue("previous", "uploaded_date", m_iniNs.GetIniString("current", "uploaded_date",""));
        m_iniNs.SetSectionValue("previous", "uploaded_by", m_iniNs.GetIniString("current", "uploaded_by",""));

        //change current version to what we currently have
        m_iniNs.SetSectionValue("current", "version", sVersionNumber);
        //set current go live time
        m_iniNs.SetSectionValue("current", "golive_date", GetNowAsIso());
        m_iniNs.SetSectionValue("current", "golive_by", sUser);
        m_iniNs.SetSectionValue("current", "uploaded_date", sUploadedDate);
        m_iniNs.SetSectionValue("current", "uploaded_by", sUploadedBy);

        //remove the version files

        if(sVersion == "pending")
        {
            RemoveFiles(sPreviousNumber);            //no longer a pending version
            m_iniNs.SetSectionValue(sVersion, "version", 0);
            m_iniNs.SetSectionValue(sVersion, "uploaded_date", "");
            m_iniNs.SetSectionValue(sVersion, "uploaded_by", "");
            m_iniNs.SetSectionValue(sVersion, "status", "");
            m_iniNs.SetSectionValue(sVersion, "next", m_iniNs.GetIniInt(sVersion, "next", 1)+1);
        }
        m_iniNs.WriteIniFile(m_sIniPath+"networksource.ini");

    }
    SendNetworkSourceInfo(bAdmin, pConnection);

}


bool MongooseThread::RemoveFiles(const std::string& sVersion)
{
    stringstream ssFile;
    stringstream ssFileCrc;
    stringstream ssFileZip;
    stringstream ssFileAllZip;

    ssFile << m_sNsFilePath << "network_" << sVersion;
    ssFileCrc << m_sNsFilePath << "promcrc_" << sVersion << ".crc";
    ssFileZip << m_sNsFilePath << "all_changed_hex_files_" << sVersion << ".zip";
    ssFileAllZip << m_sNsFilePath << "all_hex_files_" << sVersion << ".zip";

    remove(string(ssFile.str()+".vse").c_str());
    remove(string(ssFile.str()+".src").c_str());
    remove(string(ssFile.str()+".rpt").c_str());
    remove(string(ssFile.str()+".err").c_str());

    remove(string(ssFileCrc.str()).c_str());
    remove(string(ssFileZip.str()).c_str());
    remove(string(ssFileAllZip.str()).c_str());
    remove(string(ssFile.str()+".rpt").c_str());

    return true;
}
