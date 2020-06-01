#pragma once
#include <chrono>
#include <memory>
#include <vector>
#include "audiofile.h"



class Mp3File : public AudioFile
{
    public:
        Mp3File(const std::string& sPath, const std::string& sUid);
        virtual ~Mp3File();

        bool OpenToRead() override;
        int64_t ReadAudio(std::vector<float>& vSamples) override;
        bool Close() override;

        int	 GetFormat () const override;
        int	 GetChannels () const override;
        int	 GetSampleRate () const override;

        std::chrono::milliseconds GetLength() const override;

        void GoToStart() override;

    private:

};


