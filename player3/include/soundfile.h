#pragma once
#include <chrono>
#include <memory>
#include <vector>
#include "audiofile.h"

class SndfileHandle;

class SoundFile : public AudioFile
{
    public:
        SoundFile(const std::string& sPath, const std::string& sUid);
        virtual ~SoundFile();

        bool OpenToRead() override;
        int64_t ReadAudio(std::vector<float>& vSamples) override;
        bool Close() override;

        int	 GetFormat () const override;
        int	 GetChannels () const override;
        int	 GetSampleRate () const override;

        std::chrono::milliseconds GetLength() const override;

        void GoToStart() override;


    private:
        std::unique_ptr<SndfileHandle> m_pHandle;

};

