#include "logtofile.h"
#include "utils.h"
#include <syslog.h>
#include <chrono>
#include <iomanip>


LogToFile::LogToFile(const std::string& sRootPath) : m_sRootPath(CreatePath(sRootPath))
{
}

void LogToFile::OpenFile(const std::string& sFilePath, const std::string& sFileName)
{
    if(m_ofLog.is_open())
    {
        m_ofLog.close();
    }

    m_sFilePath = sFilePath;
    m_sFileName = sFileName;
    if(!mkpath(m_sFilePath, 0755))
    {
        std::stringstream ss;
        ss << "Unable to create log file "  << sFilePath;
        syslog(LOG_WARNING, ss.str().c_str());
    }
    std::string sFile(m_sFilePath+m_sFileName);
    m_ofLog.open(sFile, std::fstream::app);
    //chande the permissions
    chmod(sFile.c_str(), 0664);
}

void LogToFile::Flush(int nLogLevel, const std::stringstream&  logStream)
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ssFilePath;
    std::stringstream ssFileName;
    ssFilePath << m_sRootPath << std::put_time(localtime(&in_time_t), "%Y/%m/%d");
    ssFileName << std::put_time(localtime(&in_time_t), "/%H") << ".log";

    if(m_ofLog.is_open() == false || ssFilePath.str() != m_sFilePath || ssFileName.str() != m_sFileName)
    {
        OpenFile(ssFilePath.str(), ssFileName.str());
    }

    if(m_ofLog.is_open())
    {
        m_ofLog << std::put_time(localtime(&in_time_t), "%d/%m/%y\t%H:%M:%S") << "\t";
        m_ofLog << pml::Log::STR_LEVEL[nLogLevel] << "\t" << logStream.str();
        m_ofLog.flush();
    }
}



