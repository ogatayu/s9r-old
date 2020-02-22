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

Synth* Synth::Create()
{
    if (!instance_)
    {
        instance_ = new Synth;
        instance_->Initialize();
    }
    return instance_;
}

void Synth::Destroy()
{
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
void Synth::Initialize()
{
    AudioCtrl* audioctrl = AudioCtrl::GetInstance();

    p_ = 0;

    tuning_ = 440.0;
    fs_     = audioctrl->SampleRateGet();
    Waveform* wf = Waveform::Create( tuning_, fs_ );

    audioctrl->SignalCallbackSet( synth_signal_callback, this );
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief synth_signal_callback
 */
static double synth_signal_callback( void* userdata )
{
    Synth*    synth = (Synth*)userdata;
    Waveform* wf = Waveform::GetInstance();

    uint32_t w   = wf->CalcWFromFreq( synth->tuning_ );
    double   val = wf->GetSaw( synth->tuning_, synth->p_ );

    synth->p_ += w;

    return val;;
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
