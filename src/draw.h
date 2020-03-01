/**
 * @file draw.h
 */
#pragma once

#include "fifo.h"

struct GLFWwindow;
struct NVGcontext;

/**
 * @class Draw
 */
class Draw {
private:
    Draw(){}
    ~Draw(){}

    Draw(const Draw&);
    Draw& operator=(const Draw&);
    static Draw* instance_;

    static const int kWidth  = 480;
    static const int kHeight = 320;
    static const int kSampleNum = 256;

    FIFO* waveform_;
    float wavedata_[kSampleNum] = { 0 };

public:
    static Draw* Create();
    static void  Destroy();
    static Draw* GetInstance();

    // context info
    GLFWwindow* glfw_window_;
    NVGcontext* vg_;

    bool Initialize();
    void Start();

    void WaveformPut( float data );
    void WaveformGet( float* buf, int num );
};
