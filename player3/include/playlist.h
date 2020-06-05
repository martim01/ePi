#pragma once
#include <vector>
#include <string>
#include "json/json.h"


class Playout;
class iniManager;
class FileSource;


class Playlist
{
    public:
        Playlist(Playout& player, const iniManager& iniConfig, const std::string& sUid, unsigned long nTimesToPlay, bool bShuffle);
        bool Play();

        void Stop();

    private:
        struct item
        {
            item(const std::string& si, unsigned long nT, bool bM) : sUid(si), nTimesToPlay(nT), bMp3(bM){}
            std::string sUid;
            unsigned long nTimesToPlay;
            bool bMp3;
        };


        bool LoadPlaylist();
        bool CreatePlaylist(const Json::Value& jsFiles, const Json::Value& jsPlaylist);
        void ShufflePlaylist();

        int GetFile(const Json::Value& jsFiles, const std::string& sUid);
        enum {FILE_NOT_FOUND, FILE_SOUND, FILE_MP3};

        std::vector<item> m_vPlaylist;

        Playout& m_player;
        const iniManager& m_iniConfig;

        std::string m_sUid;
        unsigned long m_nTimesToPlay;
        bool m_bShuffle;

        std::unique_ptr<FileSource> m_pSource;
        bool m_bPlay;
};
