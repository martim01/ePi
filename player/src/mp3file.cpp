#include "mp3file.h"

Mp3File::Mp3File(const std::string& sUid, const std::string& sLabel, const std::string& sDescription) :
    AudioFile("Mp3file", sUid, sLabel, sDescription)
{

}



Mp3File::Mp3File() : AudioFile(std::string("Mp3file"))
{

}

Mp3File::~Mp3File()
{

}


bool Mp3File::OpenToRead()
{
    return false;
}

int64_t Mp3File::ReadAudio(float* pBuffer, int nSize, unsigned int nLoop)
{
    int64_t nRead(0);

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



Json::Value Mp3File::GetFormatAsJson()
{

    Json::Value jsValue;

	return jsValue;
}
