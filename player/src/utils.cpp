#include "utils.h"
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <syslog.h>
#include "guid.h"
#include "utils.h"


using namespace std;

static uuid_t NameSpace_OID = { /* 6ba7b812-9dad-11d1-80b4-00c04fd430c8 */
       0x6ba7b812,
       0x9dad,
       0x11d1,
       0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
   };




vector<string> SplitString(string str, char cSplit, size_t nMax)
{
    vector<string> vSplit;
    istringstream f(str);
    string s;

    while (getline(f, s, cSplit))
    {
        if(s.empty() == false)
        {
            if(nMax == 0 || vSplit.size() < nMax)
            {
                vSplit.push_back(s);
            }
            else
            {
                vSplit[nMax-1] = vSplit[nMax-1]+cSplit+s;
            }
        }
    }
    return vSplit;
}

void SplitString(queue<string>& qSplit, string str, char cSplit)
{
    while(qSplit.empty() == false)
    {
        qSplit.pop();
    }

    istringstream f(str);
    string s;

    while (getline(f, s, cSplit))
    {
        if(s.empty() == false)
        {
            qSplit.push(s);
        }
    }
}


bool CmpNoCase(const string& str1, const string& str2)
{
    return ((str1.size() == str2.size()) && equal(str1.begin(), str1.end(), str2.begin(), [](char c1, char c2)
    {
        return (c1==c2 || toupper(c1)==toupper(c2));
    }));
}

string CreatePath(string sPath)
{
    if(sPath[sPath.length()-1] != '/' && sPath[sPath.length()-1] != '\\')
    {
        sPath+= '/';
    }
    return sPath;
}


bool do_mkdir(const std::string& sPath, mode_t mode)
{
    struct stat st;
    bool bStatus(true);


    if (stat(sPath.c_str(), &st) != 0)
    {
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir(sPath.c_str(), mode) != 0 && errno != EEXIST)
        {
	    std::stringstream ss;
            ss <<"do_mkdir("<<sPath<<") " << strerror(errno);
	    syslog(LOG_WARNING, ss.str().c_str());
            bStatus = false;
        }
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
	std::stringstream ss;
        ss <<"do_mkdir("<<sPath<<") " << strerror(errno);
	syslog(LOG_WARNING, ss.str().c_str());
        bStatus = false;
    }

    return bStatus;
}

bool mkpath(const std::string& sPath, mode_t mode)
{

    vector<string> vSplit(SplitString(sPath, '/'));
    stringstream ssPath;

    for(size_t i = 0; i < vSplit.size(); i++)
    {
        if(sPath[0] == '/' && i == 0)
        {
            ssPath << '/';
        }
        ssPath << vSplit[i];
        if(do_mkdir(ssPath.str(), mode) == false)
        {
            return false;
        }

        ssPath << "/";
    }

    return true;
}


std::string GetCurrentTimeAsString(bool bIncludeNano)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> tp(std::chrono::high_resolution_clock::now());
    return ConvertTimeToString(tp, bIncludeNano);
}

std::string GetCurrentTimeAsIsoString()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> tp(std::chrono::high_resolution_clock::now());
    return ConvertTimeToIsoString(tp);
}

std::string ConvertTimeToString(std::chrono::time_point<std::chrono::high_resolution_clock> tp, bool bIncludeNano)
{
    std::stringstream sstr;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
    sstr << seconds.count();
    if(bIncludeNano)
    {
        sstr << ":" << (std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count()%1000000000);
    }
    return sstr.str();
}

std::string ConvertTimeToIsoString(std::chrono::time_point<std::chrono::high_resolution_clock> tp)
{
    std::time_t  t = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;

    ss << std::put_time(std::localtime(&t), "%FT%T%z");
    return ss.str();
}


std::string CreateGuid(const std::string& sName)
{
    uuid_t guid;
    uuid_create_md5_from_name(&guid, NameSpace_OID, sName.c_str(), sName.length());

    std::stringstream ss;
    ss << std::hex
       << std::setw(8) << std::setfill('0') << guid.time_low << "-"
       << std::setw(4) << std::setfill('0') << guid.time_mid << "-"
       << std::setw(4) << std::setfill('0') << guid.time_hi_and_version << "-"
       << std::setw(2) << std::setfill('0') << (int)guid.clock_seq_hi_and_reserved
       << std::setw(2) << std::setfill('0') << (int)guid.clock_seq_low << "-"
       << std::setw(2) << std::setfill('0') << (int)guid.node[0]
       << std::setw(2) << std::setfill('0') << (int)guid.node[1]
       << std::setw(2) << std::setfill('0') << (int)guid.node[2]
       << std::setw(2) << std::setfill('0') << (int)guid.node[3]
       << std::setw(2) << std::setfill('0') << (int)guid.node[4]
       << std::setw(2) << std::setfill('0') << (int)guid.node[5];

    return ss.str();
    /*std::array<char,40> output;
    snprintf(output.data(), output.size(), "%08x-%04hx-%04hx-%02x%02x-%02x%02x%02x%02x%02x%02x",
             guid.time_low, guid.time_mid, guid.time_hi_and_version, guid.clock_seq_hi_and_reserved, guid.clock_seq_low,
             guid.node[0], guid.node[1], guid.node[2], guid.node[3], guid.node[4], guid.node[5]);
    return std::string(output.data());*/
}


std::string CreateGuid()
{
    return CreateGuid(GetCurrentTimeAsString(true));
}


bool CheckJson(const Json::Value& jsObject, std::initializer_list<std::string> lstAllowed)
{
    for(Json::Value::const_iterator itParam = jsObject.begin(); itParam != jsObject.end(); ++itParam)
    {
        std::initializer_list<std::string>::iterator itList = lstAllowed.begin();
        for(; itList != lstAllowed.end(); ++itList)
        {
            if((*itList) == itParam.key().asString())
                break;
        }
        if(itList == lstAllowed.end())   //found a non allowed thing
        {
            return false;
        }
    }
    return true;
}


bool JsonMemberExistsAndIsNull(const Json::Value& jsObject, const std::string& sMember)
{
    return (jsObject.isMember(sMember) && jsObject[sMember].isNull());
}

bool JsonMemberExistsAndIsNotNull(const Json::Value& jsObject, const std::string& sMember)
{
    return (jsObject.isMember(sMember) && (jsObject[sMember].isNull()==false));
}


Json::Value ConvertToJson(const std::string& str)
{
    std::stringstream ss;
    ss.str(str);
    Json::Value jsData;
    ss >> jsData;

    return jsData;
}



