
/**
 * @class Waveform
 * @brief
 * @note  this class is singleton
 */
#include <cstdint>
#include <math.h>

#include "waveform.h"

#define PI (3.141592653589793238462643383279f)
#define _SIN(a) (wt_sine_[(a) % WT_SIZE])   // sin()関数の代わりにテーブルを使い、テーブル生成を高速化

Waveform* Waveform::instance_ = nullptr;

static float waveform_fastsin( unsigned int phase );

Waveform* Waveform::Create( float tuning, float fs )
{
    if (!instance_)
    {
        instance_ = new Waveform;
        instance_->Initialize( tuning, fs );
    }
    return instance_;
}

void Waveform::Destroy()
{
    delete instance_;
    instance_ = nullptr;
}

Waveform* Waveform::GetInstance()
{
    return instance_;
}

/**
 * initialize class
 * @param freq
 * @param fs
 */
void Waveform::Initialize( float tuning, float fs )
{
    tuning_ = tuning;
    fs_     = fs;

    // サイン波テーブル生成
    for( int ix=0; ix<WT_SIZE; ix++ ) {
#if 0
        wt_sine_[ix] = waveform_fastsin( (UINT32_MAX / WT_SIZE) * ix );
#else
        wt_sine_[ix] = sin( 2.f * PI * ix / WT_SIZE );
#endif
    }
    // 波形テーブル情報表（30Hz～22.05KHzを複数の帯域に分割しそれぞれの帯域の情報を記した表）を計算する。
    // ※この計算方法では、入力パラメタとして90分割を与えた場合、68分割に最適化される
    wt_info_num_      = 0;
    int   oldHarmoNum = -1;
    int   jx           = 0;
    float minFreq     = 30.f;

    for(int ix=0; ix<90; ix++) {
        // テーブル情報を計算
        wt_info_[jx].freq     = minFreq * pow(fs/2/minFreq,(float)ix/90);
        wt_info_[jx].harmoNum = (int)(fs/2/wt_info_[jx].freq);
        wt_info_[jx].offset   = jx * WT_SIZE;

        // 前回と同じ倍音数かどうかチェック
        if(wt_info_[jx].harmoNum == oldHarmoNum) continue;	// 同じなので処理スキップ
        oldHarmoNum = wt_info_[jx].harmoNum;

        // 対応周波数を求めなおし、noteNoを求める
        wt_info_[jx].freq   = fs / 2 / wt_info_[jx].harmoNum;
        wt_info_[jx].noteNo = 69.f + 12.f * log(wt_info_[jx].freq / 440.f) / log(2.f);
        jx++;
    }
    wt_info_num_ = jx;

    // 周波数帯域毎に、三角波、ノコギリ波、矩形波テーブルを生成
    for(int ix=0; ix<wt_info_num_; ix++){
        GenTblTriangle(&wt_triangle_[WT_SIZE * ix], wt_info_[ix].harmoNum);
        GenTblSaw(&wt_saw_[WT_SIZE * ix], wt_info_[ix].harmoNum);
        GenTblSquare(&wt_square_[WT_SIZE * ix], wt_info_[ix].harmoNum);
    }

}

/**
 * GetSine
 * @param phase
 */
const float Waveform::GetSine( float phase )
{
    return waveform_fastsin( (phase / (2.0*PI)) * UINT32_MAX );
}

/**
 * GetWave
 * @param phase
 */
static float GetWave( float* p_tbl, int fp_phase )
{
    // 16:16の固定小数からインデックス(=整数部)を取り出すマクロ
    // x:固定小数点による位相、y:インデックスオフセット
    #define _IDX(x,y)   (((y)+((x)>>16))&(WT_SIZE-1))

    int idx  = _IDX(fp_phase,0);                               // 整数部の取り出し
    int idx2 = _IDX(fp_phase,1);                               // 補間用に、idxの次のidxを求める。単純に+1すると範囲オーバーの可能性有り。
    float deci = (float)(fp_phase & ((1<<16)-1)) / (1<<16);    // 少数部の取り出し(上位16ビットをクリアし、1/65536倍する)
    return p_tbl[idx] + (p_tbl[idx2] - p_tbl[idx]) * deci;  // 線形補完で出力
}

/**
 * NoteNoから最適な（＝倍音がナイキストを超えない）波形テーブルを求める
 * @param wf 波形の種類
 * @param nn NoteNumber
 */
float* Waveform::getWTFromNoteNo( int wf, float nn )
{
    if( wf==WF_SINE ) {
        return &wt_sine_[0];
    }
    else {
        // テーブル情報表を頭からサーチ。
        // もしパフォーマンスが問題になる場合は、nnを適当に整数化したものとoffsetのマップテーブルを使う。
        float* pTbl = (wf==WF_TRI) ? &wt_triangle_[0] : ((wf==WF_SAW) ? &wt_saw_[0] : &wt_square_[0]);
        for( int ix=0; ix < wt_info_num_; ix++ ) {
            if(nn <= wt_info_[ix].noteNo) {
                return pTbl + (WT_SIZE * ix);
            }
        }
    }

    return &wt_sine_[0];
}

/**
 * NoteNo,デチューン(セント単位)から角速度を求める
 * @param nn       NoteNumber
 * @param det      detune val
 * @param fs
 */
uint32_t Waveform::calcWFromNoteNo( float nn, float det, float fs )
{
    // NoteNo、デチューン値から発振周波数fを求める noteNo=69(A4)で440Hz
    // ※デチューン値：100セントで1NoteNo分のピッチに相当する
    float f = tuning_ * pow( 2.f, ( nn+det/100.f-69.f) / 12.f );

    // 角速度(1周期＝WT_SIZEの16:16固定小数点表現)へ変換
    return (uint32_t)( WT_SIZE * (1<<16) * f/fs );
}

/**
 * GetTriangle
 * @param nn       NoteNumber
 * @param fp_phase phase of 16:16 fixed-point number
 */
const float Waveform::GetTriangle( int nn, int fp_phase )
{

}

/**
 * GetSaw
 * @param nn       NoteNumber
 * @param fp_phase phase of 16:16 fixed-point number
 */
const float Waveform::GetSaw( int nn, float fp_phase )
{

}

/**
 * GetSquare
 * @param nn       NoteNumber
 * @param fp_phase phase of 16:16 fixed-point number
 */
const float Waveform::GetSquare( int nn, float fp_phase )
{

}

/**
 * @brief 三角波テーブル生成
 */
void Waveform::GenTblTriangle(float* p_buf, int harmo_num)
{
    for(int ix=0;ix<WT_SIZE;ix++){
        p_buf[ix]=0;
        for(int jx=1;jx <= harmo_num;jx += 2) {                    // harmoNum倍音まで生成（奇数倍音のみなので2ずつ増加）
            if(jx%4 == 1) p_buf[ix] += 1.f/(jx*jx) * _SIN(jx*ix);  // jx=1,5,9,13,...
            else          p_buf[ix] -= 1.f/(jx*jx) * _SIN(jx*ix);  // jx=3,7,11,15,...
        }
        p_buf[ix] *= 8.f/(PI*PI);
    }
}

/**
 * @brief ノコギリ波テーブル生成
 */
void Waveform::GenTblSaw(float* p_buf, int harmo_num)
{
    for(int ix=0; ix<WT_SIZE; ix++) {
        p_buf[ix]=0;
        for(int jx=1; jx <= harmo_num; jx++) {
            p_buf[ix] += 1.0f/jx * _SIN(jx*ix);
        }
        p_buf[ix] *= 2.0f / PI;
    }
}

/**
 * @brief 矩形波テーブル生成
 */
void Waveform::GenTblSquare(float* p_buf, int harmo_num)
{
    for(int ix=0;ix<WT_SIZE;ix++) {
        p_buf[ix]=0;
        for(int jx=1; jx <= harmo_num; jx++) {
            p_buf[ix] += 1.0f/jx * _SIN(jx*ix);
            p_buf[ix] -= 1.0f/jx * _SIN(jx * (ix+WT_SIZE/2));
        }
        p_buf[ix] *= 1.f/PI;
    }
}


/**
 * a Fast sine func
 * @param phase
 */
static float waveform_fastsin( unsigned int phase )
{
    const float frf3 = -1.0f / 6.0f;
    const float frf5 = 1.0f / 120.0f;
    const float frf7 = -1.0f / 5040.0f;
    const float frf9 = 1.0f / 362880.0f;
    const float f0pi5 = 1.570796327f;
    float x, x2, asin;
    unsigned int tmp = 0x3f800000 | ( phase >> 7 );
    if (phase & 0x40000000) {
        tmp ^= 0x007fffff;
    }
    x = ( *( (float*)&tmp ) - 1.0f ) * f0pi5;
    x2 = x * x;
    asin = ( ( ( ( frf9 * x2 + frf7 ) * x2 + frf5 ) * x2 + frf3 ) * x2 + 1.0f ) * x;
    return ( phase & 0x80000000 ) ? -asin : asin;
}