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

    out_lv_prev_ = 0.f;

    attack_ms_  = 0;
    decay_ms_   = 0;
    sustain_lv_ = 1.f;
    release_ms_ = 0;

    count_ = 0.f;
}

/**
 * @brief SetState
 */
void Envelope::SetState(enum EnvelopeState state)
{
    state_ = state;
    count_ = 0.f;
}

/**
 * @brief Process
 */
float Envelope::Process( float in )
{
    count_++;
    float elapsed_ms = (count_ * 1000.0) / fs_;

    float out_lv;
    switch (state_)
    {
    case kIdle:
        out_lv = 1.f;
        break;

    case kAttack:
        if( attack_ms_ != 0 ) {
            out_lv = elapsed_ms / attack_ms_;
        }
        else {
            out_lv = 1.f;
        }

        // end attack
        if( elapsed_ms >= attack_ms_ ) {
            SetState( kDecay );
        }
        break;

    case kDecay:
        if( decay_ms_ != 0 ) {
            out_lv = 1.f - (sustain_lv_ * (elapsed_ms / decay_ms_));
        }
        else {
            out_lv = sustain_lv_;
        }

        // end decay
        if( elapsed_ms >= decay_ms_ ) {
            SetState( kSustain );
        }
        break;

    case kSustain:
        out_lv = sustain_lv_;
        break;

    case kRelease:
        if( release_ms_ != 0 ) {
            out_lv = (out_lv_prev_ - (elapsed_ms *(1000.0/fs_)));
        }
        else {
            out_lv = 0;
        }

        // end release
        if( elapsed_ms >= release_ms_ ) {
            SetState( kIdle );
        }
        break;

    default:
        out_lv = 0;
        break;
    }

    // limiter
    out_lv = (out_lv>1.f) ? 1.f : ((out_lv<0.f) ? 0.f : out_lv);

    out_lv_prev_ = out_lv;
    return in * out_lv;
}

/**
 * @brief Trigger
 */
void Envelope::Trigger()
{
    SetState( kAttack );
}

/**
 * @brief Release
 */
void Envelope::Release()
{
    SetState( kRelease );
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
    release_ms_ = release_ms;
}
