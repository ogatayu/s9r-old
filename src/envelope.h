/**
 * @file filter.h
 */
#pragma once

/**
 * @class Envelope
 */
class Envelope {
public:
    Envelope( float fs );
    ~Envelope(){}

    float Process( float in );

    void Trigger();
    void Release();

    void SetAttack (int attack_ms);
    void SetDecay  (int decay_ms);
    void SetSustain(float sustain_lv);
    void SetRelease(int release_ms);

    bool IsPlaying();

private:
    enum EnvelopeState
    {
        kIdle,
        kAttack,
        kDecay,
        kSustain,
        kRelease
    };

    float fs_;  // sample rate

    enum EnvelopeState state_;
    float count_;

    float out_lv_prev_;

    int   attack_ms_;
    int   decay_ms_;
    float sustain_lv_;
    int   release_ms_;

    void SetState(enum EnvelopeState state);
};
