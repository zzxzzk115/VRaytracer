#include "Window.h"
#include "Macro.h"

#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description)
{
    VRT_ERROR("GLFW Error ({0}): {1}", error, description);
}

namespace VRaytracer
{
    bool Window::Create() 
    { 
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
        {
            VRT_ERROR("GLFW Init Failed!");
            return false;
        }

        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
        // GL 3.2
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
        // GL 3.0
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

        // Create window with graphics context
        m_NativeWindow = glfwCreateWindow(m_Config.Width, m_Config.Height, "VRaytracer", nullptr, nullptr);
        if (m_NativeWindow == nullptr)
        {
            VRT_ERROR("GLFW Create Window Failed!");
            return false;
        }

        glfwMakeContextCurrent(m_NativeWindow);
        glfwSwapInterval(m_Config.EnableVSync);

        return true;
    }

    void Window::Update() { glfwPollEvents(); }

    bool Window::ShouldClose() { return glfwWindowShouldClose(m_NativeWindow); }

    void Window::Release()
    {
        glfwDestroyWindow(m_NativeWindow);
        glfwTerminate();
    }
} // namespace VRaytracer