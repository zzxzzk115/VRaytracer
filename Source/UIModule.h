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

    private:
        void DrawWidgets();

    private:
        bool     m_NeedRenderNewFrame;
        uint32_t m_RenderTextureWidth, m_RenderTextureHeight;
    };
} // namespace VRaytracer