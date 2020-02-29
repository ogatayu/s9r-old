/**
 * @file draw.h
 */
#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Draw {
private:
    GLFWwindow* glfw_window_;

public:
    Draw(){}
    ~Draw(){}

    bool Initialize();
    void Start();
};
