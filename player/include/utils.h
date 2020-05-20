#pragma once
#include <string>
#include <vector>
#include <sys/stat.h>
#include "json/json.h"
#include <chrono>
#include <queue>

extern void SplitString(std::vector<std::string>& vSplit, std::string str, char cSplit, size_t nMax=0);
extern void SplitString(std::queue<std::string>& qSplit, std::string str, char cSplit);


extern bool CmpNoCase(const std::string& str1, const std::string& str2);
extern std::string CreatePath(std::string sPath);

extern bool do_mkdir(const std::string& sPath, mode_t mode);
extern bool mkpath(const std::string& sPath, mode_t mode);


extern std::string CreateGuid(const std::string& sName);
extern std::string CreateGuid();


extern bool CheckJson(const Json::Value& jsObject, std::initializer_list<std::string> lstAllowed);
extern bool JsonMemberExistsAndIsNull(const Json::Value& jsObject, const std::string& sMember);
extern bool JsonMemberExistsAndIsNotNull(const Json::Value& jsObject, const std::string& sMember);

extern std::string ConvertTimeToString(std::chrono::time_point<std::chrono::high_resolution_clock> tp, bool bIncludeNano=true);
extern std::string GetCurrentTimeAsString(bool bIncludeNano=true);
