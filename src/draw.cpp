/**
 * @file draw.cpp
 */
#include <thread>
#include <chrono>

#include <stdio.h>
#include <math.h>
#include <stdlib.h> // for rand

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
        instance_ = new Draw;
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
    GLFWwindow* glfw_window_ = glfwCreateWindow( 320, 240, "s9r", NULL, NULL );

    if (!glfw_window_) {
        glfwTerminate();
        fprintf(stderr,"glfwCreateWindow fail\n");
        return;
    }
    glfwMakeContextCurrent(glfw_window_);
    gl3wInit();

    struct NVGcontext* vg_ = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if(!vg_) {
        glfwTerminate();
        fprintf(stderr,"nvgCreateGLES3 fail\n");
        return;
    }

    glfwSwapInterval(1);

    float width = 320, height = 240;
    float phase = 0;
    while (!glfwWindowShouldClose(glfw_window_)) {
        WaveformGet(wavedata_, kSampleNum);

        glClearColor(0.1f, 0.1f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        nvgBeginFrame(vg_, width, height, 1.0);
        {
            nvgBeginPath(vg_);
            {
                nvgMoveTo(vg_, 0, wavedata_[0] + 120);

                float w = width / kSampleNum;
                for(int ix=0; ix<kSampleNum; ix++) {
                    nvgLineTo( vg_, (float)ix * w, (wavedata_[ix]*100.0f) + 120 );
                }

                nvgStrokeColor(vg_, nvgRGBA(255,255,255,200));
                nvgStrokeWidth(vg_, 1.0f);
                nvgLineJoin(vg_, NVG_ROUND);
                nvgLineCap(vg_, NVG_ROUND);
                nvgStroke(vg_);
            }
        }
        nvgEndFrame(vg_);

        glfwSwapBuffers(glfw_window_);
        glfwPollEvents();

        // fps control
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
