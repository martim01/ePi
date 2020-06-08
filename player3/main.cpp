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
#include "filesource.h"
#include "inimanager.h"
#include "playlist.h"
#include "schedule.h"
#include "resources.h"

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




static const int APP=0;
static const int CONFIG=1;
static const int TYPE=2;
static const int UID=3;
static const int LOOP=4;
static const int SHUFFLE=5;
static const int DEBUG=6;



int main(int argc, char* argv[])
{
    //make sure got all the arguments
    if(argc < DEBUG)
    {
        return ErrorAndExit("Not enough arguments sent.");
    }


    iniManager iniConfig;
    iniConfig.ReadIniFile(argv[1]);

    Resources::Get().Load(iniConfig);

    // @todo(martim01) read ini file

    //@todo(martim01) log needs to go to somewhere not cout
    if(argc == DEBUG+1)
    {
        pml::Log::Get().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));
    }

    try
    {
        int nTimesToPlay = std::stoi(argv[LOOP]);
        bool bShuffle = (argv[SHUFFLE][0] == '1');

        std::mutex mainMutex;
        std::condition_variable mainCv;
        Playout player(mainMutex, mainCv, iniConfig.GetIniInt("player3", "output",0));
        player.Init(iniConfig.GetIniDouble("player3", "latency",0.4), iniConfig.GetIniInt("player3", "samplerate", 48000));


        //launch the correct player
        switch(argv[TYPE][0])
        {
            case 'w':
                {
                    FileSource fs(player, iniConfig, argv[UID], nTimesToPlay, false);
                    fs.Play();
                }
                break;
            case 'm':
                {
                    FileSource fs(player, iniConfig, argv[UID], nTimesToPlay, true);
                    fs.Play();
                }
                break;
            case 'p':
                {
                    Playlist pl(player, iniConfig, argv[UID], nTimesToPlay, bShuffle);
                    pl.Play();
                }
                break;
            case 's':
                {
                    Schedule sl(player, iniConfig, argv[UID]);
                    sl.Play();
                }
                break;
            default:
                return ErrorAndExit("Type not recognised.");
        }

    }
    catch(const std::invalid_argument& e)
    {
        return ErrorAndExit(e.what());
    }



    //exit
    return 0;
}
