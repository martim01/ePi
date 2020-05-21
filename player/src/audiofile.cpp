#include "audiofile.h"

AudioFile::AudioFile(const std::string& sType, const std::string& sUid, const std::string& sLabel, const std::string& sDescription) :
Resource(sType, sUid, sLabel, sDescription)
{
}

AudioFile::AudioFile(const std::string& sType) : Resource(sType)
{

}

AudioFile::AudioFile(const Json::Value& jsData) : Resource(jsData)
{
}


void AudioFile::InitJson()
{
    if(OpenToRead())
    {
        m_json["format"] = GetFormatAsJson();
        m_json["samplerate"] = GetSampleRate();
        m_json["length"] = GetLength().count();
        m_json["channels"] = GetChannels();
        m_json["valid"] = true;
    }
    else
    {
        m_json["valid"] = false;
    }
}

