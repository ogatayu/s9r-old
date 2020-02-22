/**
 * @file synth.h
 */
#pragma once

class Voice {
private:
    class VCO {
    public:
        VCO();
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
    Voice(){}
    ~Voice(){}

    Voice::VCO vco;
    Voice::VCF vcf;
    Voice::VCA vca;

    float Calc( uint32_t w );
};

class Synth {
private:
    Synth(){}
    ~Synth(){}

    Synth(const Synth&);
    Synth& operator=(const Synth&);
    static Synth* instance_;

    void Initialize( float tuning );

public:
    static Synth* Create( float tuning );
    static void   Destroy();
    static Synth* GetInstance();

    // const
    static const int kVoiceNum = 16;

    // param
    float tuning_, fs_;

    // module method
    Voice voice[kVoiceNum];

    void NoteOn( int notenum, int velocity );
    void NoteOff( int notenum, int velocity );
};
