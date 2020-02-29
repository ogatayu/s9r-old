/**
 * @file draw.cpp
 */

#include <stdio.h>

#include "draw.h"

#include "nanovg.h"
#define NANOVG_GLES3_IMPLEMENTATION
#include "nanovg_gl.h"

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
    int width = 512;
    int height = 512;
    float pixelRatio = 1.0;

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(glfw_window_)) {
        glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        nvgBeginFrame(vg, width, height, pixelRatio);


        nvgEndFrame(vg);

        glfwSwapBuffers(glfw_window_);
        glfwPollEvents();
    }

    nvgDeleteGLES3(vg);

    glfwTerminate();
}