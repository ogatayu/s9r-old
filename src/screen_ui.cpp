/**
 * @file screen_ui.cpp
 */
#include <thread>
#include <chrono>
#include <stdio.h>

#include <fmt/format.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "nanovg.h"
#define NANOVG_GLES3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "fifo.h"

#include "synth.h"

#include "screen_ui.h"
#include "keyctrl.h"


static void sui_key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );

ScreenUI* ScreenUI::instance_ = nullptr;


/**
 * @brief Create
 */
ScreenUI* ScreenUI::Create()
{
    if (!instance_)
    {
        instance_ = new ScreenUI();
        if(!instance_->Initialize()) {
            delete instance_;
            instance_ = nullptr;
        }
    }
    return instance_;
}

/**
 * @brief Destroy
 */
void ScreenUI::Destroy()
{
    delete instance_;
    instance_ = nullptr;
}

/**
 * @brief GetInstance
 */
ScreenUI* ScreenUI::GetInstance()
{
    return instance_;
}

/**
 * @brief Initialize
 */
bool ScreenUI::Initialize()
{
    frame_count_ = 0;
    waveform_ = new FIFO( kSampleNum, sizeof(float) );

    ////////////////////////////////////////////////////////////////
    // GLFW initialize
    if (!glfwInit()) {
        fprintf(stderr,"glfwInit fail\n");
        return false;
    }

#ifdef NDEBUG
    glfw_window_ = glfwCreateWindow( kWidth, kHeight, "s9r", glfwGetPrimaryMonitor(), NULL );
#else
    glfw_window_ = glfwCreateWindow( kWidth*2, kHeight*2, "s9r", NULL, NULL );
#endif
    if (!glfw_window_) {
        glfwTerminate();
        fprintf(stderr,"glfwCreateWindow fail\n");
        return false;
    }

    glfwSetWindowUserPointer(glfw_window_, this);

    // set callback
    glfwSetKeyCallback(glfw_window_, sui_key_callback);

    glfwMakeContextCurrent(glfw_window_);
    gl3wInit();

    glfwSwapInterval(1);

    ////////////////////////////////////////////////////////////////
    // nanovg initialize
    vg_ = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if(!vg_) {
        glfwTerminate();
        fprintf(stderr,"nvgCreateGLES3 fail\n");
        return false;
    }

    // font init
    int fnt = nvgCreateFont(vg_, "sans-bold", "./fonts/Roboto-Bold.ttf");
    if( fnt == -1) {
        printf("Could not add font bold.\n");
        return false;
    }

    return true;
}

/**
 * @brief Start
 */
void ScreenUI::Start()
{
    using clock = std::chrono::high_resolution_clock;
    auto wait_time = std::chrono::nanoseconds(int(1e9f / kFPS));
    auto base_time = clock::now();
    while (!glfwWindowShouldClose(glfw_window_)) {
        info_pos_y = 15.f;

        WaveformGet(wavedata_, kSampleNum);

        glClearColor(0.1f, 0.1f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        nvgBeginFrame(vg_, kWidth, kHeight, 1.0);
        {
            DrawFps();
            DrawProcTime();
            DrawWaveform();
        }
        nvgEndFrame(vg_);

        glfwSwapBuffers(glfw_window_);
        glfwPollEvents();

        // fps control
        auto last_time = clock::now() - base_time;
        base_time = clock::now();
        std::this_thread::sleep_for(wait_time-last_time);

        auto nanosec = std::chrono::duration_cast< std::chrono::nanoseconds >( last_time ).count();
        now_fps_ = 1e9f / nanosec;

        frame_count_++;
    }

    nvgDeleteGLES3(instance_->vg_);
    glfwTerminate();
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief DrawWaveform
 */
void ScreenUI::DrawWaveform()
{
    float w = (float)kWidth / (kSampleNum-1);
    nvgBeginPath(vg_);
    nvgMoveTo( vg_, -1.0f, (wavedata_[0]*100.0f) + (kHeight/2) );
    for(int ix=0; ix<kSampleNum; ix++) {
        nvgLineTo( vg_, (float)ix * w, (wavedata_[ix]*100.0f) + (kHeight/2) );
    }
    nvgStrokeColor(vg_, nvgRGBA(255,255,255,200));
    nvgStrokeWidth(vg_, 2.0f);
    nvgLineJoin(vg_, NVG_ROUND);
    nvgLineCap(vg_, NVG_ROUND);
    nvgStroke(vg_);
}

/**
 * @brief DrawFps
 */
void ScreenUI::DrawFps()
{
    nvgFontSize(vg_, 15.0f);
    nvgFontFace(vg_, "sans-bold");
    nvgTextAlign(vg_, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgFillColor(vg_, nvgRGBA(255,255,255,200));
    nvgText(vg_, 10, info_pos_y, fmt::format("FPS: {:.2f}", now_fps_).c_str(), NULL);
    info_pos_y += 15;
}

/**
 * @brief DrawProcTime
 */
void ScreenUI::DrawProcTime()
{
    Synth* synth = Synth::GetInstance();
    static uint32_t now_time = 0; // nanosecond
    static uint32_t max_time = 0;
    static uint32_t max_time_hold_count = 0;  // max time hold time(count);
    static uint32_t min_time = 0;
    float w = (float)kWidth / 20833.3333f;

    if( frame_count_ % 5 == 0 ) {
        now_time = synth->GetProcTime();
        if( max_time < now_time ) {
            max_time = now_time;
            max_time_hold_count = 0;
        }
        else {
            // hold max time
            if( max_time_hold_count > 30 ) {
                max_time = now_time;
            }
            max_time_hold_count++;
        }

        if( (min_time > now_time) || (min_time == 0) ) {
            min_time = now_time;
        }
    }

    // text
    nvgFontSize(vg_, 15.0f);
    nvgFontFace(vg_, "sans-bold");
    nvgTextAlign(vg_, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgFillColor(vg_, nvgRGBA(255,255,255,200));
    nvgText(vg_, 10, info_pos_y, fmt::format("SigProcTime: {}", now_time).c_str(), NULL);

    //max
    nvgBeginPath(vg_);
    nvgMoveTo( vg_, (-1.0f*w), (info_pos_y+15) );
    nvgLineTo( vg_, (w*max_time), (info_pos_y+15) );
    nvgStrokeColor(vg_, nvgRGBA(255,0,0,150));
    nvgStrokeWidth(vg_, 4.0f);
    nvgStroke(vg_);

    // now
    nvgBeginPath(vg_);
    nvgMoveTo( vg_, (-1.0f*w), (info_pos_y+15) );
    nvgLineTo( vg_, (w*now_time), (info_pos_y+15) );
    nvgStrokeColor(vg_, nvgRGBA(0,255,0,255));
    nvgStrokeWidth(vg_, 4.0f);
    nvgStroke(vg_);

    // min
    nvgBeginPath(vg_);
    nvgMoveTo( vg_, (-1.0f*w), (info_pos_y+15) );
    nvgLineTo( vg_, (w*min_time), (info_pos_y+15) );
    nvgStrokeColor(vg_, nvgRGBA(0,0,255,255));
    nvgStrokeWidth(vg_, 4.0f);
    nvgStroke(vg_);


    info_pos_y += 35;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief WaveformPut
 */
void ScreenUI::WaveformPut( float wavedata_ )
{
    waveform_->Put( &wavedata_ );
}

/**
 * @brief WaveformGet
 */
void ScreenUI::WaveformGet( float* buf, int num )
{
    waveform_->SnoopFromTail(buf, num);
    //memcpy( buf, waveform_->buffer_, sizeof(float) * num );
    return;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief sui_get_for
 */
ScreenUI* sui_get_for(GLFWwindow* glfw_window)
{
    return static_cast<ScreenUI*>(glfwGetWindowUserPointer(glfw_window));
}

/**
 * @brief sui_key_callback
 */
static void sui_key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
{
    ScreenUI* sui = sui_get_for(glfw_window);
    sui->keyctrl_.KeyEventHandle( key, action );
}
