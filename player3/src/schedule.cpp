#include "schedule.h"
#include <algorithm>
#include <random>
#include "filesource.h"
#include <fstream>
#include "inimanager.h"
#include "utils.h"
#include "log.h"
#include <thread>
#include "epiwriter.h"

Schedule::Schedule(Playout& player, const iniManager& iniConfig, const std::string& sUid) :
  m_player(player),
  m_iniConfig(iniConfig),
  m_sUid(sUid),
  m_pSource(nullptr)
{

}

bool Schedule::Play()
{
    if(!LoadSchedule())
    {
        return false;
    }

    m_nCurrentItem = m_vSchedule.size();

    //launch thread to do the actual schedule...

    std::thread th([this]() {
        while(true)
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
    th.detach();


    while(true)
    {
        if(m_nCurrentItem < m_vSchedule.size())
        {
            PlayItem();
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
    std::ifstream ifs;
    ifs.open(CreatePath(m_iniConfig.GetIniString("Paths", "Resources", "/var/ePi/resources"))+"resources.json", std::ifstream::in);
    if(!ifs.is_open())
    {
        pml::Log::Get(pml::Log::LOG_CRITICAL) << "Schedule\tCould not open resources file!" << std::endl;
		return false;
	}
	try
	{
        Json::Value jsResources;
        ifs >> jsResources;

        if(jsResources["schedules"].isArray() && jsResources["files"].isArray())
        {
            for(size_t i = 0; i < jsResources["schedules"].size(); i++)
            {
                if(jsResources["schedules"][i]["uid"].asString() == m_sUid)
                {
                    return CreateSchedule(jsResources["files"], jsResources["schedules"][i]);
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

bool Schedule::CreateSchedule(const Json::Value& jsFiles, const Json::Value& jsSchedule)
{
    if(jsSchedule["files"].isArray() == false)
    {
        return false;
    }

    m_vSchedule.reserve(jsSchedule["files"].size());

    for(size_t i = 0; i < jsSchedule["files"].size(); i++)
    {
        if(jsSchedule["files"][i]["uid"].isString() && jsSchedule["files"][i]["times_to_play"].isInt() && jsSchedule["files"][i]["cron"].isString())
        {
            switch(GetFile(jsFiles, jsSchedule["files"][i]["uid"].asString()))
            {
                case FILE_SOUND:
                    m_vSchedule.push_back(item(jsSchedule["files"][i]["uid"].asString(), jsSchedule["files"][i]["times_to_play"].asInt(), false));
                    m_vSchedule.back().job.SetString(jsSchedule["files"][i]["cron"].asString());
                    break;
                case FILE_MP3:
                    m_vSchedule.push_back(item(jsSchedule["files"][i]["uid"].asString(), jsSchedule["files"][i]["times_to_play"].asInt(), true));
                    m_vSchedule.back().job.SetString(jsSchedule["files"][i]["cron"].asString());
                    break;
            }
        }
    }
    return true;
}

int Schedule::GetFile(const Json::Value& jsFiles, const std::string& sUid)
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


void Schedule::PlayItem()
{
    if(m_nCurrentItem < m_vSchedule.size())
    {
        m_pSource = std::unique_ptr<FileSource>(new FileSource(m_player, CreatePath(m_iniConfig.GetIniString("Paths", "Audio", "/var/ePi/audio")),
                                                                m_vSchedule[m_nCurrentItem].sUid, m_vSchedule[m_nCurrentItem].nTimesToPlay, m_vSchedule[m_nCurrentItem].bMp3));
        m_nCurrentItem = m_vSchedule.size();
        m_pSource->Play();
    }
}


void Schedule::SetItem(size_t nItem)
{
    m_nCurrentItem = nItem;
    if(m_pSource)
    {
        m_pSource->Stop();
    }
}
