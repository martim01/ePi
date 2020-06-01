#pragma once
#include <string>
#include <memory>
#include "json/json.h"
#include <chrono>

class AudioFile;
class Playout;
class Resampler;

class FileSource
{
    public:
        FileSource(Playout& player, const std::string& sPath, const std::string& sUid, unsigned long nTimesToPlay, bool bMp3);
        ~FileSource();
        bool Play();

    private:

        bool PlayOnce();

        Playout& m_player;

        std::string m_sPath;
        std::string m_sUid;
        unsigned long m_nTimesToPlay;

        std::unique_ptr<AudioFile> m_pFile;
        std::unique_ptr<Resampler> m_pSampler;

        Json::Value m_jsStatus;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

        static const size_t BUFFER_SIZE=8192;
        static const size_t BUFFER_QUEUE_MIN=BUFFER_SIZE*2;
        static const size_t BUFFER_QUEUE_MAX=BUFFER_SIZE*5;
};
