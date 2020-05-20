#pragma once
#include "resource.h"
#include <chrono>

class AudioFile : public Resource
{
    public:
        AudioFile(const std::string& sType, const std::string& sUid, const std::string& sLabel, const std::string& sDescription);
        AudioFile(const std::string& sType);


        virtual bool OpenToRead()=0;
        virtual int64_t ReadAudio(float* pBuffer, int nSize, unsigned int nLoop)=0;
        virtual bool Close()=0;

        virtual int	 GetFormat() const=0;
        virtual int	 GetChannels() const=0;
        virtual int	 GetSampleRate() const=0;

        virtual std::chrono::milliseconds GetLength() const=0;

    protected:
        void UpdateJson() override;
        virtual Json::Value GetFormatAsJson()=0;
};
