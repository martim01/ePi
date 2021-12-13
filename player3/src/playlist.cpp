#include "playlist.h"
#include <algorithm>
#include <random>
#include "filesource.h"
#include <fstream>
#include "inimanager.h"
#include "epiutils.h"
#include "log.h"
#include "resources.h"

Playlist::Playlist(Playout& player, const iniManager& iniConfig, const std::string& sUid, unsigned long nTimesToPlay, bool bShuffle) :
  m_player(player),
  m_iniConfig(iniConfig),
  m_sUid(sUid),
  m_nTimesToPlay(nTimesToPlay),
  m_bShuffle(bShuffle),
  m_pSource(nullptr),
  m_bPlay(true)
{

}

bool Playlist::Play()
{
    if(!LoadPlaylist())
    {
        return false;
    }
    if(m_bShuffle)
    {
        ShufflePlaylist();
    }

    for(size_t nLoop = 1; nLoop <= m_nTimesToPlay; nLoop++)
    {
        for(auto anItem  : m_vPlaylist)
        {
            m_pSource = std::unique_ptr<FileSource>(new FileSource(m_player, m_iniConfig, anItem.sUid, anItem.nTimesToPlay, anItem.bMp3));
            m_pSource->Play();

            if(m_bPlay == false)
            {
                return true;
            }
        }
    }
    return true;
}


bool Playlist::LoadPlaylist()
{
    if(Resources::Get().IsValid())
    {
        for(Json::ArrayIndex i = 0; i < Resources::Get().GetJson()["playlists"].size(); i++)
        {
            if(Resources::Get().GetJson()["playlists"][i]["uid"].asString() == m_sUid)
            {
                return CreatePlaylist(Resources::Get().GetJson()["files"], Resources::Get().GetJson()["playlists"][i]);
            }
        }
        return false;
    }
    return false;

}

bool Playlist::CreatePlaylist(const Json::Value& jsFiles, const Json::Value& jsPlaylist)
{
    if(jsPlaylist["files"].isArray() == false)
    {
        return false;
    }

    m_vPlaylist.reserve(jsPlaylist["files"].size());

    for(Json::ArrayIndex i = 0; i < jsPlaylist["files"].size(); i++)
    {
        if(jsPlaylist["files"][i]["uid"].isString() && jsPlaylist["files"][i]["times_to_play"].isInt())
        {
            switch(GetFile(jsFiles, jsPlaylist["files"][i]["uid"].asString()))
            {
                case FILE_SOUND:
                    m_vPlaylist.push_back(item(jsPlaylist["files"][i]["uid"].asString(), jsPlaylist["files"][i]["times_to_play"].asInt(), false));
                    break;
                case FILE_MP3:
                    m_vPlaylist.push_back(item(jsPlaylist["files"][i]["uid"].asString(), jsPlaylist["files"][i]["times_to_play"].asInt(), true));
                    break;
            }
        }
    }
    return true;
}

int Playlist::GetFile(const Json::Value& jsFiles, const std::string& sUid)
{
    for(Json::ArrayIndex i = 0; i < jsFiles.size(); i++)
    {
        if(jsFiles[i]["uid"].isString() && jsFiles[i]["uid"].asString() == sUid)
        {
            if(jsFiles[i]["type"].isString() && CmpNoCase(jsFiles[i]["type"].asString(), "wavfile"))
            {
                return FILE_SOUND;
            }
            else if(jsFiles[i]["type"].isString() && CmpNoCase(jsFiles[i]["type"].asString(), "mp3file"))
            {
                return FILE_MP3;
            }
            else
            {
                return FILE_NOT_FOUND;
            }
        }
    }
    return FILE_NOT_FOUND;
}

void Playlist::ShufflePlaylist()
{
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(m_vPlaylist.begin(), m_vPlaylist.end(), g);
}


void Playlist::Stop()
{
    if(m_pSource)
    {
        m_pSource->Stop();
    }
}
