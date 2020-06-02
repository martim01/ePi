#pragma once
#include <vector>
#include <string>
#include "json/json.h"
#include "epicron.h"
#include <atomic>

class Playout;
class iniManager;
class FileSource;
class Schedule
{
    public:
        Schedule(Playout& player, const iniManager& iniConfig, const std::string& sUid);
        bool Play();

    private:
        struct item
        {
            item(const std::string& si, unsigned long nT, bool bM) : sUid(si), nTimesToPlay(nT), bMp3(bM){}
            std::string sUid;
            unsigned long nTimesToPlay;
            bool bMp3;
            CronJob job;
        };

        void PlayItem();
        void SetItem(size_t nItem);


        bool LoadSchedule();
        bool CreateSchedule(const Json::Value& jsFiles, const Json::Value& jsSchedule);

        int GetFile(const Json::Value& jsFiles, const std::string& sUid);
        enum {FILE_NOT_FOUND, FILE_SOUND, FILE_MP3};

        std::vector<item> m_vSchedule;

        Playout& m_player;
        const iniManager& m_iniConfig;

        std::string m_sUid;


        std::atomic<size_t> m_nCurrentItem;

        std::unique_ptr<FileSource> m_pSource;
};

