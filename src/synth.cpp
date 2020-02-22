/**
 * @file synth.cpp
 */
#include <iostream>
#include <thread>

#include <math.h>

#include "audio.h"
#include "synth.h"
#include "waveform.h"


static double synth_signal_callback( void* userdata );

Synth* Synth::instance_ = nullptr;


Synth* Synth::Create( float tuning )
{
    if (!instance_)
    {
        instance_ = new Synth;
        instance_->Initialize( tuning);
    }
    return instance_;
}

void Synth::Destroy()
{
    AudioCtrl* audioctrl = AudioCtrl::GetInstance();
    audioctrl->SignalCallbackUnset();

    delete instance_;
    instance_ = nullptr;
}

Synth* Synth::GetInstance()
{
    return instance_;
}

/**
 * @brief initialize class
 */
void Synth::Initialize( float tuning )
{
    AudioCtrl* audioctrl = AudioCtrl::GetInstance();

    // init param
    tuning_ = tuning;
    fs_     = audioctrl->SampleRateGet();

    // create wave form
    Waveform* wf = Waveform::Create( tuning_, fs_ );

    // set audio callback
    audioctrl->SignalCallbackSet( synth_signal_callback, this );

    // create VCO
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief NoteOn
 */
void Synth::NoteOn( int notenum, int velocity )
{
    return;
}

/**
 * @brief NoteOff
 */
void Synth::NoteOff( int notenum, int velocity )
{
    return;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief synth_signal_callback
 */
static double synth_signal_callback( void* userdata )
{
    Synth* synth = (Synth*)userdata;
    Waveform* wf = Waveform::GetInstance();

    uint32_t w = wf->CalcWFromFreq( synth->tuning_ );
    double val = synth->voice[0].Calc( w );

    return val;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 信号処理部
 */
float Voice::Calc( uint32_t w )
{
    float val;
    val = vco.Calc(w);
    val = vcf.Calc(val);
    val = vca.Calc(val);
    return val;
}

///////////////////////////////////////////////////////////////////////////////

Voice::VCO::VCO()
{
    p_ = 0;
}

/**
 * @brief 波形生成
 *
 * @param[in] w 角速度
 */
float Voice::VCO::Calc( uint32_t w )
{
    Waveform* wf = Waveform::GetInstance();
    float val = wf->GetSine( p_ );
    p_ += w;
    return val;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 減算処理
 *
 * @param[in] val
 */
float Voice::VCF::Calc( float val )
{
    return val;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 減算処理
 *
 * @param[in] val
 */
float Voice::VCA::Calc( float val )
{
    return val;
}
