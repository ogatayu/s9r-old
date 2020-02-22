/**
 * @file synth.h
 */
#pragma once

class Synth {
private:
    Synth(){}
    ~Synth(){}

    Synth(const Synth&);
    Synth& operator=(const Synth&);
    static Synth* instance_;

    void Initialize();

    // internal class
    class VCO {
    public:
        VCO(){}
        ~VCO(){}
    };

    class VCF {
    public:
        VCF(){}
        ~VCF(){}
    };

    class VCA {
    public:
        VCA(){}
        ~VCA(){}
    };

    class EG {
    public:
        EG(){}
        ~EG(){}
    };

public:
    static Synth* Create();
    static void   Destroy();
    static Synth* GetInstance();

    // param
    float    tuning_, fs_;
    uint32_t p_;

    void NoteOn( int notenum, int velocity );
    void NoteOff( int notenum, int velocity );
};
