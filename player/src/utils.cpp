#include "utils.h"
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <syslog.h>

using namespace std;

void SplitString(vector<string>& vSplit, string str, char cSplit, size_t nMax)
{
    vSplit.clear();
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

    vector<string> vSplit;
    SplitString(vSplit, sPath, '/');
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
