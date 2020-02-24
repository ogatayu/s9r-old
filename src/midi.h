/**
 * @file midi.h
 */
#pragma once

#include <vector>

class MidiCtrl {
private:
    MidiCtrl(){}
    ~MidiCtrl(){}

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

    void InputCallback( double deltatime, std::vector< unsigned char > *message );

    bool IsStatusChanged();
    bool ResetStatusChange();
};
