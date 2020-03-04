/**
 * @file keyctrl.cpp
 */
#include <cstdlib>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "midi.h"

#include "keyctrl.h"

static void midi_key(int nn, int action);

/**
 * @brief constructor
 */
KeyCtrl::KeyCtrl()
{
    ;
}

/**
 * @brief Start
 */
void KeyCtrl::KeyEventHandle( int key, int action )
{
    switch (key) {
        case 'Z': midi_key(60, action); break;
        case 'S': midi_key(61, action); break;
        case 'X': midi_key(62, action); break;
        case 'D': midi_key(63, action); break;
        case 'C': midi_key(64, action); break;
        case 'V': midi_key(65, action); break;
        case 'G': midi_key(66, action); break;
        case 'B': midi_key(67, action); break;
        case 'H': midi_key(68, action); break;
        case 'N': midi_key(69, action); break;
        case 'J': midi_key(70, action); break;
        case 'M': midi_key(71, action); break;
        default: break;
    }
}

static void midi_key(int nn, int action)
{
    std::vector<unsigned char> msg(3);

    /* kind */
    switch (action) {
        case GLFW_PRESS:   msg.at(0) = 0x90; break; /* Note On */
        case GLFW_RELEASE: msg.at(0) = 0x80; break; /* Note Off */
        default: break;
    }
    msg.at(1) = nn;  /* notenum */
    msg.at(2) = 100;  /* velocity */

    MidiCtrl* midictrl = MidiCtrl::GetInstance();
    midictrl->MidiSend( &msg );
}
