
#include "wavfile.h"
#include "sndfile.hh"


WavFile::WavFile(const std::string& sUid, const std::string& sLabel, const std::string& sDescription) :
    AudioFile("wavfile", sUid, sLabel, sDescription),
    m_pHandle(nullptr)
{

}



WavFile::WavFile() : AudioFile(std::string("wavfile"))
{

}

WavFile::WavFile(const Json::Value& jsData) : AudioFile(jsData)
{


}

WavFile::~WavFile()
{

}

int WavFile::IsWavFile(const std::string& sFile)
{
    SndfileHandle sf(sFile, SFM_READ);
    return sf.error();
}


bool WavFile::OpenToRead()
{
    if(m_pHandle)
	{
		Close();
	}
    m_pHandle = std::unique_ptr<SndfileHandle>(new SndfileHandle(m_json["label"].asString(), SFM_READ, 0,0,0)); //todo need path
    return (m_pHandle!=nullptr);
}

int64_t WavFile::ReadAudio(float* pBuffer, int nSize, unsigned int nLoop)
{

    int64_t nRead(0);
    if(m_pHandle)
    {
        nRead = m_pHandle->read(pBuffer, nSize);
        if(nLoop  != 0)
        {
            while(nRead < nSize)
            {
                int nPointer = nRead;
                nSize -= nRead;

                m_pHandle->seek(SEEK_SET, 0);
                nRead = m_pHandle->read(&pBuffer[nPointer], nSize);
            }
        }
    }
    return nRead;
}

bool WavFile::Close()
{
    m_pHandle = nullptr;
    return true;
}

int	WavFile::GetFormat () const
{
    if(m_pHandle)
    {
        return m_pHandle->format();
    }

    return 0;
}

int	WavFile::GetChannels () const
{

    if(m_pHandle)
    {
        return m_pHandle->channels();
    }

    return 0;
}

int	WavFile::GetSampleRate () const
{
    if(m_pHandle)
    {
        return m_pHandle->samplerate();
    }

    return 0;
}


std::chrono::milliseconds WavFile::GetLength() const
{
    if(m_pHandle)
    {
        auto nFrames = m_pHandle->seek(0, SEEK_END);
        nFrames *= GetSampleRate();
        m_pHandle->seek(0,SEEK_SET);
        return std::chrono::milliseconds(nFrames);
    }

    return std::chrono::milliseconds(0);
}



Json::Value WavFile::GetFormatAsJson()
{

    Json::Value jsValue;
    int nFormat = GetFormat();
    switch(nFormat & SF_FORMAT_TYPEMASK)
    {
        case SF_FORMAT_WAV:
            jsValue["type"] = "wav";
            break;
        case SF_FORMAT_AIFF:
            jsValue["type"] = "Apple/SGI AIFF";
            break;
        case SF_FORMAT_AU:
            jsValue["type"] = "Sun/NeXT AU";
            break;
        case SF_FORMAT_RAW:
            jsValue["type"] = "RAW PCM";
            break;
        case SF_FORMAT_PAF:
            jsValue["type"] = "Ensoniq PARIS file";
            break;
        case SF_FORMAT_SVX:
            jsValue["type"] = "Amiga IFF / SVX8 / SV16";
            break;
        case SF_FORMAT_NIST:
            jsValue["type"] = "Sphere NIST";
            break;
        case SF_FORMAT_VOC:
            jsValue["type"] = "VOC";
            break;
        case SF_FORMAT_IRCAM:
            jsValue["type"] = "Berkeley/IRCAM/CARL";
            break;
        case SF_FORMAT_W64:
            jsValue["type"] = "Sonic Foundry's 64 bit RIFF/WAV";
            break;
        case SF_FORMAT_MAT4:
            jsValue["type"] = "Matlab (tm) V4.2 / GNU Octave 2.0";
            break;
        case SF_FORMAT_MAT5:
            jsValue["type"] = "Matlab (tm) V5.0 / GNU Octave 2.1";
            break;
        case SF_FORMAT_PVF:
            jsValue["type"] = "Portable Voice Format";
            break;
        case SF_FORMAT_XI:
            jsValue["type"] = "Fasttracker 2 Extended Instrument";
            break;
        case SF_FORMAT_HTK:
            jsValue["type"] = "HMM Tool Kit format";
            break;
        case SF_FORMAT_SDS:
            jsValue["type"] = "Midi Sample Dump Standard";
            break;
        case SF_FORMAT_AVR:
            jsValue["type"] = "Audio Visual Research";
            break;
        case SF_FORMAT_WAVEX:
            jsValue["type"] = "MS WAVE with WAVEFORMATEX";
            break;
        case SF_FORMAT_SD2:
            jsValue["type"] = "Sound Designer 2";
            break;
        case SF_FORMAT_FLAC:
            jsValue["type"] = "FLAC";
            break;
        case SF_FORMAT_CAF:
            jsValue["type"] = "Core Audio File";
            break;
        case SF_FORMAT_WVE:
            jsValue["type"] = "Psion WVE";
            break;
        case SF_FORMAT_OGG:
            jsValue["type"] = "Xiph OGG container";
            break;
        case SF_FORMAT_MPC2K:
            jsValue["type"] = "Akai MPC 2000 sampler";
            break;
        case SF_FORMAT_RF64:
            jsValue["type"] = "RF64 WAV file ";
            break;
    }

    switch(nFormat & SF_FORMAT_SUBMASK)
	{

        case SF_FORMAT_PCM_S8:
            jsValue["subtype"] = "Signed 8 bit data";
            break;
        case SF_FORMAT_PCM_16:
            jsValue["subtype"] = "Signed 16 bit data";
            break;
        case SF_FORMAT_PCM_24:
            jsValue["subtype"] = "Signed 24 bit data";
            break;
        case SF_FORMAT_PCM_32:
            jsValue["subtype"] = "Signed 32 bit data";
            break;
        case SF_FORMAT_PCM_U8:
            jsValue["subtype"] = "Unsigned 8 bit data (WAV and RAW only)";
            break;
        case SF_FORMAT_FLOAT:
            jsValue["subtype"] = "32 bit float data";
            break;
        case SF_FORMAT_DOUBLE:
            jsValue["subtype"] = "64 bit float data";
            break;
        case SF_FORMAT_ULAW:
            jsValue["subtype"] = "U-Law encoded.";
            break;
        case SF_FORMAT_ALAW:
            jsValue["subtype"] = "A-Law encoded.";
            break;
        case SF_FORMAT_IMA_ADPCM:
            jsValue["subtype"] = "IMA ADPCM.";
            break;
        case SF_FORMAT_MS_ADPCM:
            jsValue["subtype"] = "Microsoft ADPCM.";
            break;
        case SF_FORMAT_GSM610:
            jsValue["subtype"] = "GSM 6.10 encoding.";
            break;
        case SF_FORMAT_VOX_ADPCM:
            jsValue["subtype"] = "OKI / Dialogix ADPCM";
            break;
        case SF_FORMAT_G721_32:
            jsValue["subtype"] = "32kbs G721 ADPCM encoding.";
            break;
        case SF_FORMAT_G723_24:
            jsValue["subtype"] = "24kbs G723 ADPCM encoding.";
            break;
        case SF_FORMAT_G723_40:
            jsValue["subtype"] = "40kbs G723 ADPCM encoding.";
            break;
        case SF_FORMAT_DWVW_12:
            jsValue["subtype"] = "12 bit Delta Width Variable Word encoding.";
            break;
        case SF_FORMAT_DWVW_16:
            jsValue["subtype"] = "16 bit Delta Width Variable Word encoding.";
            break;
        case SF_FORMAT_DWVW_24:
            jsValue["subtype"] = "24 bit Delta Width Variable Word encoding.";
            break;
        case SF_FORMAT_DWVW_N:
            jsValue["subtype"] = "N bit Delta Width Variable Word encoding.";
            break;
        case SF_FORMAT_DPCM_8:
            jsValue["subtype"] = "8 bit differential PCM (XI only)";
            break;
        case SF_FORMAT_DPCM_16:
            jsValue["subtype"] = "16 bit differential PCM (XI only)";
            break;
        case SF_FORMAT_VORBIS:
            jsValue["subtype"] = "Xiph Vorbis encoding.";
            break;
        case SF_FORMAT_ALAC_16:
            jsValue["subtype"] = "Apple Lossless Audio Codec (16 bit).";
            break;
        case SF_FORMAT_ALAC_20:
            jsValue["subtype"] = "Apple Lossless Audio Codec (20 bit).";
            break;
        case SF_FORMAT_ALAC_24:
            jsValue["subtype"] = "Apple Lossless Audio Codec (24 bit).";
            break;
        case SF_FORMAT_ALAC_32:
            jsValue["subtype"] = "Apple Lossless Audio Codec (32 bit).";
            break;
    }

	switch(nFormat & SF_FORMAT_ENDMASK)
    {
        case SF_ENDIAN_FILE:
            jsValue["enianness"]="Default file endian-ness.";
            break;
        case SF_ENDIAN_LITTLE:
            jsValue["enianness"]="Little endian-ness.";
            break;
        case SF_ENDIAN_BIG:
            jsValue["enianness"]="Big endian-ness.";
            break;
        case SF_ENDIAN_CPU:
            jsValue["enianness"]="CPU endian-ness.";
            break;
    }

	return jsValue;
}
