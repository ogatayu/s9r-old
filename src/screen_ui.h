/**
 * @file screen_ui.h
 */
#pragma once

#include "fifo.h"
#include "keyctrl.h"

struct GLFWwindow;
struct NVGcontext;

/**
 * @class ScreenUI
 */
class ScreenUI {
public:
    static ScreenUI* Create();
    static void  Destroy();
    static ScreenUI* GetInstance();

    // context info
    GLFWwindow* glfw_window_;
    NVGcontext* vg_;

    KeyCtrl keyctrl_;

    void Start();

    void WaveformPut( float data );
    void WaveformGet( float* buf, int num );

private:
    ScreenUI(){}
    ~ScreenUI(){}

    ScreenUI(const ScreenUI&);
    ScreenUI& operator=(const ScreenUI&);
    static ScreenUI* instance_;

    bool Initialize();

    static const int kFPS  = 30;
    static const int kWidth  = 480;
    static const int kHeight = 320;
    static const int kSampleNum = 256;

    FIFO* waveform_;
    float wavedata_[kSampleNum];

    float    now_fps_;
    uint32_t frame_count_;

    float info_pos_y;
    void DrawWaveform();
    void DrawFps();
    void DrawProcTime();
};
