/**
 * @file synth.h
 */
#pragma once

#include <list>

#include "audio.h."
#include "voice.h"

/**
 * @class VoiceCtrl
 */
class VoiceCtrl {
private:
    void NoteOn( int notenum, int velocity );
    void NoteOff( int notenum );

    // const
    static const int kVoiceNum = 32;

    // variable
    int key_mode_;
    int current_voice_no_;  // カレントボイス番号
    int poly_num_;          // ポリモード時の最大ボイス数
    int unison_num_;        // ユニゾンボイス数

    int mono_current_velocity_;  // モノモード時に使うワーク用ベロシテシティ値

    Voice* voice_[kVoiceNum];

    std::list<Voice*> on_voices_; // キーオン中のボイスリスト

    // func
    void TriggerPoly();
    void TriggerMono();

    Voice* GetNextOffVoice();

public:
    VoiceCtrl();
    ~VoiceCtrl(){}

    enum {
        kPoly = 0,  // ポリ
        kMono,      // モノ
        kLegato     // レガート
    };

    void  Trigger();
    float SignalProcess();
};


/**
 * @class Synth
 */
class Synth {
private:
    Synth(){}
    ~Synth(){}

    Synth(const Synth&);
    Synth& operator=(const Synth&);
    static Synth* instance_;

    void Initialize( float tuning );

    VoiceCtrl* voicectrl_;
    AudioCtrl* audioctrl_;

    uint32_t sigproc_time_; // nanosecond

public:
    static Synth* Create( float tuning );
    static void   Destroy();
    static Synth* GetInstance();

    void Start();

    float SignalCallback();
    uint32_t GetProcTime() { return sigproc_time_; }
};
