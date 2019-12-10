/**
 * @file midi.h
 */
#pragma once

/**
 * マクロ定義
 */
const int MIDI_ON_NOTE_MAX = 16;

/**
 * 構造体
 */
typedef struct _KEY_TABLE {
    int  velocity;
    bool isPressed;

    unsigned int m_p;
} MIDI_KEY_TABLE;


/**
 * グローバル変数
 */
extern int midiOnNoteNum;
extern int midiOnNoteList[MIDI_ON_NOTE_MAX];

extern MIDI_KEY_TABLE midiKeyTable[128];

/**
 * 関数定義
 */
extern bool midiInit( void );


