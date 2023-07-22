#pragma once

#include "UIModule.h"
#include "Macro.h"
#include "Raytracer.h"
#include "Renderer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace VRaytracer
{
    bool UIModule::Init()
    {
        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glslVersion = "#version 100";
#elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glslVersion = "#version 150";
#else
        // GL 3.0 + GLSL 130
        const char* glslVersion = "#version 130";
#endif

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
        // io.ConfigViewportsNoAutoMerge = true;
        // io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
        // ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        auto window = Raytracer::GetWindow()->GetNativeWindow();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glslVersion);

        return true;
    }

    void UIModule::Update() 
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw Widgets
        DrawWidgets();

        // Rendering
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Render();
        auto window       = Raytracer::GetWindow();
        auto nativeWindow = window->GetNativeWindow();

        Renderer::SetViewport(0, 0, window->GetWidth(), window->GetHeight());

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backupCurrentContext);
        }

        glfwSwapBuffers(nativeWindow);
    }

    void UIModule::Release() 
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void UIModule::DrawWidgets() 
    {
        Renderer::Clear(0.45f, 0.55f, 0.60f, 1.00f);

        // Draw Menubar & Toolbar

        // Draw Control Panel        
        ImGui::Begin("Control Panel");
        bool needRenderNewFrame = ImGui::Button("Render"); 
        ImGui::End();

        // Draw RenderTarget
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("RenderTarget");
        ImGui::PopStyleVar();
        if (needRenderNewFrame)
        {
            auto         size = ImGui::GetContentRegionAvail();
            if (size.x <= 0 || size.y <= 0)
            {
                size = {600, 400};
            }
            RenderConfig renderConfig;
            renderConfig.RenderTargetWidth  = size.x;
            renderConfig.RenderTargetHeight = size.y;
            auto frameBuffer                = Raytracer::GetCore()->Render(renderConfig);
            Renderer::SetViewport(0, 0, frameBuffer->Width, frameBuffer->Height);
            Renderer::Render(frameBuffer);
            m_RenderTextureWidth = frameBuffer->Width;
            m_RenderTextureHeight = frameBuffer->Height;
        }
        auto renderTextureID = Renderer::GetRenderTextureID();
        if (renderTextureID != 0)
        {
            ImVec2 frameBufferSize {(float)m_RenderTextureWidth, (float)m_RenderTextureHeight};
            ImGui::Image((ImTextureID)(intptr_t)renderTextureID, frameBufferSize, {0, 1}, {1, 0});
        }
        ImGui::End();

    }
} // namespace VRaytracer