/**
 * @file synth.h
 */
#pragma once

#include <list>

#include "filter.h"
#include "envelope.h"


/**
 * @class Voice
 */
class Voice {
private:
    class VCO {
    public:
        VCO() {
            p_  = 0;
            nn_ = 0;
            detune_cent_ = 0;
        }
        ~VCO(){}

        // variable
        uint32_t p_;            // phase(16:16 fixed-point)
        float    nn_;           // 発振指定されたノートNo
        float    current_nn_;   // 現在発振中のノートNo（ノートNoを小数にする事でポルタメント中のノートNoを表現する）
        float    porta_start_nn_;      // ポルタメント開始時のnoteNo
        float    current_porta_time_;      // ポルタメント経過時間（1で正規化、0～1でポルタメント中）
        float    porta_time_delta_;    // ポルタメント速度
        float    detune_cent_;      // ボイス間デチューン値（単位はセント）

        void  SetNoteNo( int nn, bool is_key_on );
        float Calc();
    };

    class VCF {
    private:
        Filter* filter;
    public:
        VCF();
        ~VCF(){}
        float Calc( float val );
    };

    class VCA {
    private:
        Envelope* env;
    public:
        VCA();
        ~VCA(){}
        void  Trigger();
        void  Release();
        float Calc( float val );
        bool  IsPlaying();
    };

    class EG {
    private:
        float attack_, decay_, sustain_, release_;
    public:
        EG(){}
        ~EG(){}
    };

public:
    Voice() {
        voice_no_ = 0;
        nn_       = 0;
        velocity_ = 0;
        key_on_   = false;
    }
    ~Voice(){}

    Voice::VCO vco;
    Voice::VCF vcf;
    Voice::VCA vca;

    int  voice_no_;  // Voice No.
    int  nn_;        // Note No.
    int  velocity_;  // velocity when key on
    bool key_on_;    // if key on then true


    void Trigger(void);
    void Release();

    void SetNoteInfo(int nn,int velo);
    void SetUnisonInfo(Voice* pMasterVoice, int unisonNum, int unisonNo);

    int  GetNoteNo(void) { return nn_; };    // 発振ノートNoを返す

    int  GetNo(void) { return voice_no_; };  // ボイス番号を返す
    void SetNo(int no) { voice_no_ = no; };  // ボイス番号を返す

    float Calc();
    bool  IsPlaying();
    bool  IsKeyOn(void);
};


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

    // param
    float tuning_, fs_;

    VoiceCtrl* voicectrl_;
    AudioCtrl* audioctrl_;

public:
    static Synth* Create( float tuning );
    static void   Destroy();
    static Synth* GetInstance();

    void Start();
    float GetSamplerate() { return fs_; }

    float SignalCallback();

};
