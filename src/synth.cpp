/**
 * @file synth.cpp
 */
#include <iostream>
#include <math.h>

#include "synth.h"
#include "waveform.h"


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
 * initialize class
 */
void Synth::Initialize()
{
    tuning_ = 440.0;
    fs_     = 44100.0;

    Waveform::Create( tuning_, fs_ );
    Waveform* wf = Waveform::GetInstance();

}