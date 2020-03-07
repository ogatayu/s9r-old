/**
 * @file waveform.h
 */
#pragma once

#define WT_SIZE (1024) // 波形テーブルサイズ(単位はサンプル)

/**
 * @class Waveform
 */
class Waveform {
public:
    // wave form type
    const int WF_SINE   = 0;  // サイン波
    const int WF_TRI    = 1;  // 三角波
    const int WF_SAW    = 2;  // 鋸波
    const int WF_SQUARE = 3;  // 矩形波

    static Waveform* Create( float tuning, float fs );
    static void      Destroy();
    static Waveform* GetInstance();

    uint32_t CalcWFromNoteNo( float nn, float det );
    uint32_t CalcWFromFreq( float freq );

    // 位相は16:16の固定小数で扱う
    const float GetSine( int fixed_phase );
    const float GetTriangle( float nn, int fixed_phase );
    const float GetSaw( float nn, int fixed_phase );
    const float GetSquare( float nn, int fixed_phase );

    float GetSamplerate() { return fs_; }

private:
    Waveform(){}
    ~Waveform(){}

    Waveform(const Waveform&);
    Waveform& operator=(const Waveform&);
    static Waveform* instance_;

    // param
    float tuning_, fs_;

    // wavetable
    float wt_sine_[WT_SIZE];         // サイン波テーブル
    float wt_triangle_[WT_SIZE*68];  // 三角波テーブル（周波数帯域毎に分割）
    float wt_saw_[WT_SIZE*68];       // ノコギリ
    float wt_square_[WT_SIZE*68];    // 矩形

    // wavetable infomation
    typedef struct tagTableInfo {
        float freq;      // 基音周波数
        float noteNo;    // 基音周波数に対応するMIDIノートNo
        int   harmoNum;  // 倍音数
        int   offset;    // 対応する波形テーブルの先頭へのオフセット
    } TABLEINFO;
    TABLEINFO wt_info_[68+1];   // テーブル情報表
    int wt_info_num_;           // テーブル情報表の行数


    void Initialize( float freq, float fs );

    void GenTblTriangle(float* p_buf, int harmo_num);
    void GenTblSaw(float* p_buf, int harmo_num);
    void GenTblSquare(float* p_buf, int harmo_num);

    float* GetWTFromNN( int wf, float nn );
    float* GetWTFromFreq( int wf, float freq );
};
