#pragma once
#include <string>
#include <vector>
#include <sys/stat.h>

extern void SplitString(std::vector<std::string>& vSplit, std::string str, char cSplit, size_t nMax=0);
extern bool CmpNoCase(const std::string& str1, const std::string& str2);
extern std::string CreatePath(std::string sPath);

extern bool do_mkdir(const std::string& sPath, mode_t mode);
extern bool mkpath(const std::string& sPath, mode_t mode);
