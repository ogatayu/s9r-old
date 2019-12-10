/**
 * @file midi.cpp
 */

#include <iostream>
#include <cstdlib>

#include "RtMidi.h"

#include "midi.h"


int midiOnNoteNum = 0;
int midiOnNoteList[MIDI_ON_NOTE_MAX];

MIDI_KEY_TABLE midiKeyTable[128] = { 0 };


/**
 * プロトタイプ宣言
 */
static bool midi_choose_port( RtMidiIn *rtmidi );
static void midi_in_callback( double deltatime, std::vector< unsigned char > *message, void * userData );


/**
 * @brief midiInit
 */
bool midiInit( void )
{
    RtMidiIn *midiin = NULL;

    // initialize MIDI iunput
    try {
        midiin = new RtMidiIn();

        if ( midi_choose_port( midiin ) == false ) {
            goto cleanup;
        }
        midiin->setCallback( &midi_in_callback, NULL );

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

/**
 * @brief midi_in_callback
 * @param[in]  deltatime
 * @param[in]  message
 * @param[in]  userData
 */
#include <math.h>
static void midi_in_callback( double deltatime, std::vector< unsigned char > *message, void * userData )
{
    const unsigned int nBytes = message->size();
    if(nBytes <= 0) { return; }

    const int nKind    = message->at(0) & 0xF0;
    const int notenum  = message->at(1);
    const int velocity = message->at(2);

    switch( nKind ) {
        case 0x80:  // ノートオフ
            midiKeyTable[notenum].isPressed = false;
            if( midiOnNoteNum > 0) {
                midiOnNoteNum--;
            }
            break;

        case 0x90:  // ノートオン
            midiKeyTable[notenum].velocity  = velocity;
            midiKeyTable[notenum].isPressed = true;

            midiOnNoteList[midiOnNoteNum] = notenum;
            midiOnNoteNum++;
            break;
    }

#if 1
    for ( unsigned int ix=0; ix<nBytes; ix++ ) {
        std::cout << "Byte " << ix << " = " << (int)message->at(ix) << ", ";
    }
    if ( nBytes > 0 ) {
        std::cout << "stamp = " << deltatime << std::endl;
    }
#endif
}
