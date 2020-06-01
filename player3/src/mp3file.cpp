#include "mp3file.h"
#include "sndfile.hh"
#include <iostream>

Mp3File::Mp3File(const std::string& sPath, const std::string& sUid) : AudioFile(sPath, sUid)
{

}



Mp3File::~Mp3File()
{

}

bool Mp3File::OpenToRead()
{
    return true;
}

int64_t Mp3File::ReadAudio(std::vector<float>& vSamples)
{
    size_t nRead(0);
    return nRead;
}

bool Mp3File::Close()
{
    return true;
}

int	Mp3File::GetFormat () const
{

    return 0;
}

int	Mp3File::GetChannels () const
{


    return 0;
}

int	Mp3File::GetSampleRate () const
{

    return 0;
}


std::chrono::milliseconds Mp3File::GetLength() const
{


    return std::chrono::milliseconds(0);
}


void Mp3File::GoToStart()
{

}

