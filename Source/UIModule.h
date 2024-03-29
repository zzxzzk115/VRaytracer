#pragma once

#include "IRuntimeModule.h"
#include "Renderer.h"

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
        void SetDarkThemeColors();

    private:
        uint32_t     m_RenderTextureWidth, m_RenderTextureHeight;
        RenderConfiguration m_RenderConfig;
        RenderConfiguration m_RenderConfigLastFrame;
        static const char* s_Scenes[];
    };
} // namespace VRaytracer