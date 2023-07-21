#pragma once

#include "Macro.h"
#include "IRuntimeModule.h"
#include "UIModule.h"
#include "Window.h"
#include "RaytracerCore.h"

namespace VRaytracer
{
    struct RaytracerConfiguration
    {

    };

    class Raytracer
    {
    public:
        Raytracer(RaytracerConfiguration config) : m_Config(config) {}

        void                      Run();
        static Ref<Window>        GetWindow() { return s_Window; }
        static Ref<RaytracerCore> GetCore() { return s_Core; }

    private:
        bool Init();
        void Release();

    private:
        RaytracerConfiguration           m_Config;
        std::vector<Ref<IRuntimeModule>> m_RuntimeModules;
        static Ref<RaytracerCore>        s_Core;
        static Ref<Window>               s_Window;
    };

} // namespace VRaytracer