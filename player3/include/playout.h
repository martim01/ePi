#pragma once
#include <queue>
#include "portaudio.h"
#include <list>
#include <mutex>
#include <chrono>
#include <condition_variable>

class SoundFile;

class Playout
{
    public:
        Playout(std::mutex& mut, std::condition_variable& cv, unsigned int nDevice);


        ///< Destructor
        ~Playout();

        /** Initializes portaudio
        **/
        bool Init(double dLatencey, unsigned long nSampleRate);

        void OutputCallback(float* pBuffer, size_t nFrameCount, double dLatency, int nFlags);

        size_t AddSamples(std::vector<float> vSamples);

        size_t GetBufferSize() const
        {   return m_qBuffer.size(); }

        std::string GetDeviceName() const;
        double GetOutputLatency() const;
        bool IsStreamOpen() const;
        void FlushBuffer();
        std::mutex& GetMutex() { return m_mutex;}
        std::condition_variable& GetConditionVariable() { return m_cv; }

private:

    bool OpenStream(double dLatencey, unsigned long nSampleRate, PaStreamCallback *streamCallback);

    void CloseFile();

    std::mutex& m_mutex;
    std::condition_variable& m_cv;

    PaStream* m_pStream;                ///< pointer to the PaStream tha reads in the audio

    int m_nDevice;
    unsigned int m_nChannelsOut;

    const PaDeviceInfo* m_pInfo;


    std::queue<float> m_qBuffer;


};

/** PaStreamCallback function - simply calls wmComparitor::Callback using userData to get the wmComparitor object
**/
int paCallback( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData );
//int paCallbackB( const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData );




