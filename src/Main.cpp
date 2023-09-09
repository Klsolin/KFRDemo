//
//  Main.cpp
//  KFRDemo
//
//  Created by klsoline on 2023/09/04.
//

#include <chrono>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "KFREffect.h"

// const int fps = 60;
// const float interval = 1.0f /  fps;

int winWidth = 1920;
int winHeight = 1080;

KFREffect effect;

bool InitEffect(GLFWwindow* window)
{
    effect.Destroy();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    return effect.Init(width, height);
}

void ErrorCallback(int ec, const char* desc)
{
    std::cerr << "error code " << ec << ": " << desc << std::endl;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    effect.Resize(width, height);
    effect.Draw();

    glfwSwapBuffers(window);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(action != GLFW_PRESS)
    {
        return;
    }

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);

    int x = int(xpos * xscale);
    int y = int((height - 1 - ypos) * yscale);

    effect.CursorClick(x, y, button);
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);

    int x = int(xpos * xscale);
    int y = int((height - 1 - ypos) * yscale);

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        effect.CursorDrag(x, y);
    }
    else
    {
        effect.CursorMove(x, y);
    }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        if(key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        else
        {
            effect.KeyPress(key);
        }
    }
    else if(action == GLFW_RELEASE)
    {
        effect.KeyRelease(key);
    }
}

int main(int argc, const char* argv[])
{
    glfwSetErrorCallback(ErrorCallback);

    if(glfwInit() != GLFW_TRUE)
    {
        std::cerr << "Failed to init glfw" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "KFRDemo", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if(!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSwapInterval(0);

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);

    int width, height;
    width = int(winWidth / xscale);
    height = int(winHeight / yscale);

    glfwSetWindowSize(window, width, height);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetKeyCallback(window, KeyCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init glad" << std::endl;
        return -1;
    }

    // std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
    // std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    // std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    // std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    if(!InitEffect(window))
    {
        std::cerr << "Failed to init effect" << std::endl;
        return -1;
    }

    auto start = std::chrono::high_resolution_clock::now();

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(now - start).count();

        // if(elapsed > interval)
        {
            start = now;
            effect.Update(elapsed);
        }

        effect.Draw();

        glfwSwapBuffers(window);
    }

    effect.Destroy();

    glfwTerminate();

    return 0;
}

