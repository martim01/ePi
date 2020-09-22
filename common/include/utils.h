#pragma once
#include <string>
#include <vector>
#include <sys/stat.h>
#include <chrono>
#include <queue>

extern std::vector<std::string> SplitString(std::string str, char cSplit, size_t nMax=0);
extern void SplitString(std::queue<std::string>& qSplit, std::string str, char cSplit);

extern std::string& ltrim(std::string& s);
extern std::string& rtrim(std::string& s);
extern std::string& trim(std::string& s);

extern bool CmpNoCase(const std::string& str1, const std::string& str2);
extern std::string CreatePath(std::string sPath);

extern bool do_mkdir(const std::string& sPath, mode_t mode);
extern bool mkpath(const std::string& sPath, mode_t mode);




extern std::string ConvertTimeToIsoString(std::time_t theTime);
extern std::string ConvertTimeToString(std::chrono::time_point<std::chrono::system_clock> tp, bool bIncludeNano=true);
extern std::string ConvertTimeToIsoString(std::chrono::time_point<std::chrono::system_clock> tp);
extern std::string GetCurrentTimeAsString(bool bIncludeNano=true);
extern std::string GetCurrentTimeAsIsoString();



extern std::string GetIpAddress(const std::string& sInterface);
extern std::string exec(const std::string& sCmd);


