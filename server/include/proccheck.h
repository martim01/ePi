#pragma once
#include <string>

extern bool IsProcRunning(const std::string& sProc);
extern pid_t FindProc(const std::string& sProc, bool bKill);
extern int KillProc(const std::string& sProc);
