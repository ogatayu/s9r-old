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
    bool  IsKeyOn();
};
