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

public:
    static Synth* Create();
    static void   Destroy();
    static Synth* GetInstance();
};
