# ePi
Raspberry Pi Emergency Audio Material and Loop Player

## Introduction
This is a project to create a broadcast standard audio loop player that can also be used as an emergency material player for radio transmission chains.

The project will consist of a number of applications

### Server
A RESTFUL server that allows the user to see the status of the player, upload audio files, schedules and playlists and control playback.

### Player3
The application that will play out the audio files through a soundcard (as an AES3 stream). It will be able to play a single file, a playlist of files one after the other or a schedule of files based on a "cron" type schedule.

### Player67
The same as Player3 but will produce an AES67 bitstream.

### Controller
An application designed for a Raspberry Pi 7" touchscreen that will allow full control of the ePi via the Server API.
