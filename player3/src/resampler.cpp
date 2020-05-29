#include "resampler.h"

Resampler::Resampler(unsigned int nOutputRate) :
    m_dOutputRate(nOutputRate),
    m_pState(nullptr)
{
}

Resampler::~Resampler()
{
    DeleteState();
}


void Resampler::DeleteState()
{
    if(m_pState)
    {
        src_delete(m_pState);
        m_pState = 0;
    }
}

bool Resampler::Init(int nChannels, unsigned int nInputRate)
{
    DeleteState();

    int nError(0);
    m_pState = src_new (SRC_SINC_BEST_QUALITY, nChannels, &nError);
    if(nError == 0)
    {
         return SetInputRate(nInputRate);
    }
    return false;
}


bool Resampler::SetInputRate(unsigned int nInputRate)
{
    if(!m_pState)
    {
        return false;
    }

    src_reset(m_pState);
    m_dRatio = m_dOutputRate/static_cast<double>(nInputRate);
    return (src_set_ratio(m_pState, m_dRatio)==0);
}

bool Resampler::Resample(const std::vector<float>& vInput, std::vector<float>& vOutput)
{
    if(!m_pState)
    {
        return false;
    }

    vOutput.resize(vInput.size() * m_dRatio*2);

    m_data.data_out = vOutput.data();
    m_data.data_in = vInput.data();
    m_data.input_frames	= vInput.size() / 2;    //@todo(martim1) different channels
    m_data.output_frames	= m_data.input_frames*m_dRatio*2;
    m_data.src_ratio = m_dRatio;
    m_data.end_of_input = 0;

    if(src_process(m_pState, &m_data) != 0)
    { //error
        vOutput.clear();
        return false;
    }
    else
    {//Now filter
        vOutput.resize(m_data.output_frames_gen*2);     //@todo(martim1) different channels
        return true;
    }
}
