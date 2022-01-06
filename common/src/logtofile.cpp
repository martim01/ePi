#include "logtofile.h"
#include "epiutils.h"
#include <syslog.h>
#include <chrono>
#include <iomanip>


LogToFile::LogToFile(const std::string& sRootPath,int nTimestamp, pml::LogOutput::enumTS eResolution) : LogOutput(nTimestamp, eResolution),
m_sRootPath(CreatePath(sRootPath))
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

void LogToFile::Flush(pml::enumLevel eLogLevel, const std::stringstream&  logStream)
{
    if(eLogLevel >= m_eLevel)
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ssFilePath;
        std::stringstream ssFileName;
        ssFileName << std::put_time(localtime(&in_time_t), "/%Y-%m-%dT%H") << ".log";

        if(m_ofLog.is_open() == false || m_sRootPath != m_sFilePath || ssFileName.str() != m_sFileName)
        {
            OpenFile(m_sRootPath, ssFileName.str());
        }

        if(m_ofLog.is_open())
        {
            m_ofLog << Timestamp().str();
            m_ofLog << pml::LogStream::STR_LEVEL[eLogLevel] << "\t" << logStream.str();
            m_ofLog.flush();
        }
    }
}



