#pragma once

#include "Macro.h"

namespace VRaytracer
{
    struct RaytracerConfiguration
    {

    };

    class Raytracer
    {
    public:
        Raytracer(RaytracerConfiguration config) : m_Config(config) {}
        void Run()
        {
            Init();

            while (true)
            {
                VRT_INFO("VRaytracer is running");
            }

            Release();
        }

    private:
        bool Init() {}
        void Release() {}

    private:
        RaytracerConfiguration m_Config;
    };
} // namespace VRaytracer