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
static const int TYPE=1;
static const int UID=2;
static const int LOOP=3;
static const int SHUFFLE=4;



int main(int argc, char* argv[])
{
    //make sure got all the arguments
    if(argc < 5)
    {
        return ErrorAndExit("Not enough arguments sent.");
    }


    iniManager iniConfig;
    // @todo(martim01) read ini file

    //@todo(martim01) log needs to go to somewhere not cout
    //pml::Log::Get().AddOutput(std::unique_ptr<pml::LogOutput>(new pml::LogOutput()));

    try
    {
        int nTimesToPlay = std::stoi(argv[LOOP]);

        std::mutex mainMutex;
        std::condition_variable mainCv;
        Playout player(mainMutex, mainCv, 0);
        player.Init(0.4);


        //launch the correct player
        switch(argv[TYPE][0])
        {
            case 'w':
                {
                    FileSource fs(player, CreatePath(iniConfig.GetIniString("Paths", "Audio", "/var/ePi/audio")), argv[UID], nTimesToPlay, false);
                    fs.Play();
                }
                break;
            case 'm':
                {
                    FileSource fs(player, CreatePath(iniConfig.GetIniString("Paths", "Audio", "/var/ePi/audio")), argv[UID], nTimesToPlay, true);
                    fs.Play();
                }
                break;
            case 'p':
                break;
            case 's':
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
