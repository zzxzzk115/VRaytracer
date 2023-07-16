#pragma once

#include <string>

namespace VRaytracer
{
    class IRuntimeModule
    {
    public:
        virtual bool        Init()    = 0;
        virtual void        Update()  = 0;
        virtual void        Release() = 0;
        virtual std::string GetName() = 0;
    };
} // namespace VRaytracer