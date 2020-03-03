/**
 * @file draw.cpp
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
#include "draw.h"

Draw* Draw::instance_ = nullptr;


/**
 * @brief Destroy
 */
Draw* Draw::Create()
{
    if (!instance_)
    {
        instance_ = new Draw();
        instance_->Initialize();
    }
    return instance_;
}

/**
 * @brief Destroy
 */
void Draw::Destroy()
{
    delete instance_;
    instance_ = nullptr;
}

/**
 * @brief GetInstance
 */
Draw* Draw::GetInstance()
{
    return instance_;
}

/**
 * @brief Initialize
 */
bool Draw::Initialize()
{
    waveform_ = new FIFO( kSampleNum, sizeof(float) );
    return true;
}

/**
 * @brief Start
 */
void Draw::Start()
{
    if (!glfwInit()) {
        fprintf(stderr,"glfwInit fail\n");
        return;
    }

#ifdef NDEBUG
    GLFWwindow* glfw_window_ = glfwCreateWindow( kWidth, kHeight, "s9r", glfwGetPrimaryMonitor(), NULL );
#else
    GLFWwindow* glfw_window_ = glfwCreateWindow( kWidth, kHeight, "s9r", NULL, NULL );
#endif

    if (!glfw_window_) {
        glfwTerminate();
        fprintf(stderr,"glfwCreateWindow fail\n");
        return;
    }
    glfwMakeContextCurrent(glfw_window_);
    gl3wInit();

    glfwSwapInterval(1);

    // nanovg create
    struct NVGcontext* vg_ = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if(!vg_) {
        glfwTerminate();
        fprintf(stderr,"nvgCreateGLES3 fail\n");
        return;
    }

    // font init
    int fnt = nvgCreateFont(vg_, "sans-bold", "./fonts/Roboto-Bold.ttf");
    if( fnt == -1) {
        printf("Could not add font bold.\n");
        return;
    }

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
            nvgBeginPath(vg_);
            {
                // waveform
                float w = (float)kWidth / (kSampleNum-1);
                nvgMoveTo( vg_, -1.0f * w, (wavedata_[0]*100.0f) + (kHeight/2) );
                for(int ix=0; ix<kSampleNum; ix++) {
                    nvgLineTo( vg_, (float)ix * w, (wavedata_[ix]*100.0f) + (kHeight/2) );
                }

                nvgStrokeColor(vg_, nvgRGBA(255,255,255,200));
                nvgStrokeWidth(vg_, 2.0f);
                nvgLineJoin(vg_, NVG_ROUND);
                nvgLineCap(vg_, NVG_ROUND);
                nvgStroke(vg_);

                // infomation
                nvgFontSize(vg_, 15.0f);
                nvgFontFace(vg_, "sans-bold");
                nvgTextAlign(vg_, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
                nvgFillColor(vg_, nvgRGBA(255,255,255,200));
                nvgText(vg_, 10, 15, fmt::format("FPS: {:.2f}", fps).c_str(), NULL);
            }
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
    }

    nvgDeleteGLES3(instance_->vg_);
    glfwTerminate();
}


///////////////////////////////////////////////////////////////////////////////

void  Draw::WaveformPut( float wavedata_ )
{
    waveform_->Put( &wavedata_ );
}

void Draw::WaveformGet( float* buf, int num )
{
    waveform_->SnoopFromTail(buf, num);
    //memcpy( buf, waveform_->buffer_, sizeof(float) * num );
    return;
}
