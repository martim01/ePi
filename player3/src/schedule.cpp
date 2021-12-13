#include "schedule.h"
#include <algorithm>
#include <random>
#include "filesource.h"
#include <fstream>
#include "inimanager.h"
#include "epiutils.h"
#include "log.h"
#include <thread>
#include "epiwriter.h"
#include "resources.h"
#include "playlist.h"

Schedule::Schedule(Playout& player, const iniManager& iniConfig, const std::string& sUid) :
  m_player(player),
  m_iniConfig(iniConfig),
  m_sUid(sUid),
  m_pSource(nullptr),
  m_pPlaylist(nullptr),
  m_pThread(nullptr),
  m_bRun(true)
{

}

Schedule::~Schedule()
{
    if(m_pThread)
    {
        m_bRun = false;
        m_pThread->join();
    }
}

bool Schedule::Play()
{
    if(!LoadSchedule())
    {

        return false;
    }

    m_nCurrentItem = m_vSchedule.size();

    //launch thread to do the actual schedule...

    m_pThread = std::make_unique<std::thread>([this]() {
        while(m_bRun)
        {
            auto now = std::chrono::system_clock::now();
            for(size_t i = 0; i < m_vSchedule.size(); i++)
            {
                if(m_vSchedule[i].job.JobNow(now))
                {
                    SetItem(i);
                }
            }
            std::this_thread::sleep_until(now+std::chrono::seconds(1));
        }
    });


    while(m_bRun)
    {
        if(m_nCurrentItem < m_vSchedule.size())
        {
            PlayItem(); //this sets m_nCurrentItem to m_vSchedule.size() so is only called once
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            Json::Value jsStatus;
            jsStatus["action"] = "Waiting";
            jsStatus["time"] = GetCurrentTimeAsIsoString();
            epiWriter::Get().writeToStdOut(jsStatus);
        }
    }

    return true;
}


bool Schedule::LoadSchedule()
{

    if(Resources::Get().IsValid())
    {
        for(Json::ArrayIndex i = 0; i < Resources::Get().GetJson()["schedules"].size(); i++)
        {
            if(Resources::Get().GetJson()["schedules"][i]["uid"].asString() == m_sUid)
            {
                return CreateSchedule(Resources::Get().GetJson()["files"], Resources::Get().GetJson()["playlists"], Resources::Get().GetJson()["schedules"][i]);
            }
        }
    }
    return false;
}



bool Schedule::CreateSchedule(const Json::Value& jsFiles, const Json::Value& jsPlaylists, const Json::Value& jsSchedule)
{
    m_vSchedule.reserve(100);

    if(jsSchedule["files"].isArray())
    {
        for(Json::ArrayIndex i = 0; i < jsSchedule["files"].size(); i++)
        {
            if(jsSchedule["files"][i]["uid"].isString() && jsSchedule["files"][i]["times_to_play"].isInt() && jsSchedule["files"][i]["cron"].isString())
            {
                switch(GetFile(jsFiles, jsSchedule["files"][i]["uid"].asString()))
                {
                    case FILE_SOUND:
                        m_vSchedule.push_back(item(jsSchedule["files"][i]["uid"].asString(), jsSchedule["files"][i]["times_to_play"].asInt(), item::SND_FILE));
                        m_vSchedule.back().job.SetString(jsSchedule["files"][i]["cron"].asString());
                        break;
                    case FILE_MP3:
                        m_vSchedule.push_back(item(jsSchedule["files"][i]["uid"].asString(), jsSchedule["files"][i]["times_to_play"].asInt(), item::MP3_FILE));
                        m_vSchedule.back().job.SetString(jsSchedule["files"][i]["cron"].asString());
                        break;
                }
            }
        }
    }
    if(jsSchedule["playlists"].isArray())
    {
        for(Json::ArrayIndex i = 0; i < jsSchedule["playlists"].size(); i++)
        {
            if(jsSchedule["playlists"][i]["uid"].isString() == false)
            {
                pmlLog() << "uid" << std::endl;
            }
            if(jsSchedule["playlists"][i]["times_to_play"].isInt() == false)
            {
                pmlLog() << "times_to_play" << std::endl;
            }
            if(jsSchedule["playlists"][i]["cron"].isString() == false)
            {
                pmlLog() << "cron" << std::endl;
            }
            if(jsSchedule["playlists"][i]["shuffle"].isBool() == false)
            {
                pmlLog() << "shuffle" << std::endl;
            }


            if(jsSchedule["playlists"][i]["uid"].isString() && jsSchedule["playlists"][i]["times_to_play"].isInt() && jsSchedule["playlists"][i]["cron"].isString() && jsSchedule["playlists"][i]["shuffle"].isBool())
            {
                if(GetPlaylist(jsPlaylists, jsSchedule["playlists"][i]["uid"].asString()))
                {
                    m_vSchedule.push_back(item(jsSchedule["playlists"][i]["uid"].asString(), jsSchedule["playlists"][i]["times_to_play"].asInt(), item::PLAYLIST, jsSchedule["playlists"][i]["shuffle"].asBool()));
                    m_vSchedule.back().job.SetString(jsSchedule["playlists"][i]["cron"].asString());
                }
            }

        }
    }


    return (m_vSchedule.empty() == false);
}

int Schedule::GetFile(const Json::Value& jsFiles, const std::string& sUid)
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

bool Schedule::GetPlaylist(const Json::Value& jsPlaylists, const std::string& sUid)
{
    for(Json::ArrayIndex i = 0; i < jsPlaylists.size(); i++)
    {
        if(jsPlaylists[i]["uid"].isString() && jsPlaylists[i]["uid"].asString() == sUid)
        {
            return true;
        }
    }
    return false;
}

void Schedule::PlayItem()
{
    if(m_nCurrentItem < m_vSchedule.size())
    {
        m_pPlaylist = nullptr;
        m_pSource = nullptr;
        switch(m_vSchedule[m_nCurrentItem].eType)
        {
            case item::MP3_FILE:
                m_pSource = std::unique_ptr<FileSource>(new FileSource(m_player, m_iniConfig,
                                                                m_vSchedule[m_nCurrentItem].sUid, m_vSchedule[m_nCurrentItem].nTimesToPlay, true));
                break;
            case item::SND_FILE:
                m_pSource = std::unique_ptr<FileSource>(new FileSource(m_player, m_iniConfig,
                                                                m_vSchedule[m_nCurrentItem].sUid, m_vSchedule[m_nCurrentItem].nTimesToPlay, false));
                break;
            case item::PLAYLIST:
                m_pPlaylist = std::unique_ptr<Playlist>(new Playlist(m_player, m_iniConfig, m_vSchedule[m_nCurrentItem].sUid, m_vSchedule[m_nCurrentItem].nTimesToPlay, m_vSchedule[m_nCurrentItem].bShuffle));
        }

        m_nCurrentItem = m_vSchedule.size();

        if(m_pSource)
        {
            m_pSource->Play();
        }
        else if(m_pPlaylist)
        {
            m_pPlaylist->Play();
        }
    }
}


void Schedule::SetItem(size_t nItem)
{
    m_nCurrentItem = nItem;
    if(m_pSource)
    {
        m_pSource->Stop();
    }
    else if(m_pPlaylist)
    {
        m_pPlaylist->Stop();
    }
}
