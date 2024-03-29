#include "Raytracer.h"
#include "Renderer.h"
#include "UIModule.h"

namespace VRaytracer
{
    Ref<Window>        Raytracer::s_Window = nullptr;
    Ref<RaytracerCore> Raytracer::s_Core   = nullptr;

    void Raytracer::Run()
    {
        if (!Init())
        {
            VRT_ERROR("VRaytracer Init Failed!");
            return;
        }

        while (!s_Window->ShouldClose())
        {
            s_Window->Update();

            for (auto& runtimeModule : m_RuntimeModules)
            {
                runtimeModule->Update();
            }
        }

        Release();
    }

    bool Raytracer::Init()
    {
        VRT_INFO("Raytracer Init...");

        // Create Window
        WindowConfiguration windowConfig;
        windowConfig.Width = 1600;
        windowConfig.Height = 960;
        windowConfig.EnableVSync = true;

        s_Window = CreateRef<Window>(windowConfig);
        if (!s_Window->Create())
        {
            return false;
        }

        // Init Renderer
        if (!Renderer::Init())
        {
            return false;
        }

        // Init Core
        s_Core = CreateRef<RaytracerCore>();

        // Init Modules
        auto uiModule = CreateRef<UIModule>();
        m_RuntimeModules.push_back(uiModule);

        for (auto& runtimeModule : m_RuntimeModules)
        {
            if (!runtimeModule->Init())
            {
                VRT_ERROR("RuntimeModule({0}) Init Failed!", runtimeModule->GetName());
                return false;
            }
        }

        VRT_INFO("Raytracer Init OK");

        return true;
    }

    void Raytracer::Release()
    {
        VRT_INFO("Raytracer Release...");

        for (auto& runtimeModule : m_RuntimeModules)
        {
            runtimeModule->Release();
        }
        
        Renderer::Release();
        s_Window->Release();

        VRT_INFO("Raytracer Release OK");
    }

} // namespace VRaytracer