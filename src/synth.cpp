/**
 * @file synth.cpp
 */
#include <iostream>
#include <math.h>

#include "common.h"


// 波形テーブル
#define	WT_SIZE (1024)
#define TBL_NUM (128)
static float synthTableSin[WT_SIZE];     // sine
static float synthTableTri[WT_SIZE * TBL_NUM];     // 三角波
static float synthTableSaw[WT_SIZE * TBL_NUM];     // ノコギリ
static float synthTableSquare[WT_SIZE * TBL_NUM];  // 矩形

#define _SIN(a) (synthTableSin[(a) % WT_SIZE])   // sin()関数の代わりにテーブルを使い、テーブル生成を高速化


static void synth_maketbl_tryangle(float* p_buf, int freq_limit);
static void synth_maketbl_saw(float* p_buf, int freq_limit);
static void synth_maketbl_square(float* p_buf, int freq_limit);

static float fastsin( unsigned int phase );


/**
 * @brief synthInit
 */
bool synthInit( void )
{
    float fs, freq, freq_min;
    int   freq_limit;

    fs       = 48000.0f;
    freq_min = 30.0f;

    // サイン波テーブル生成
    for( int ix=0; ix < WT_SIZE; ix++ ) {
#if 0
        synthTableSin[ix] += fastsin( (UINT32_MAX / WT_SIZE) * ix );
#else
        synthTableSin[ix] = sin( ((2.0f * PI) / WT_SIZE) * ix );
#endif
    }

    // 三角波テーブル生成
    CMN_MEASURE_START();
    for(int ix=1; ix<TBL_NUM; ix++) {
        freq       = 440.0f * pow(2.0f, (ix - 69.0f) / 12.0f);
        freq_limit = (int)(fs / 2 / freq);
        synth_maketbl_tryangle(&synthTableTri[WT_SIZE * ix], freq_limit);
    }
    CMN_MEASURE_END( "Tryangle table create" );

    // ノコギリ波テーブル生成
    CMN_MEASURE_START();
    for(int ix=1; ix<TBL_NUM; ix++) {
        freq       = 440.0f * pow(2.0f, (ix - 69.0f) / 12.0f);
        freq_limit = (int)(fs / 2 / freq);
        synth_maketbl_saw(&synthTableSaw[WT_SIZE * ix], freq_limit);
    }
    CMN_MEASURE_END( "Saw wave table create" );

    // 矩形波テーブル生成
    CMN_MEASURE_START();
    for(int ix=1; ix<TBL_NUM; ix++) {
        freq       = 440.0f * pow(2.0f, (ix - 69.0f) / 12.0f);
        freq_limit = (int)(fs / 2 / freq);
        synth_maketbl_square(&synthTableSquare[WT_SIZE * ix], freq_limit);
    }
    CMN_MEASURE_END( "Square wave table create" );

    return true;
}


/**
 * @brief 三角波テーブル生成
 */
static void synth_maketbl_tryangle(float* p_buf, int freq_limit)
{
    for(int ix=0;ix<WT_SIZE;ix++){
        p_buf[ix]=0;
        for(int jx=1;jx <= freq_limit;jx += 2){                    // harmoNum倍音まで生成（奇数倍音のみなので2ずつ増加）
            if(jx%4 == 1) p_buf[ix] += 1.f/(jx*jx) * _SIN(jx*ix);  // jx=1,5,9,13,...
            else          p_buf[ix] -= 1.f/(jx*jx) * _SIN(jx*ix);  // jx=3,7,11,15,...
        }
        p_buf[ix] *= 8.f/(PI*PI);
    }
}


/**
 * @brief ノコギリ波テーブル生成
 */
static void synth_maketbl_saw(float* p_buf, int freq_limit)
{
    for(int ix=0; ix<WT_SIZE; ix++){
        p_buf[ix]=0;
        for(int jx=1; jx <= freq_limit; jx++) {
            p_buf[ix] += 1.0f/jx * _SIN(jx*ix);
        }
        p_buf[ix] *= 2.0f / PI;
    }
}


/**
 * @brief 矩形波テーブル生成
 */
static void synth_maketbl_square(float* p_buf, int freq_limit)
{
    for(int ix=0;ix<WT_SIZE;ix++){
        p_buf[ix]=0;
        for(int jx=1; jx <= freq_limit; jx++){
            p_buf[ix] += 1.0f/jx * _SIN(jx*ix);
            p_buf[ix] -= 1.0f/jx * _SIN(jx * (ix+WT_SIZE/2));
        }
        p_buf[ix] *= 1.f/PI;
    }
}


/**
 * @brief synthGetW
 */
unsigned int synthGetW(int nn, float det, float fs)
{
    float f = 440.f * pow( 2.f, (nn + det/100.f - 69.f) / 12.f );

    // 角速度(１周期＝WT_SIZEの16:16固定小数点表現)へ変換
    return (unsigned int)(WT_SIZE * (1<<16) * f/48000.f);
}


/**
 * @brief synthGetWave
 */
float synthGetWave( int phase )
{
    #define _IDX(x,y)  (((y)+((x)>>16))&(WT_SIZE-1))
    return synthTableSin[ (WT_SIZE*69) + _IDX(phase, 0) ];
}


/**
 * @brief fastsin
 */
static float fastsin( unsigned int phase )
{
    const float frf3 = -1.0f / 6.0f;
    const float frf5 = 1.0f / 120.0f;
    const float frf7 = -1.0f / 5040.0f;
    const float frf9 = 1.0f / 362880.0f;
    const float f0pi5 = 1.570796327f;
    float x, x2, asin;
    unsigned int tmp = 0x3f800000 | ( phase >> 7 );
    if (phase & 0x40000000)
        tmp ^= 0x007fffff;
    x = ( *( (float*)&tmp ) - 1.0f ) * f0pi5;
    x2 = x * x;
    asin = ( ( ( ( frf9 * x2 + frf7 ) * x2 + frf5 ) * x2 + frf3 ) * x2 + 1.0f ) * x;
    return ( phase & 0x80000000 ) ? -asin : asin;
}

