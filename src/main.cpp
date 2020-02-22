/**
 * @file main.cpp
 */
#include <cstdint>

#include "waveform.h"
#include "audio.h"
#include "synth.h"

int main(int argc, char *argv[])
{
    AudioCtrl* audioctrl = AudioCtrl::Create();
    Synth*     synth     = Synth::Create();

    audioctrl->Start();

    return 0;
}

