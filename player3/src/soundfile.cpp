#include "soundfile.h"
#include "sndfile.hh"


SoundFile::SoundFile(const std::string& sPath, const std::string& sUid) :
    m_sFile(sPath+sUid),
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
    m_pHandle = std::unique_ptr<SndfileHandle>(new SndfileHandle(m_sFile, SFM_READ, 0,0,0));
    return (m_pHandle!=nullptr);
}

int64_t SoundFile::ReadAudio(std::vector<float> vSamples, size_t& nOffset, int& nLoop)
{
    int64_t nRead(0);
    int64_t nTotalRead(0);

    if(m_pHandle && nOffset < vSamples.size())
    {
        float* pbuffer(vSamples.data());

        size_t nToRead = vSamples.size()-nOffset;

        //try to fill the whole sample buffer
        nRead = m_pHandle->read(&pbuffer[nOffset+nTotalRead], (nToRead-nTotalRead));

        nTotalRead += nRead;    //store how much read

        if(nTotalRead < nToRead)
        {
            --nLoop;    //we've looped
            if(nLoop != 0) //are we looping
            {
                while(nTotalRead < nToRead)    //not read all so got to end of file
                {

                    m_pHandle->seek(SEEK_SET, 0);   //seek back to beginning of file
                    nRead = m_pHandle->read(&pbuffer[nOffset+nTotalRead], (nToRead-nTotalRead));
                    nTotalRead += nRead;    //store how much read
                }
            }
        }
    }
    nOffset += nTotalRead;
    nOffset %= vSamples.size();
    return nTotalRead;
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
    if(m_pHandle)
    {
        auto nFrames = m_pHandle->seek(0, SEEK_END);
        nFrames *= 1000;
        nFrames /= GetSampleRate();
        m_pHandle->seek(0,SEEK_SET);
        return std::chrono::milliseconds(nFrames);
    }

    return std::chrono::milliseconds(0);
}

