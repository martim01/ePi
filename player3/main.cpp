#include <iostream>
#include <thread>
#include <chrono>
#include "json/json.h"
#include "epiwriter.h"
#include "portaudio.h"
#include "log.h"
#include "playout.h"
#include "soundfile.h"
#include "resampler.h"
#include <thread>
#include "utils.h"
#include <mutex>
#include <condition_variable>


using namespace std;

/**
Possible types
w = file sndfile can read
m = mp3 file
p = playlist
s = schedule
**/

int ErrorAndExit(const std::string& sError)
{
    Json::Value jsMessage;
    jsMessage["error"] = sError;
    epiWriter::Get().writeToStdOut(jsMessage);
    return -1;
}



void PlaySoundFile(std::mutex& mainMutex, std::condition_variable& mainCV, Playout& player, const std::string& sFile, const std::string& sLoop)
{
    try
    {
        int nTimesToPlay = std::stoi(sLoop);

        SoundFile sf("/var/ePi/audio/", sFile);
        if(sf.OpenToRead() == false)
        {
            return;
        }
        //Init the resampler
        int nSampleRate = sf.GetSampleRate();
        Resampler resample(48000);
        if(resample.Init(sf.GetChannels(), sf.GetSampleRate()) == false)
        {
            return;
        }


        std::chrono::time_point<std::chrono::high_resolution_clock> start(std::chrono::high_resolution_clock::now());

        Json::Value jsStatus;
        jsStatus["file"]["uid"] = sFile;
        jsStatus["file"]["started_at"] = ConvertTimeToIsoString(start);
        jsStatus["file"]["length"] = sf.GetLength().count();



        std::vector<float> vRead(8192);
        std::vector<float> vResample;
        size_t nOffset = 0;
        while(nTimesToPlay != 0)
        {
            int64_t nRead = sf.ReadAudio(vRead,nOffset, nTimesToPlay);
            if(!resample.Resample(vRead, vResample))
            {
                jsStatus["error"] = true;
            }
            else
            {
                player.AddSamples(vResample);
            }
            //wait for player to say it needs more samples
            std::unique_lock<std::mutex> lck(mainMutex);
            mainCV.wait(lck, [&player]{return player.GetBufferSize()< 16384;});

            // output current playback time
            auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()-start.time_since_epoch());
            jsStatus["file"]["time"] = now.count();
            jsStatus["file"]["times_to_play"] = nTimesToPlay;
            epiWriter::Get().writeToStdOut(jsStatus);
        }

        //wait for player to finish playing...
        std::unique_lock<std::mutex> lck(mainMutex);
        mainCV.wait(lck, [&player]{return player.GetBufferSize()==0;});

        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()-start.time_since_epoch());
        jsStatus["file"]["time"] = now.count();
        epiWriter::Get().writeToStdOut(jsStatus);
    }
    catch(const std::invalid_argument& e)
    {
        return;
    }

}

int main(int argc, char* argv[])
{
    //make sure got all the arguments
    if(argc < 5)
    {
        return ErrorAndExit("Not enough arguments sent.");
    }

    //@todo(martim01) log needs to go to somewhere not cout
    pml::Log::Get().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));

    std::mutex mainMutex;
    std::condition_variable mainCv;
    Playout player(mainMutex, mainCv, 3);
    player.Init(0.4);

    //debug stuff
    int nDevices = Pa_GetDeviceCount();
    for(int i = 0; i < nDevices; i++)
    {
        const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(i);
        if(pInfo)
        {
            pml::Log::Get() << "device: " << i << "=" << pInfo->name << std::endl;
        }
    }

    //launch the correct player
    switch(argv[1][0])
    {
        case 'w':
            PlaySoundFile(mainMutex, mainCv, player, argv[2], argv[3]);
            break;
        case 'm':
            break;
        case 'p':
            break;
        case 's':
            break;
        default:
            return ErrorAndExit("Type not recognised.");
    }

    //exit
    return 0;
}
