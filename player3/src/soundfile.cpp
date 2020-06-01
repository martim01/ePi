#include "soundfile.h"
#include "sndfile.hh"
#include <iostream>
#include "epiwriter.h"

SoundFile::SoundFile(const std::string& sPath, const std::string& sUid) : AudioFile(sPath, sUid),
    m_pHandle(nullptr)
{
}



SoundFile::~SoundFile()
{

}

bool SoundFile::OpenToRead()
{
    if(m_pHandle)
	{
		Close();
	}

	Json::Value jsDebug;
	jsDebug["action"] = "Open To Read;";

    m_pHandle = std::unique_ptr<SndfileHandle>(new SndfileHandle(m_sFile, SFM_READ, 0,0,0));
    if(!m_pHandle.get())
    {
        jsDebug["error"] = m_pHandle->strError();
        epiWriter::Get().writeToStdOut(jsDebug);
        return false;
    }
    jsDebug["succes"] = "true";
    epiWriter::Get().writeToStdOut(jsDebug);
    return true;
}

int64_t SoundFile::ReadAudio(std::vector<float>& vSamples)
{
    size_t nRead(0);
    if(m_pHandle)
    {
        float* pbuffer(vSamples.data());

        //try to fill the whole sample buffer
        nRead = m_pHandle->read(pbuffer, vSamples.size());

    }
    vSamples.resize(nRead);
    return nRead;
}

bool SoundFile::Close()
{
    m_pHandle = nullptr;
    return true;
}

int	SoundFile::GetFormat () const
{
    if(m_pHandle)
    {
        return m_pHandle->format();
    }

    return 0;
}

int	SoundFile::GetChannels () const
{

    if(m_pHandle)
    {
        return m_pHandle->channels();
    }

    return 0;
}

int	SoundFile::GetSampleRate () const
{
    if(m_pHandle)
    {
        return m_pHandle->samplerate();
    }

    return 0;
}


std::chrono::milliseconds SoundFile::GetLength() const
{
    if(m_pHandle && GetSampleRate() > 0)
    {
        auto nFrames = m_pHandle->seek(0, SEEK_END);
        nFrames *= 1000;
        nFrames /= GetSampleRate();
        m_pHandle->seek(0,SEEK_SET);
        return std::chrono::milliseconds(nFrames);
    }

    return std::chrono::milliseconds(0);
}

void SoundFile::GoToStart()
{
    if(m_pHandle)
    {
        m_pHandle->seek(0, SEEK_SET);
    }
}
