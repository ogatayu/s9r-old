/**
 * @file midi.h
 */
#pragma once

#include "synth.h"

class MidiCtrl {
private:
    MidiCtrl(){}
    ~MidiCtrl(){}

    MidiCtrl(const MidiCtrl&);
    MidiCtrl& operator=(const MidiCtrl&);
    static MidiCtrl* instance_;

    bool Initialize();

    Synth* synth_;

    /* midi state */
    //int on_note_num_ = 0;
    //int on_note_list_[MIDI_ON_NOTE_MAX];

    /* midi key table */
    typedef struct _KEY_TABLE {
        int  velocity;
        bool isPressed;

        unsigned int m_p;
    } MIDI_KEY_TABLE;
    MIDI_KEY_TABLE key_table_[128] = { 0 };


public:
    static MidiCtrl* Create();
    static void      Destroy();
    static MidiCtrl* GetInstance();
};
