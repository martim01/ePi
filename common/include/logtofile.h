#pragma once
#include "log.h"
#include <string>
#include <fstream>

class LogToFile : public pml::LogOutput
{
    public:
        explicit LogToFile(const std::string& sRootPath);
        virtual ~LogToFile(){}
        void Flush(int nLogLevel, const std::stringstream&  logStream) override;

    private:

        void OpenFile(const std::string& sFilePath, const std::string& sFileName);
        std::string m_sRootPath;
        std::string m_sFilePath;
        std::string m_sFileName;

        std::ofstream m_ofLog;
	bool m_bLogToConsole;
};

