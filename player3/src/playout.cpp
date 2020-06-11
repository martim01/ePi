#include "playout.h"
#include "log.h"

Playout::Playout(std::mutex& mut, std::condition_variable& cv, unsigned int nDevice) :
m_mutex(mut),
m_cv(cv),
m_pStream(nullptr),
m_nDevice(nDevice),
m_nChannelsOut(2)
{

}




bool Playout::Init(double dLatency, unsigned long nSampleRate)
{
    PaError err = Pa_Initialize();
    if(err != paNoError)
    {
        return false;
    }

    return OpenStream(dLatency, nSampleRate, paCallback);
}

bool Playout::OpenStream(double dLatency, unsigned long nSampleRate, PaStreamCallback *streamCallback)
{
    pml::Log::Get() << "Audio\tAttempt to open device " << m_nDevice << std::endl;


    PaStreamParameters outputParameters;

    const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(m_nDevice);
    if(pInfo)
    {
        if(pInfo->maxOutputChannels < 2)
        {
            m_nChannelsOut = pInfo->maxInputChannels;
            pml::Log::Get() << "Audio\tOutput channels changed to " << m_nChannelsOut << std::endl;
        }
        else
        {
            m_nChannelsOut = 2;
        }
    }


    outputParameters.channelCount = m_nChannelsOut;
    outputParameters.device = m_nDevice;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = dLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    PaError err;

    pml::Log::Get() << "Audio\tAttempt to open " << m_nChannelsOut << " channel OUTPUT stream on device " <<  m_nDevice << std::endl;

    err = Pa_OpenStream(&m_pStream, 0, &outputParameters, nSampleRate, 0, paNoFlag, streamCallback, reinterpret_cast<void*>(this) );

    if(err == paNoError)
    {
        err = Pa_StartStream(m_pStream);
        if(err == paNoError)
        {
//            PaAlsa_EnableRealtimeScheduling(m_pStream,1);
            const PaStreamInfo* pStreamInfo = Pa_GetStreamInfo(m_pStream);
            if(pStreamInfo)
            {
                pml::Log::Get() << "Audio\tStreamInfo:  Output Latency " << pStreamInfo->outputLatency << " Sample Rate " << pStreamInfo->sampleRate << std::endl;
            }
            else
            {
                pml::Log::Get(pml::Log::LOG_ERROR) << "Audio\tOpened but no stream info." << std::endl;
                return false;
            }
            return true;
        }
    }
    m_pStream = 0;
    pml::Log::Get(pml::Log::LOG_ERROR) << "Audio\tFailed to open device " << m_nDevice << " " << Pa_GetErrorText(err)
                                       << " with sample rate="<<nSampleRate <<" and output channels=" << m_nChannelsOut << std::endl;



    return false;
}


Playout::~Playout()
{
    if(m_pStream)
    {
        PaError err = Pa_AbortStream(m_pStream);
        err = Pa_CloseStream(m_pStream);
        if(err != paNoError)
        {
            pml::Log::Get(pml::Log::LOG_ERROR) << "Audio\tFailed to stop PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        }
    }
    Pa_Terminate();
}


void Playout::OutputCallback(float* pBuffer, size_t nFrameCount, double dPlayoutLatency, int nFlags)
{

    std::lock_guard<std::mutex> lg(m_mutex);
    bool bUnderrun(false);
    for(size_t i = 0; i < nFrameCount*m_nChannelsOut; i++)
    {
        if(m_qBuffer.empty() == false)
        {
            pBuffer[i] = m_qBuffer.front();
            m_qBuffer.pop();
        }
        else
        {
            bUnderrun = true;
            pBuffer[i] = 0.0;
        }
    }

    if(bUnderrun)
    {
        pml::Log::Get(pml::Log::LOG_WARN) << "Buffer underrun!" << std::endl;
    }

    if(m_qBuffer.size() < 16384)
    {
        m_cv.notify_one();
    }
}




int paCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData )
{

    if(userData)
    {
        Playout* pComp = reinterpret_cast<Playout*>(userData);
        pComp->OutputCallback(reinterpret_cast<float*>(output), frameCount,timeInfo->outputBufferDacTime-timeInfo->currentTime, statusFlags);
    }

    return 0;
}



std::string Playout::GetDeviceName() const
{
    const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(m_nDevice);
    if(pInfo)
    {
        return pInfo->name;
    }
    return "";
}


double Playout::GetOutputLatency() const
{
    if(m_pStream)
    {
        const PaStreamInfo* pInfo = Pa_GetStreamInfo(m_pStream);
        if(pInfo)
        {
            return pInfo->outputLatency;
        }
    }

    return -1;
}


size_t Playout::AddSamples(std::vector<float> vSamples)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    for(size_t i = 0; i < vSamples.size(); i++)
    {

         m_qBuffer.push(vSamples[i]);
    }
    return m_qBuffer.size();
}


bool Playout::IsStreamOpen() const
{
    return (m_pStream && Pa_IsStreamActive(m_pStream));
}



void Playout::FlushBuffer()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    while(m_qBuffer.empty() == false)
    {
        m_qBuffer.pop();
    }
}

