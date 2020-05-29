#pragma once
#include "samplerate.h"
#include <vector>
#include <memory>

class Resampler
{
    public:
        Resampler(unsigned int nOutputRate);
        ~Resampler();

        bool Init(int nChannels, unsigned int nInputRate);
        bool SetInputRate(unsigned int nInputRate);
        bool Resample(const std::vector<float>& vInput, std::vector<float>& vOutput);


    private:

        void DeleteState();

        double m_dOutputRate;
        double m_dRatio;
        SRC_STATE* m_pState;
        SRC_DATA m_data;
};
