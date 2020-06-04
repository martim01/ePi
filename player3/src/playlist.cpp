#include "playlist.h"
#include <algorithm>
#include <random>
#include "filesource.h"
#include <fstream>
#include "inimanager.h"
#include "utils.h"
#include "log.h"


Playlist::Playlist(Playout& player, const iniManager& iniConfig, const std::string& sUid, unsigned long nTimesToPlay, bool bShuffle) :
  m_player(player),
  m_iniConfig(iniConfig),
  m_sUid(sUid),
  m_nTimesToPlay(nTimesToPlay),
  m_bShuffle(bShuffle)
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
            FileSource fs(m_player, CreatePath(m_iniConfig.GetIniString("Paths", "Audio", "/var/ePi/audio")), anItem.sUid, anItem.nTimesToPlay, anItem.bMp3);
            fs.Play();
        }
    }
    return true;
}


bool Playlist::LoadPlaylist()
{
    std::ifstream ifs;
    ifs.open(CreatePath(m_iniConfig.GetIniString("Paths", "Resources", "/var/ePi/resources"))+"resources.json", std::ifstream::in);
    if(!ifs.is_open())
    {
        pml::Log::Get(pml::Log::LOG_CRITICAL) << "Playlist\tCould not open resources file!" << std::endl;
		return false;
	}
	try
	{
        Json::Value jsResources;
        ifs >> jsResources;

        if(jsResources["playlists"].isArray() && jsResources["files"].isArray())
        {
            for(size_t i = 0; i < jsResources["playlists"].size(); i++)
            {
                if(jsResources["playlists"][i]["uid"].asString() == m_sUid)
                {
                    return CreatePlaylist(jsResources["files"], jsResources["playlists"][i]);
                }
            }
        }
        return true;
    }
    catch(const Json::RuntimeError& e)
    {
        return false;
    }
}

bool Playlist::CreatePlaylist(const Json::Value& jsFiles, const Json::Value& jsPlaylist)
{
    if(jsPlaylist["files"].isArray() == false)
    {
        return false;
    }

    m_vPlaylist.reserve(jsPlaylist["files"].size());

    for(size_t i = 0; i < jsPlaylist["files"].size(); i++)
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
    for(size_t i = 0; i < jsFiles.size(); i++)
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

