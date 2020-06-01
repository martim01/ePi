#include "filesource.h"
#include "playout.h"
#include "resampler.h"
#include "mp3file.h"
#include "soundfile.h"
#include "epiwriter.h"
#include "utils.h"


FileSource::FileSource(Playout& player, const std::string& sPath, const std::string& sUid, unsigned long nTimesToPlay, bool bMp3) :
    m_player(player),
    m_sPath(sPath),
    m_sUid(sUid),
    m_nTimesToPlay(nTimesToPlay),
    m_pFile(bMp3 ? std::unique_ptr<AudioFile>(new Mp3File(sPath, m_sUid)) : std::unique_ptr<AudioFile>(new SoundFile(sPath, m_sUid))),
    m_pSampler(nullptr)
{
    m_jsStatus["file"]["uid"] = m_sUid;
    m_jsStatus["file"]["path"] = m_pFile->GetFileName();
    epiWriter::Get().writeToStdOut(m_jsStatus);
}

FileSource::~FileSource()
{
}

bool FileSource::Play()
{
    if(m_pFile->OpenToRead())
    {
        m_start = std::chrono::high_resolution_clock::now();

        m_jsStatus["file"]["started_at"] = ConvertTimeToIsoString(m_start);
        m_jsStatus["file"]["length"] = m_pFile->GetLength().count();

        //Init the resampler
        if(m_pFile->GetSampleRate() != 48000)
        {
            m_pSampler = std::unique_ptr<Resampler>(new Resampler(48000));
            if(m_pSampler->Init(m_pFile->GetChannels(), m_pFile->GetSampleRate()) == false)
            {
                m_jsStatus["file"]["error"] = true;
                m_jsStatus["file"]["error_reason"] = "Can't init resampler";
                epiWriter::Get().writeToStdOut(m_jsStatus);
                return false;
            }
        }

        for(unsigned long nLoop = 1; nLoop <= m_nTimesToPlay; nLoop++)
        {
            m_jsStatus["file"]["loop"] = Json::UInt(nLoop);
            PlayOnce();
        }
        return true;
    }
    else
    {
        m_jsStatus["file"]["error"] = true;
        m_jsStatus["file"]["error_reason"] = "Can't open file";
        epiWriter::Get().writeToStdOut(m_jsStatus);
        return false;
    }

}

bool FileSource::PlayOnce()
{
    m_pFile->GoToStart();


    std::chrono::time_point<std::chrono::high_resolution_clock> loopStart(std::chrono::high_resolution_clock::now());


    std::vector<float> vRead(BUFFER_SIZE);
    std::vector<float> vResample;
    while((vRead.size() == BUFFER_SIZE))
    {
        while(vRead.size() == BUFFER_SIZE && m_player.GetBufferSize() < BUFFER_QUEUE_MAX)
        {
            vRead.resize(BUFFER_SIZE);
            m_pFile->ReadAudio(vRead);

            if(m_pSampler != nullptr)
            {
                if(!m_pSampler->Resample(vRead, vResample))
                {
                    m_jsStatus["file"]["error"] = true;
                    m_jsStatus["file"]["error_reason"] = "Can't resample";
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

        //wait for player to say it needs more samples
        std::unique_lock<std::mutex> lck(m_player.GetMutex());
        m_player.GetConditionVariable().wait(lck, [this]{return m_player.GetBufferSize()< BUFFER_QUEUE_MIN;});

        // output current playback time
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()-loopStart.time_since_epoch());
        m_jsStatus["file"]["time"] = now.count();

        epiWriter::Get().writeToStdOut(m_jsStatus);
    }

    //wait for player to finish playing...
    std::unique_lock<std::mutex> lck(m_player.GetMutex());
    m_player.GetConditionVariable().wait(lck, [this]{return m_player.GetBufferSize()==0;});

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()-loopStart.time_since_epoch());
    m_jsStatus["file"]["time"] = now.count();
    epiWriter::Get().writeToStdOut(m_jsStatus);

    return true;
}
