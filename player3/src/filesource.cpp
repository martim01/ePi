#include "filesource.h"
#include "playout.h"
#include "resampler.h"
#include "mp3file.h"
#include "soundfile.h"
#include "epiwriter.h"
#include "utils.h"
#include "inimanager.h"
#include "log.h"


FileSource::FileSource(Playout& player, const iniManager& iniConfig, const std::string& sUid, unsigned long nTimesToPlay, bool bMp3) :
    m_player(player),
    m_iniConfig(iniConfig),
    m_sUid(sUid),
    m_nTimesToPlay(nTimesToPlay),
    m_pFile(bMp3 ? std::unique_ptr<AudioFile>(new Mp3File(CreatePath(m_iniConfig.GetIniString("paths", "audio", "/var/ePi/audio")), m_sUid)) :
                   std::unique_ptr<AudioFile>(new SoundFile(CreatePath(m_iniConfig.GetIniString("paths", "audio", "/var/ePi/audio")), m_sUid))),
    m_pSampler(nullptr),
    m_bPlay(true)
{
    m_jsStatus["action"] = "Playing";
    m_jsStatus["playing"]["uid"] = m_sUid;
    m_jsStatus["playing"]["path"] = m_pFile->GetFileName();
    m_jsStatus["time"] = GetCurrentTimeAsIsoString();
    epiWriter::Get().writeToStdOut(m_jsStatus);
}

FileSource::~FileSource()
{
}

void FileSource::Stop()
{
    m_bPlay = false;
}

bool FileSource::Play()
{
    if(m_pFile->OpenToRead())
    {
        m_start = std::chrono::system_clock::now();

        m_jsStatus["playing"]["started_at"] = ConvertTimeToIsoString(m_start);
        m_jsStatus["playing"]["length"] = m_pFile->GetLength().count();

        //Init the resampler
        int nSampleRate(m_iniConfig.GetIniInt("player3", "samplerate", 48000));
        if(m_pFile->GetSampleRate() != nSampleRate)
        {
            m_pSampler = std::unique_ptr<Resampler>(new Resampler(nSampleRate));
            if(m_pSampler->Init(m_pFile->GetChannels(), m_pFile->GetSampleRate()) == false)
            {
                m_jsStatus["playing"]["error"] = true;
                m_jsStatus["playing"]["error_reason"] = "Can't init resampler";
                epiWriter::Get().writeToStdOut(m_jsStatus);
                return false;
            }
            m_jsStatus["playing"]["samplerate"]["out"] = nSampleRate;
            m_jsStatus["playing"]["samplerate"]["in"] = m_pFile->GetSampleRate();
            m_jsStatus["playing"]["resampler"] = true;
        }
        else
        {
            m_jsStatus["playing"]["resampler"] = false;
        }
        for(unsigned long nLoop = 1; nLoop <= m_nTimesToPlay && m_bPlay; nLoop++)
        {
            m_jsStatus["playing"]["loop"] = Json::UInt(nLoop);
            PlayOnce();
        }
        return true;
    }
    else
    {
        m_jsStatus["playing"]["error"] = true;
        m_jsStatus["playing"]["error_reason"] = "Can't open file";
        epiWriter::Get().writeToStdOut(m_jsStatus);
        return false;
    }

}

bool FileSource::PlayOnce()
{
    std::cout << "PlayOnce" << std::endl;
    m_pFile->GoToStart();


    std::chrono::time_point<std::chrono::system_clock> loopStart(std::chrono::system_clock::now());


    std::vector<float> vRead(BUFFER_SIZE);
    std::vector<float> vResample;
    while((vRead.size() == BUFFER_SIZE) && m_bPlay)
    {
        while(vRead.size() == BUFFER_SIZE && m_player.GetBufferSize() < BUFFER_QUEUE_MAX  && m_bPlay)
        {
            vRead.resize(BUFFER_SIZE);
            m_pFile->ReadAudio(vRead);

            if(m_pSampler != nullptr)
            {
                m_jsStatus["playing"]["resampler"] = true;
                if(!m_pSampler->Resample(vRead, vResample))
                {
                    m_jsStatus["playing"]["error"] = true;
                    m_jsStatus["playing"]["error_reason"] = "Can't resample";
                    epiWriter::Get().writeToStdOut(m_jsStatus);
                    return false;
                }
                else
                {
                    m_player.AddSamples(vResample);
                }
            }
            else
            {

                m_player.AddSamples(vRead);
            }
        }
        std::cout << "PlayOnce: Wait" << std::endl;

        //wait for player to say it needs more samples
        std::unique_lock<std::mutex> lck(m_player.GetMutex());
        m_player.GetConditionVariable().wait(lck, [this]{return m_player.GetBufferSize()< BUFFER_QUEUE_MIN;});

        // output current playback time
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()-loopStart.time_since_epoch());
        m_jsStatus["playing"]["time"] = now.count();
        m_jsStatus["time"] = GetCurrentTimeAsIsoString();

        epiWriter::Get().writeToStdOut(m_jsStatus);
    }

    //wait for player to finish playing...
    std::unique_lock<std::mutex> lck(m_player.GetMutex());
    m_player.GetConditionVariable().wait(lck, [this]{return m_player.GetBufferSize()==0;});

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()-loopStart.time_since_epoch());
    m_jsStatus["playing"]["time"] = now.count();
    epiWriter::Get().writeToStdOut(m_jsStatus);

    return true;
}
