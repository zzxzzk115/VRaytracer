#pragma once

#include "IRuntimeModule.h"

namespace VRaytracer
{
    class UIModule : public IRuntimeModule
    {
    public:
        virtual bool Init() override;
        virtual void Update() override;
        virtual void Release() override;
        virtual std::string GetName() override { return "UIModule"; }
    };
} // namespace VRaytracer