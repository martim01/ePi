#pragma once
#include "audiofile.h"
#include <memory>



class Mp3File : public AudioFile
{
    public:
        Mp3File(const std::string& sUid, const std::string& sLabel, const std::string& sDescription);
        Mp3File();
        virtual ~Mp3File();

        bool OpenToRead() override;
        int64_t ReadAudio(float* pBuffer, int nSize, unsigned int nLoop) override;
        bool Close() override;

        int	 GetFormat () const override;
        int	 GetChannels () const override;
        int	 GetSampleRate () const override;

        std::chrono::milliseconds GetLength() const override;

    private:

        Json::Value GetFormatAsJson() override;


};

