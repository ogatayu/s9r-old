/**
 * @file main.cpp
 */
#include <cstdint>

#include "waveform.h"
#include "audio.h"
#include "midi.h"
#include "synth.h"

#include "draw.h"

int main(int argc, char *argv[])
{
    // initialize
    AudioCtrl* audioctrl = AudioCtrl::Create();
    if(!audioctrl) {
        return 1;
    }

    MidiCtrl* midictrl = MidiCtrl::Create();
    if(!midictrl) {
        return 1;
    }

    Synth* synth = Synth::Create( 440.0 );
    if(!synth) {
        return 1;
    }

    // s9r start!!!
    synth->Start();

    // end
    AudioCtrl::Destroy();
    MidiCtrl::Destroy();
    Synth::Destroy();

    return 0;
}

