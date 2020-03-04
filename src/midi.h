/**
 * @file midi.h
 */
#pragma once

#include <vector>

class MidiCtrl {
private:
    MidiCtrl() {
        on_key_num_        = 0;
        dumper_            = false;
        is_status_changed_ = false;
        for( int ix=0; ix<128; ix++ ) {
            key_table_[ix]      = 0;
            on_key_nn_list_[ix] = 0;
            dumper_table_[ix]   = 0;
        }
    }
    ~MidiCtrl() {}

    MidiCtrl(const MidiCtrl&);
    MidiCtrl& operator=(const MidiCtrl&);
    static MidiCtrl* instance_;

    bool Initialize();

    void KeyOn( int nn, int v );
    void KeyOff( int nn );

    int  on_key_num_;           // 押下中のキー数
    int  key_table_[128];       // キーとベロシティとの対応表
    int  on_key_nn_list_[128];  // 押下されたキーの順番を保持するリスト
    bool dumper_;               // ダンパーペダルフラグ
    bool dumper_table_[128];    // ダンパーオフで、リリースすべき鍵盤情報
    bool is_status_changed_;    // キーの押下状態が変更されたかのフラグ

public:
    static MidiCtrl* Create();
    static void      Destroy();
    static MidiCtrl* GetInstance();

    void MidiRecv( std::vector<unsigned char> *msg );
    void MidiSend( std::vector<unsigned char> *msg ) { MidiRecv(msg); }

    // key management
    bool IsStatusChanged();
    void ResetStatusChange();

    int GetVelocity(int nn) { return key_table_[nn]; };
    int GetOnKeyNum(void)   { return on_key_num_; };

    // n番目に新しい押鍵キーのノートNOを取得。なければ、-1を返す。
    int GetOnKeyNN(int n)
    {
        if(n >= on_key_num_) return -1;
        return on_key_nn_list_[on_key_num_-1-n];
    };

    // n番目に新しい新規押鍵キーのノートNOを取得。なければ、-1を返す。
    int GetNewOnKeyNN(int n)
    {
        if(n >= on_key_num_) return -1;
        int note_num = on_key_nn_list_[on_key_num_ -1-n];
        if(note_num < 256) return -1;		// 新規ではない
        return note_num-256;
    };
};
