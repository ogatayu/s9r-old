/**
 * @file synth.h
 */
#pragma once

/**
 * @class Voice
 */
class Voice {
private:
    class VCO {
    public:
        VCO() {
            p_ = 0;
        }
        ~VCO(){}

        uint32_t p_;

        float Calc( uint32_t w );
    };

    class VCF {
    public:
        VCF(){}
        ~VCF(){}
        float Calc( float val );
    };

    class VCA {
    public:
        VCA(){}
        ~VCA(){}
        float Calc( float val );
    };

    class EG {
    private:
        float attack_, decay_, sustain_, release_;
    public:
        EG(){}
        ~EG(){}
    };

public:
    Voice() {
        nn_       = 0;
        velocity_ = 0;
        key_on_   = false;
    }
    ~Voice(){}

    Voice::VCO vco;
    Voice::VCF vcf;
    Voice::VCA vca;

    int  nn_;        // Note No.
    int  velocity_;  // velocity when key on
    bool key_on_;    // if key on then true

    float Calc( uint32_t w );
    bool  IfPlaying();
};


/**
 * @class VoiceCtrl
 */
class VoiceCtrl {
private:
    void NoteOn( int notenum, int velocity );
    void NoteOff( int notenum );

    // const
    static const int kVoiceNum = 8;

    // variable
    int key_mode_ = 0;

    Voice voice[kVoiceNum];

    std::list<Voice*> on_voices_; // キーオン中のボイスリスト

    // func
    void TriggerPoly();
    void TriggerMono();

public:
    void Trigger();

}


/**
 * @class Synth
 */
class Synth {
private:
    Synth(){}
    ~Synth(){}

    Synth(const Synth&);
    Synth& operator=(const Synth&);
    static Synth* instance_;

    void Initialize( float tuning );

    // param
    float tuning_, fs_;

    // module method
    VoiceCtrl voicectrl;

public:
    static Synth* Create( float tuning );
    static void   Destroy();
    static Synth* GetInstance();

    float SignalCallback();
};
