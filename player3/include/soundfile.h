#pragma once
#include <chrono>
#include <memory>
#include <vector>


class SndfileHandle;

class SoundFile
{
    public:
        SoundFile(const std::string& sPath, const std::string& sUid);
        virtual ~SoundFile();

        bool OpenToRead();
        int64_t ReadAudio(std::vector<float>& vSamples, size_t& nOffset, int& nLoop);
        bool Close();

        int	 GetFormat () const;
        int	 GetChannels () const;
        int	 GetSampleRate () const;

        std::chrono::milliseconds GetLength() const;

    private:
        std::string m_sFile;
        std::unique_ptr<SndfileHandle> m_pHandle;


};

