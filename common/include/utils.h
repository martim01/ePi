#pragma once
#include <string>
#include <vector>
#include <sys/stat.h>
#include "json/json.h"
#include <chrono>
#include <queue>

extern std::vector<std::string> SplitString(std::string str, char cSplit, size_t nMax=0);
extern void SplitString(std::queue<std::string>& qSplit, std::string str, char cSplit);


extern bool CmpNoCase(const std::string& str1, const std::string& str2);
extern std::string CreatePath(std::string sPath);

extern bool do_mkdir(const std::string& sPath, mode_t mode);
extern bool mkpath(const std::string& sPath, mode_t mode);


extern bool CheckJson(const Json::Value& jsObject, std::initializer_list<std::string> lstAllowed);
extern bool JsonMemberExistsAndIsNull(const Json::Value& jsObject, const std::string& sMember);
extern bool JsonMemberExistsAndIsNotNull(const Json::Value& jsObject, const std::string& sMember);

extern std::string ConvertTimeToString(std::chrono::time_point<std::chrono::system_clock> tp, bool bIncludeNano=true);
extern std::string ConvertTimeToIsoString(std::chrono::time_point<std::chrono::system_clock> tp);
extern std::string GetCurrentTimeAsString(bool bIncludeNano=true);
extern std::string GetCurrentTimeAsIsoString();

extern Json::Value ConvertToJson(const std::string& str);

extern std::string GetIpAddress(const std::string& sInterface);
extern std::string exec(const std::string& sCmd);

extern void UpdateJsonObject(Json::Value& dest, const Json::Value& source);
