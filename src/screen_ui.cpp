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
    glfw_window_ = glfwCreateWindow( kWidth, kHeight, "s9r", NULL, NULL );
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
    frame_count_ = 0;

    using clock = std::chrono::high_resolution_clock;
    auto wait_time = std::chrono::nanoseconds(int(1e9f / 30.f));
    float fps = 0;
    auto base_time = clock::now();
    while (!glfwWindowShouldClose(glfw_window_)) {
        WaveformGet(wavedata_, kSampleNum);

        glClearColor(0.1f, 0.1f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        nvgBeginFrame(vg_, kWidth, kHeight, 1.0);
        {
            // fps
            nvgFontSize(vg_, 15.0f);
            nvgFontFace(vg_, "sans-bold");
            nvgTextAlign(vg_, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
            nvgFillColor(vg_, nvgRGBA(255,255,255,200));
            nvgText(vg_, 10, 15, fmt::format("FPS: {:.2f}", fps).c_str(), NULL);

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
        fps = 1e9f / nanosec;

        frame_count_++;
    }

    nvgDeleteGLES3(instance_->vg_);
    glfwTerminate();
}

///////////////////////////////////////////////////////////////////////////////

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


void ScreenUI::DrawProcTime()
{
    Synth* synth = Synth::GetInstance();
    static uint32_t synth_sigproc_time = 0; // nanosecond

    nvgFontSize(vg_, 15.0f);
    nvgFontFace(vg_, "sans-bold");
    nvgTextAlign(vg_, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgFillColor(vg_, nvgRGBA(255,255,255,200));
    nvgText(vg_, 10, 30, fmt::format("SigProcTime: {}", synth_sigproc_time).c_str(), NULL);
    if( frame_count_ % 15 == 0 ) {
        synth_sigproc_time = synth->GetProcTime();
    }

    float w = (float)kWidth / 20833.3333f;
    nvgBeginPath(vg_);
    nvgMoveTo( vg_, -1.0f * w, 45 );
    nvgLineTo( vg_, w * synth_sigproc_time, 45 );
    nvgStrokeColor(vg_, nvgRGBA(255,0,0,200));
    nvgStrokeWidth(vg_, 4.0f);
    nvgStroke(vg_);
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
