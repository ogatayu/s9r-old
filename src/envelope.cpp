/**
 * @file envelope.cpp
 */

#include "envelope.h"

/**
 * @brief constructor
 */
Envelope::Envelope( float fs )
{
    fs_ = fs;

    out_prev_ = 0.f;

    attack_ms_  = 0;
    decay_ms_   = 0;
    sustain_lv_ = 1.f;
    release_ms_ = 0;

    count_ = 0.f;
}

/**
 * @brief Process
 */
float Envelope::Process( float in )
{
    count_++;
    float elapsed_ms = (count_ * 1000.0) / fs_;

    float out;
    switch (state_)
    {
    case kIdle:
        out = in;
        break;

    case kAttack:
        if( attack_ms_ != 0 ) {
            out = in * (elapsed_ms / attack_ms_);
        }
        else {
            out = in;
        }
        if( elapsed_ms >= attack_ms_ ) {
            state_ = kDecay;
            count_ = 0.f;
        }
        break;

    case kDecay:
        if( decay_ms_ != 0 ) {
            out = in * (1.f - (sustain_lv_ * (elapsed_ms / decay_ms_)));
        }
        else {
            out = in * sustain_lv_;
        }
        if( elapsed_ms >= decay_ms_ ) {
            state_ = kSustain;
            count_ = 0.f;
        }
        break;

    case kSustain:
        out = in * sustain_lv_;
        break;

    case kRelease:
        out = out_prev_;
        break;

    default:
        out = 0;
        break;
    }

    // limiter
    out = (out>1.f) ? 1.f : ((out<0.f) ? 0.f : out);

    out_prev_ = out;
    return out;
}

/**
 * @brief Trigger
 */
void Envelope::Trigger()
{
    state_ = kAttack;
    count_ = 0.f;
}

/**
 * @brief Release
 */
void Envelope::Release()
{
    state_ = kRelease;
}

/**
 * @brief SetAttack
 */
void Envelope::SetAttack(int attack_ms)
{
    attack_ms_ = attack_ms;
}

/**
 * @brief SetDecay
 */
void Envelope::SetDecay(int decay_ms)
{
    decay_ms_ = decay_ms;
}

/**
 * @brief SetSustain
 */
void Envelope::SetSustain(float sustain_lv)
{
    sustain_lv_ = sustain_lv;
}

/**
 * @brief SetRelease
 */
void Envelope::SetRelease(int release_ms)
{
    release_ms = release_ms_;
}
