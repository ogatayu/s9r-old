/**
 * @file keyctrl.h
 */
#pragma once

/**
 * @class KeyCtrl
 */
class KeyCtrl {
private:
public:
    KeyCtrl();
    ~KeyCtrl(){}
    void KeyEventHandle( int key, int action );
};
