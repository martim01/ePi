#pragma once
#include <string>
#include <vector>
#include <chrono>

class AudioFile
{
    public:
        AudioFile(const std::string& sPath, const std::string& sUid) : m_sFile(sPath+sUid){}
        virtual ~AudioFile();

        virtual bool OpenToRead()=0;
        virtual int64_t ReadAudio(std::vector<float>& vSamples)=0;
        virtual bool Close()=0;

        virtual int	 GetFormat () const=0;
        virtual int	 GetChannels () const=0;
        virtual int	 GetSampleRate () const=0;

        virtual std::chrono::milliseconds GetLength() const=0;
        virtual void GoToStart()=0;

        const std::string& GetFileName() const {return m_sFile;}
    protected:
        std::string m_sFile;

};


