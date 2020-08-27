# ePi

[![Code Grade](https://www.code-inspector.com/project/11011/score/svg)](https://www.code-inspector.com)
[![Code Grade](https://www.code-inspector.com/project/11011/status/svg)](https://www.code-inspector.com)

Raspberry Pi Emergency Audio Material and Loop Player

## Introduction
This is a project to create a broadcast standard audio loop player that can also be used as an emergency material player for radio transmission chains.

The project consists of a number of applications

### episerver
A RESTFUL server that allows the user to see the status of the player, upload audio files, schedules and playlists and control playback.

### player3
The application that plays out the audio files through a soundcard. It is able to play a single file, a playlist of files one after the other or a schedule of files/playlists based on a "cron" type schedule.

### player67
The same as Player3 but produces an AES67 bitstream. (Not yet written)

### controller
An application designed for a Raspberry Pi 7" touchscreen that allows control of the ePi via the Server API. Designed for using the ePi as an emergency playout machine.

### launcher
A simple application that reads and ini file and launches 1 or more controllers and restarts them should they stop.

### loopi
A set of webpages written using PHP and javascript that allows full control of the episerver. Designed for using the ePi as a loop/barker player.
## Prerequisites

### Core libraries required for the Server and Player3

* [PortAudio](http://www.portaudio.com/download.html)
* [libsndfile](http://www.mega-nerd.com/libsndfile/)
* [Secret Rabit Code](http://www.mega-nerd.com/SRC/)
* OpenSSL

##### To install these libraries on Raspbian
```
sudo apt-get update
sudo apt-get install portaudio19-dev libsndfile1-dev libsamplerate0-dev libssl-dev
```

### Core libraries needed for the Controller and Launcher

* [wxWidgets](www.wxwidgets.org)
* libcap-dev

##### To install these libraries on Raspbian
```
sudo apt-get update
sudo apt-get install libcap-dev libwxgtk3.0-dev
```

### Other source code included in the project

* [Mongoose](https://github.com/cesanta/mongoose) - source code included in the Server project
* [JsonCPP](https://github.com/open-source-parsers/jsoncpp) - included as a Git submodule
* [pml_log](https://github.com/martim01/log) - included as a Git submodule


## Building ePi

* Workspace and project files are supplied for [Code::Blocks IDE](http://www.codeblocks.org/). There are Debug and Release builds for Linux.
* There are also CMakeLists.txt files.

### To build in Code::Blocks
* Make sure all the Git submodules are up to date
* Open epi.workspace in Code::Blocks
* Build the workspace.
* All the applications will end up in ```{epi directory}/bin```

### To build using CMake
```
mkdir {epi directory}/build
cd {epi directory}/build
cmake ..
cmake --build .
```
This will initialise and update the Git submodules and build all the applications - they will end up in ```{epi directory}/bin```
To build only the controller and launcher
```
cmake -DBUILD_SERVER=OFF ..
```
To build only the episerver and players
```
cmake -DBUILD_CONTROLLER=OFF ..
```


## Creating an ePi Server
Instructions can be found in the [wiki](https://github.com/martim01/ePi/wiki/building)
