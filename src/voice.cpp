/**
 * @file voice.cpp
 */
#include <cstdint>
#include <math.h>

#include "common.h"

#include "waveform.h"
#include "filter.h"
#include "envelope.h"

#include "voice.h"

/**
 * @brief 信号処理部
 */
float Voice::Calc()
{
    float val;
    val = vco.Calc();
    val = vcf.Calc(val);
    val = vca.Calc(val);
    return val;
}

// キーオン中かどうかを返す
bool Voice::IsKeyOn()
{
    return key_on_;
}

bool Voice::IsPlaying()
{
    return vca.IsPlaying();
}

// トリガー通知
void Voice::Trigger(void)
{
    vca.Trigger();
    key_on_ = true;
}

// キーオフ通知
void Voice::Release(void)
{
    vca.Release();
    key_on_ = false;
}


// 発振ノートNo等の設定。キーオン毎にコールされる。
void Voice::SetNoteInfo( int nn, int velo )
{
    nn_       = nn;
    velocity_ = velo;
    vco.SetNoteNo( nn, IsKeyOn() );
}


void Voice::SetUnisonInfo(Voice* pMasterVoice, int unisonNum, int unisonNo)
{
    /* do nothing (now...) */
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 波形生成
 *
 * @param[in] nn Note number
 * @param[in] is_key_on
 */
void Voice::VCO::SetNoteNo( int nn, bool is_key_on )
{
    // ポルタメント関連の変数処理
    if(current_porta_time_ < 1.f) porta_start_nn_ = current_nn_;  // ポルタメント中→開始NoteNoはポルタメント中のNoteNo
    else                          porta_start_nn_ = nn_;          // 定常状態→開始NoteNoは今までのノートNo

    // ポルタメントが必要かどうかを判定
    // NordLead2やSynth1の"Auto"なポルタメント動作とする。
    // これを止め、常にポルタメントするようにするには、開始条件からisKeyOnを削除する。
    if( (porta_start_nn_   != nn) &&
        (porta_time_delta_ != 0.f) &&
         is_key_on
         ) {
            // ポルタメント開始
            current_porta_time_ = 0.f;
        }
    else {
        // ポルタメント不要
        current_porta_time_ = 1.f;
    }

    nn_ = (float)nn;
}

/**
 * @brief 波形生成
 *
 * @param[in] w 角速度
 */
float Voice::VCO::Calc()
{
    Waveform* wf = Waveform::GetInstance();
    uint32_t w = wf->CalcWFromNoteNo( nn_, detune_cent_ );

    //float val = wf->GetSine( p_ );
    float val = wf->GetSaw( nn_, p_ );

    p_ += w;
    return val;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief constructor
 */
Voice::VCF::VCF()
{
    Waveform* w = Waveform::GetInstance();
    filter = new Filter( w->GetSamplerate() );

    filter->LowPass( 1000.f, 0.7f );
}

/**
 * @brief 減算処理
 *
 * @param[in] val
 */
float Voice::VCF::Calc( float val )
{
    //return filter->Process(val);
    return val;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief VCA constructor
 */
Voice::VCA::VCA()
{
    Waveform* w = Waveform::GetInstance();
    env = new Envelope( w->GetSamplerate() );

    env->SetAttack( 100 );
    env->SetDecay( 200 );
    env->SetSustain( 0.5f );
    env->SetRelease( 1000 );
}

/**
 * @brief 音量加工
 */
void Voice::VCA::Trigger()
{
    env->Trigger();
}

/**
 * @brief 音量加工
 */
void Voice::VCA::Release()
{
    env->Release();
}


/**
 * @brief 音量加工
 *
 * @param[in] val
 */
float Voice::VCA::Calc( float val )
{
    return env->Process( val ) * 0.5f;
}

/**
 * @brief 音量加工
 */
bool Voice::VCA::IsPlaying()
{
    return env->IsPlaying();
}
