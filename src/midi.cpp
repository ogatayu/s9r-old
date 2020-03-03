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
        instance_ = new MidiCtrl();
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

    // initialize MIDI iunput
    try {
        midiin = new RtMidiIn();

        // Check inputs.
        unsigned int nPorts = midiin->getPortCount();
        std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";

        for ( unsigned i=0; i<nPorts; i++ ) {
        std::string portName = midiin->getPortName(i);
        std::cout << "  Input Port #" << i << ": " << portName << '\n';
        }

        if ( midi_choose_port( midiin ) == false ) {
            goto cleanup;
        }
        midiin->setCallback( &midi_input_callback, instance_ );

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

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief midi_in_callback
 * @param[in]  deltatime
 * @param[in]  message
 */
void MidiCtrl::InputCallback( double deltatime, std::vector< unsigned char > *message )
{
    const unsigned int bytes = message->size();
    if(bytes <= 0) { return; }

    const int kind     = message->at(0) & 0xF0;
    const int notenum  = message->at(1);
    const int velocity = message->at(2);

    switch( kind ) {
        case 0x80:  // Note off
            if(dumper_) { dumper_table_[notenum] = true; }
            else        { KeyOff(notenum); }
            break;

        case 0x90:  // Note on
            KeyOn( notenum, velocity );
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
 * @brief midi_in_callback
 * @param[in]  deltatime
 * @param[in]  message
 * @param[in]  user_data
 */
static void midi_input_callback( double deltatime, std::vector< unsigned char > *message, void * user_data )
{
    MidiCtrl* midictrl = (MidiCtrl*)user_data;
    midictrl->InputCallback( deltatime, message );
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief KeyOn
 * @param[in] Note Num
 * @param[in] v
 */
void MidiCtrl::KeyOn( int nn, int v )
{
    if( key_table_[nn] != 0 ) { KeyOff(nn); }
    key_table_[nn]    =  v;
    dumper_table_[nn] = false;
    on_key_nn_list_[on_key_num_] = nn + 256;  // +256は新規押鍵フラグ
    on_key_num_++;
    is_status_changed_ = true;
}

/**
 * @brief KeyOff
 * @param[in] Note Num
 * @param[in] v
 */
void MidiCtrl::KeyOff( int nn )
{
    key_table_[nn]     = 0;
    is_status_changed_ = true;

    // オンキーリストから、キーオフした鍵盤情報を取り除く
    for( int i=0; i<on_key_num_; i++ ){
        int onNN = on_key_nn_list_[i];
        if( nn == ((onNN >= 256) ? onNN-256 : onNN) ) {
            for( ; i<on_key_num_-1; i++ ) {
                on_key_nn_list_[i] = on_key_nn_list_[i+1];
            }
            on_key_num_--;
            return;
        }
    }
}

/**
 * @brief IsStatusChanged
 */
bool MidiCtrl::IsStatusChanged()
{
    return is_status_changed_;
};

/**
 * @brief ResetStatusChange
 */
void MidiCtrl::ResetStatusChange()
{
    is_status_changed_ = false;

    // 新規押鍵フラグもすべて落とす
    for( int i=0; i<on_key_num_; i++ ) {
        if(on_key_nn_list_[i] >= 256) {
            on_key_nn_list_[i] -= 256;
        }
    }
};

///////////////////////////////////////////////////////////////////////////////

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

