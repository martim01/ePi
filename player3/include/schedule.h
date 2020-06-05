#pragma once
#include <vector>
#include <string>
#include "json/json.h"
#include "epicron.h"
#include <atomic>

class Playout;
class iniManager;
class FileSource;
class Playlist;

class Schedule
{
    public:
        Schedule(Playout& player, const iniManager& iniConfig, const std::string& sUid);
        bool Play();

    private:

        struct item
        {
            enum eType{SND_FILE, MP3_FILE, PLAYLIST};

            item(const std::string& si, unsigned long nT, eType eM, bool bS=false) : sUid(si), nTimesToPlay(nT), eType(eM), bShuffle(bS){}
            std::string sUid;
            unsigned long nTimesToPlay;
            eType eType;
            bool bShuffle;

            CronJob job;
        };

        void PlayItem();
        void SetItem(size_t nItem);


        bool LoadSchedule();
        bool CreateSchedule(const Json::Value& jsFiles, const Json::Value& jsPlaylists, const Json::Value& jsSchedule);

        int GetFile(const Json::Value& jsFiles, const std::string& sUid);
        bool GetPlaylist(const Json::Value& jsFiles, const std::string& sUid);
        enum {FILE_NOT_FOUND, FILE_SOUND, FILE_MP3};

        std::vector<item> m_vSchedule;

        Playout& m_player;
        const iniManager& m_iniConfig;

        std::string m_sUid;


        std::atomic<size_t> m_nCurrentItem;

        std::unique_ptr<FileSource> m_pSource;
        std::unique_ptr<Playlist> m_pPlaylist;
};

