#pragma once
#include "audiofile.h"
#include <memory>

class SndfileHandle;

class WavFile : public AudioFile
{
    public:
        WavFile(const std::string& sUid, const std::string& sLabel, const std::string& sDescription);
        WavFile();
        WavFile(const Json::Value& jsData);
        virtual ~WavFile();

        static int IsWavFile(const std::string& sFile);
        bool OpenToRead() override;
        int64_t ReadAudio(float* pBuffer, int nSize, unsigned int nLoop) override;
        bool Close() override;

        int	 GetFormat () const override;
        int	 GetChannels () const override;
        int	 GetSampleRate () const override;

        std::chrono::milliseconds GetLength() const override;

    private:

        Json::Value GetFormatAsJson() override;

        std::unique_ptr<SndfileHandle> m_pHandle;


};
