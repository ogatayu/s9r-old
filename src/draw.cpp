/**
 * @file draw.cpp
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h> // for rand

#include "draw.h"

#include "nanovg.h"
#define NANOVG_GLES3_IMPLEMENTATION
#include "nanovg_gl.h"


#define PI (3.141592653589793238462643383279f)
#define SAMPLE_NUM (256)

/**
 * @brief Initialize
 */
bool Draw::Initialize()
{
    return true;
}

/**
 * @brief Start
 */
void Draw::Start()
{
    static float data[SAMPLE_NUM] = { 0 };

    if (!glfwInit()) {
        return;
    }
    GLFWwindow* glfw_window_ = glfwCreateWindow( 320, 240, "s9r", NULL, NULL );

    if (!glfw_window_) {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(glfw_window_);
    gl3wInit();

    struct NVGcontext* vg = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if(!vg) {
        glfwTerminate();
        return;
    }

    glfwSwapInterval(1);

    float width = 320, height = 240;
    float phase = 0;
    while (!glfwWindowShouldClose(glfw_window_)) {
        for(int ix=0; ix<SAMPLE_NUM; ix++) {
            data[ix]  = sin((phase++/50) * PI);
            //data[ix] *= (float)(rand()-(RAND_MAX/2))/(float)RAND_MAX;
        }

        glClearColor(0.1f, 0.1f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        nvgBeginFrame(vg, width, height, 1.0);
        {
            nvgBeginPath(vg);
            {
                nvgMoveTo(vg, 0, data[0] + 100);

                float w = width / SAMPLE_NUM;
                for(int ix=0; ix<SAMPLE_NUM; ix++) {
                    nvgLineTo( vg, (float)ix * w, (data[ix]*100.0f) + 120 );
                }

                nvgStrokeColor(vg, nvgRGBA(255,255,255,200));
                nvgStrokeWidth(vg, 2.0f);
                nvgLineJoin(vg, NVG_ROUND);
                nvgLineCap(vg, NVG_ROUND);
                nvgStroke(vg);
            }
        }
        nvgEndFrame(vg);

        glfwSwapBuffers(glfw_window_);
        glfwPollEvents();
    }

    nvgDeleteGLES3(vg);

    glfwTerminate();
}