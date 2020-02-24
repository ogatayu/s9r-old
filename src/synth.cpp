/**
 * @file synth.cpp
 */
#include <iostream>
#include <thread>

#include <math.h>

#include "audio.h"
#include "midi.h"
#include "synth.h"
#include "waveform.h"


static double synth_signal_callback( void* userdata );

Synth* Synth::instance_ = nullptr;


/**
 * @brief Create
 */
Synth* Synth::Create( float tuning )
{
    if (!instance_)
    {
        instance_ = new Synth;
        instance_->Initialize( tuning );
    }
    return instance_;
}


/**
 * @brief Destroy
 */
void Synth::Destroy()
{
    AudioCtrl* audioctrl = AudioCtrl::GetInstance();
    audioctrl->SignalCallbackUnset();

    delete instance_;
    instance_ = nullptr;
}

/**
 * @brief GetInstance
 */
Synth* Synth::GetInstance()
{
    return instance_;
}

/**
 * @brief initialize class
 */
void Synth::Initialize( float tuning )
{
    AudioCtrl* audioctrl = AudioCtrl::GetInstance();

    // init param
    tuning_ = tuning;
    fs_     = audioctrl->SampleRateGet();

    // create wave form
    Waveform* wf = Waveform::Create( tuning_, fs_ );

    // set audio callback
    audioctrl->SignalCallbackSet( synth_signal_callback, this );
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Signal callback
 */
float Synth::SignalCallback()
{
    MidiCtrl* midictrl = MidiCtrl::GetInstance();

    if( midictrl.IsStatusChanged() ) {
        voicectrl.Trigger();           // トリガー/リリース処理
        midictrl.ResetStatusChange();  // 鍵盤状態変更フラグを落とす
    }


    // 各ボイスの信号処理を行いステレオMIXする（ボイスコントローラーの仕事）。
}

/**
 * @brief Signal callback handler
 */
static double synth_signal_callback( void* userdata )
{
    Synth* synth = (Synth*)userdata;
    return synth->SignalCallback();
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief NoteOn
 */
void VoiceCtrl::NoteOn( int notenum, int velocity )
{
    for(int ix = 0; ix<kVoiceNum; ix++) {
        if( voice[ix].key_on_ == false ) {
            Voice *v = &voice[ix];
            v->nn_       = notenum;
            v->velocity_ = velocity;
            v->key_on_   = true;
            //printf("key ON : nn=%d, v=%d\n", notenum, velocity);
            break;
        }
    }

    return;
}

/**
 * @brief NoteOff
 */
void VoiceCtrl::NoteOff( int notenum )
{
    for(int ix = 0; ix<kVoiceNum; ix++) {
        if( voice[ix].nn_ == notenum ) {
            voice[ix].key_on_ = false;
            //printf("key OFF: nn=%d, v=%d\n", notenum, velocity);
            break;
        }
    }

    return;
}

/**
 * @brief Trigger
 */
void VoiceCtrl::Trigger()
{
    if(key_mode_ == 0) TriggerPoly();
    else               TriggerMono();

#if 0
    Waveform* wf = Waveform::GetInstance();
    double  val = 0;
    uint32_t w;
    for(int ix = 0; ix<kVoiceNum; ix++) {
        if( voice[ix].IfPlaying() == true ) {
            Voice *v = &(voice[ix]);
            w    = wf->CalcWFromNoteNo( v->nn_, 0 );
            val += v->Calc( w );
        }
    }
#endif
}

// ポリモード時のトリガー/リリースを制御
void VoiceCtrl::TriggerPoly()
{
    //// まずキーリリース処理
    // オンボイスリストからみて、キーボードテーブル上でノートオフ(ベロシティ値=0)に
    // なっているかをチェック→ノートオフならリリースする
    for( std::list<Voice*>::iterator v=on_voices_.begin(); v != on_voices_.end(); ){
        if( m_pKbdInfo->getVelocity( (*v)->getNoteNo() ) == 0 ) {
            (*v)->release(0);
            v = on_voices_.erase(v);
        }
        else {
            v++;
        }
    }

    //// トリガー処理
    // 新規に押鍵されたキーを対象に処理を行う。
    int num = MIN( m_polyNum, m_pKbdInfo->getOnKeyNum() );    // 処理する最大ノート数はポリ数と押鍵キー数のどちらか少ない方
    for( int i=0; i<num; i++ ) {
        int noteNo = m_pKbdInfo->getNewOnKeyNN(i);        // 新規押鍵キーでi番目に新しいキーのノートNO（NEWキーフラグ付き）
        if(noteNo == -1) break;                            // 新規押鍵キーはなかった→これ以上古い新規押鍵もないはずなので処理終了

        // 既に同じノートNoを発音しているボイスがあれば、リリースする。
        // 例えばアルペジエータでゲートタイム１００％とした時に、この対応がなければ、音がどんどん重なっていく。
        // これは、このアルゴリズムではnote on/offは鍵盤の状態を変更するだけで、もし同一時刻にoff→onの順で同時にきた場合、
        // 結果的にnoteoffは無視される事になるため。
        // このような事にならないために、同一ノートの複数ボイス発音は不可とする。
        for(std::list<Voice*>::iterator v=on_voices_.begin();v != on_voices_.end();){
            if((*v)->getNoteNo()==noteNo){
                (*v)->release(0);
                v=on_voices_.erase(v);
            }else v++;
        }

        // ユニゾンボイスの数だけ、トリガー処理
        Voice* pUnisonMasterVoice;
        for(int u = 0; u < m_unisonNum; u++){
            //// トリガーするボイスの決定
            // キーオフされているボイスを取得。もし全部ビジーだったら、一番古くからオンになっているボイスを取得
            // ※ただし、ベース音は除くなどの工夫の余地はある
            Voice* v = getNextOffVoice();
            if(v == NULL){
                v = on_voices_.front();    // オン中で一番古いボイス
                if(v == NULL) return;
                on_voices_.pop_front();
            }
            m_curVoiceNo = v->getNo();
            if(u==0) pUnisonMasterVoice = v; // ユニソンマスターボイスの退避

            // ノートNO等の設定とトリガー
            v->setNoteInfo(noteNo,m_pKbdInfo->getVelocity(noteNo));
            v->setUnisonInfo(pUnisonMasterVoice,m_unisonNum,u);
            v->trigger();

            // オンボイスリストへ追加
            on_voices_.push_back(v);
        }
    }
}

// モノモード時のトリガー/リリースを制御
void VoiceCtrl::controlVoiceMono()
{
    // なにもキーがおさえられていなければ、現在のオンボイスをリリースして終わり
    if(m_pKbdInfo->getOnKeyNum()==0){
        for(int i=0;i < m_unisonNum; i++){
            on_voices_.remove(m_pVoices[i]);
            if(m_pVoices[i]->isKeyOn()) m_pVoices[i]->release(0);
        }
        m_monoCurrentVelocity = 0;
        return;
    }

    // なにかキーがおさえられているので一番新しい鍵盤をとってきて、発音が必要か判定
    // 1.新規の押鍵なら無条件に発音
    // 2.新規ではない場合、ボイスがオン中でないか、オン中でもノートNOが違えば発音
    BOOL bProcess=FALSE;
    int noteNo = m_pKbdInfo->getNewOnKeyNN(0);
    if(noteNo != -1){
        bProcess=TRUE;
        m_monoCurrentVelocity = m_pKbdInfo->getVelocity(noteNo);
    }else{
        // 新規ではない→何かキーが離された結果、発音される事になったノート
        // このベロシティは離されたキーと同じとする。よって、m_monoCurrentVelocityは更新しない。
        noteNo = m_pKbdInfo->getOnKeyNN(0);
        if(!m_pVoices[0]->isKeyOn() || m_pVoices[0]->getNoteNo() != noteNo) bProcess = TRUE;
    }

    if(!bProcess) return;    // 発音不要

    // 発音処理
    Voice* pUnisonMasterVoice = m_pVoices[0];    // ユニゾンマスターは常にボイス番号ゼロ
    if(m_keyMode==KM_MONO){
        // モノモード時
        // 必ずトリガー
        for(int i=0;i < m_unisonNum; i++) {
            Voice* v = m_pVoices[i];
            // ノートNO等の設定とトリガー
            v->setNoteInfo(noteNo,m_monoCurrentVelocity);
            v->setUnisonInfo(pUnisonMasterVoice,m_unisonNum,0);
            v->trigger();

            // オンボイスリストへ追加 (すでに登録されている可能性があるので、一度削除してから追加)
            on_voices_.remove(v);
            on_voices_.push_back(v);
        }
    }else if(m_keyMode==KM_MONO2){
        // レガートモノモード時
        // 現在キーオフだった場合のみトリガー
        for(int i=0;i < m_unisonNum; i++) {
            Voice* v = m_pVoices[i];
            // ノートNO等の設定とトリガー
            v->setNoteInfo(noteNo,m_monoCurrentVelocity);
            if(!v->isKeyOn()){
                // 現在キーオフ→トリガーし、オンボイスリストへ追加
                v->setUnisonInfo(pUnisonMasterVoice,m_unisonNum,0);
                v->trigger();
                on_voices_.push_back(v);
            }else{
                // 現在オン→トリガーしない
            }
        }
    }
}

// 信号処理（各ボイスの信号処理を行い、ステレオMIXする）
void VoiceCtrl::processSignalLR(int size, SIG_LR* pSig, BOOL* pProcessed){
    // ボイスの信号をモノラルバッファに出力し、ステレオMIXしていく。
    CACHE_ALIGN16 SIGREAL monoSig[MAX_SIGBUF];
    int processedVoiceNum=0;
    for(int i=0;i<m_voiceNum;i++){
        // ボイス毎に信号処理を実行
        if(!m_pVoices[i]->isBusy()) continue;    // 発音中でなければ、処理依頼しない
        BOOL voiceProcessed=FALSE;
        m_pVoices[i]->processSignal(size,monoSig,&voiceProcessed);    // 信号処理
        if(!voiceProcessed) continue;            // 信号処理されていないので、何もしない

        // mono出力をステレオMIX
        // 初めてのMIX処理なら、まず出力バッファを無音にする
        if(processedVoiceNum == 0) for(int j=0;j<size;j++) pSig[j].l = pSig[j].r = 0;
        float pan = m_pVoices[i]->getPan();
        float lGain = (pan<0.5f) ? 1.f : (1.f-pan)*2.f;
        float rGain = (pan>0.5f) ? 1.f : pan*2.f;
        for(int j=0;j<size;j++){
            pSig[j].l += monoSig[j]*lGain;
            pSig[j].r += monoSig[j]*rGain;
        }
        processedVoiceNum++;
        *pProcessed = TRUE;
    }
}


///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 信号処理部
 */
float Voice::Calc( uint32_t w )
{
    float val;
    val = vco.Calc(w);
    val = vcf.Calc(val);
    val = vca.Calc(val);
    return val;
}

bool Voice::IfPlaying()
{
    return key_on_;  // エンベロープを実装するまでkeyon=playingとする
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 波形生成
 *
 * @param[in] w 角速度
 */
float Voice::VCO::Calc( uint32_t w )
{
    Waveform* wf = Waveform::GetInstance();
    float val = wf->GetSine( p_ );
    p_ += w;
    return val;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 減算処理
 *
 * @param[in] val
 */
float Voice::VCF::Calc( float val )
{
    return val;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 音量加工
 *
 * @param[in] val
 */
float Voice::VCA::Calc( float val )
{
    return val;
}
