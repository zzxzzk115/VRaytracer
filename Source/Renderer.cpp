#include "Renderer.h"
#include "Macro.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VRaytracer
{
    uint32_t Renderer::s_RenderTextureID = 0;

    bool Renderer::Init()
    {
        // Init glad
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            VRT_ERROR("Failed to initialize GLAD");
            return false;
        }

        return true;
    }

    void Renderer::Release()
    {
        // Clean up
        glDeleteTextures(1, &s_RenderTextureID);
    }

    void Renderer::Render(std::shared_ptr<FrameBuffer> frameBuffer) 
    {
        glGenTextures(1, &s_RenderTextureID);
        glBindTexture(GL_TEXTURE_2D, s_RenderTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_RenderTextureID, 0);
        
        glBindTexture(GL_TEXTURE_2D, s_RenderTextureID);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     frameBuffer->Width,
                     frameBuffer->Height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     frameBuffer->Data);
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