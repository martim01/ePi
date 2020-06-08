#pragma once
#include <string>
#include <memory>
#include "json/json.h"
#include <chrono>
#include <atomic>

class AudioFile;
class Playout;
class Resampler;
class iniManager;


class FileSource
{
    public:
        FileSource(Playout& player, const iniManager& iniConfig, const std::string& sUid, unsigned long nTimesToPlay, bool bMp3);
        ~FileSource();
        bool Play();
        void Stop();

    private:

        bool PlayOnce();

        Playout& m_player;
        const iniManager& m_iniConfig;

        std::string m_sUid;
        unsigned long m_nTimesToPlay;

        std::unique_ptr<AudioFile> m_pFile;
        std::unique_ptr<Resampler> m_pSampler;

        Json::Value m_jsStatus;
        std::chrono::time_point<std::chrono::system_clock> m_start;

        std::atomic<bool> m_bPlay;

        static const size_t BUFFER_SIZE=8192;
        static const size_t BUFFER_QUEUE_MIN=BUFFER_SIZE*2;
        static const size_t BUFFER_QUEUE_MAX=BUFFER_SIZE*5;
};
