/**
 * @file screen_ui.h
 */
#pragma once

#include "fifo.h"

struct GLFWwindow;
struct NVGcontext;

/**
 * @class ScreenUI
 */
class ScreenUI {
private:
    ScreenUI(){}
    ~ScreenUI(){}

    ScreenUI(const ScreenUI&);
    ScreenUI& operator=(const ScreenUI&);
    static ScreenUI* instance_;

    bool Initialize();

    static const int kWidth  = 480;
    static const int kHeight = 320;
    static const int kSampleNum = 256;

    FIFO* waveform_;
    float wavedata_[kSampleNum] = { 0 };

public:
    static ScreenUI* Create();
    static void  Destroy();
    static ScreenUI* GetInstance();

    // context info
    GLFWwindow* glfw_window_;
    NVGcontext* vg_;

    void Start();

    void WaveformPut( float data );
    void WaveformGet( float* buf, int num );
};
