#include "Renderer.h"
#include "Macro.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VRaytracer
{
    bool Renderer::Init()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            VRT_ERROR("Failed to initialize GLAD");
            return false;
        }

        return true;
    }

    void Renderer::Clear(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(0, 0, width, height);
    }

} // namespace VRaytracer