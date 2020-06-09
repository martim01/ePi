#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <syslog.h>
#include "utils.h"
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

using namespace std;


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
    std::chrono::time_point<std::chrono::system_clock> tp(std::chrono::system_clock::now());
    return ConvertTimeToString(tp, bIncludeNano);
}

std::string GetCurrentTimeAsIsoString()
{
    std::chrono::time_point<std::chrono::system_clock> tp(std::chrono::system_clock::now());
    return ConvertTimeToIsoString(tp);
}

std::string ConvertTimeToString(std::chrono::time_point<std::chrono::system_clock> tp, bool bIncludeNano)
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

std::string ConvertTimeToIsoString(std::chrono::time_point<std::chrono::system_clock> tp)
{
    std::time_t  t = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;

    ss << std::put_time(std::localtime(&t), "%FT%T%z");
    return ss.str();
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
    Json::Value jsData;
    try
    {
        std::stringstream ss;
        ss.str(str);

        ss >> jsData;

    }
    catch(const Json::RuntimeError& e)
    {
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << str << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }

    return jsData;
}


std::string GetIpAddress(const std::string& sInterface)
{
    int fd = socket(AF_INET, SOCK_DGRAM,0);
    ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy((char*)ifr.ifr_ifrn.ifrn_name, sInterface.c_str(), IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    return inet_ntoa((((sockaddr_in*)&ifr.ifr_addr)->sin_addr));

}
