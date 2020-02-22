/**
 * @file midi.cpp
 */

#include <iostream>
#include <cstdlib>

#include "RtMidi.h"

#include "midi.h"


static void midi_input_callback( double deltatime, std::vector< unsigned char > *message, void * user_data );
static bool midi_choose_port( RtMidiIn *rtmidi );

MidiCtrl* MidiCtrl::instance_ = nullptr;

/**
 * @brief Create
 */
MidiCtrl* MidiCtrl::Create()
{
    if (!instance_)
    {
        instance_ = new MidiCtrl;
        instance_->Initialize();
    }
    return instance_;
}

/**
 * @brief Destroy
 */
void MidiCtrl::Destroy()
{
    delete instance_;
    instance_ = nullptr;
}

/**
 * @brief GetInstance
 */
MidiCtrl* MidiCtrl::GetInstance()
{
    return instance_;
}

/**
 * @brief initialize class
 */
bool MidiCtrl::Initialize()
{
    RtMidiIn *midiin = NULL;

    // init class
    synth_ = Synth::GetInstance();

    // initialize MIDI iunput
    try {
        midiin = new RtMidiIn();

        if ( midi_choose_port( midiin ) == false ) {
            goto cleanup;
        }
        midiin->setCallback( &midi_input_callback, synth_ );

        // Don't ignore sysex, timing, or active sensing messages.
        midiin->ignoreTypes( false, false, false );
    } catch ( RtMidiError &error ) {
        error.printMessage();
        return false;
    }
    return true;

cleanup:
    delete midiin;
    return false;
}

/**
 * @brief midi_in_callback
 * @param[in]  deltatime
 * @param[in]  message
 * @param[in]  user_data
 */
static void midi_input_callback( double deltatime, std::vector< unsigned char > *message, void * user_data )
{
    Synth* synth = (Synth*)user_data;
    const unsigned int bytes = message->size();
    if(bytes <= 0) { return; }

    const int kind     = message->at(0) & 0xF0;
    const int notenum  = message->at(1);
    const int velocity = message->at(2);

    switch( kind ) {
        case 0x80:  // Note on
            synth->NoteOn( notenum, velocity );
            break;

        case 0x90:  // Note off
            synth->NoteOff( notenum, velocity );
            break;
    }

#if 1
    for ( unsigned int ix=0; ix<bytes; ix++ ) {
        std::cout << "Byte " << ix << " = " << (int)message->at(ix) << ", ";
    }
    if ( bytes > 0 ) {
        std::cout << "stamp = " << deltatime << std::endl;
    }
#endif
}

/**
 * @brief midi_choose_port
 * @param[in,out] rtmidi
 */
static bool midi_choose_port( RtMidiIn *rtmidi )
{
    std::string portName;
    unsigned int ix = 0, nPorts = rtmidi->getPortCount();
    if ( nPorts == 0 ) {
        std::cout << "No input ports available!" << std::endl;
        return false;
    }

    if ( nPorts == 1 ) {
        std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
    }
    else {
        for ( ix=0; ix<nPorts; ix++ ) {
            portName = rtmidi->getPortName(ix);
            std::cout << "  Input port #" << ix << ": " << portName << '\n';
        }

        do {
            std::cout << "\nChoose a port number: ";
            std::cin >> ix;
        } while ( ix >= nPorts );
        std::string keyHit;
        std::getline( std::cin, keyHit );  // used to clear out stdin
    }

    rtmidi->openPort( ix );

  return true;
}

