#pragma once

#include "IRuntimeModule.h"
#include "Event.h"

namespace VRaytracer
{
    class UIModule : public IRuntimeModule
    {
    public:
        virtual bool Init() override;
        virtual void Update() override;
        virtual void Release() override;
        virtual std::string GetName() override { return "UIModule"; }

        Event<void> EventOnRenderButtonDown;

    private:
        void DrawWidgets();
    };
} // namespace VRaytracer