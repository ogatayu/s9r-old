/**
 * @file main.cpp
 */
#include <cstdint>

#include "waveform.h"
#include "audio.h"
#include "midi.h"
#include "synth.h"

int main(int argc, char *argv[])
{
    // initialize
    AudioCtrl* audioctrl = AudioCtrl::Create();
    MidiCtrl*  midictrl  = MidiCtrl::Create();
    Synth*     synth     = Synth::Create( 440.0 );

    // s9r start!!!
    audioctrl->Start();

    // end
    AudioCtrl::Destroy();

    return 0;
}

