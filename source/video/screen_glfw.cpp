/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "global.hpp"
#include "../context.hpp"
#include "controller/controller.hpp"
#include "video/video.hpp"
#include "video/scancode.hpp"
#include "video/screen_glfw.hpp"

// Boost
#include <boost/algorithm/string.hpp>

// Constructor and destructor
c_video_screen_glfw::c_video_screen_glfw(
    uint id, const std::string& name, const std::string& color,
    int view_cols, int view_rows,
    int pos_x, int pos_y, int width, int height, bool fullscreen
) :
    // Window
    m_window_width(width), m_window_height(height), m_window(nullptr)
{
    // GLFW
    if (!glfwInit())
        throw c_exception("Screen: Failed to initalize GLFW!", { throw_format("name", name) });

    // Shared context
    GLFWwindow* share = nullptr;

    // Context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);

    // Fullscreen
    GLFWmonitor* monitor = nullptr;
    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        width = mode->width;
        height = mode->height;
    }

    // Window
    std::string title = std::string("madj - ") + name;
    m_window = glfwCreateWindow(width, height, title.c_str(), monitor, share);
    if (!m_window)
        throw c_exception("Screen: Failed to create window!", { throw_format("name", name) });

    // Size
    glfwGetFramebufferSize(m_window, &m_window_width, &m_window_height);
    if (m_window_width <= 0 || m_window_height <= 0)
        throw c_exception("Screen: Failed to get window size!", { throw_format("name", name) });
}

c_video_screen_glfw::~c_video_screen_glfw()
{
    // Window
    if (m_window)
        glfwDestroyWindow(m_window);
}
