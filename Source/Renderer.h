#pragma once

#include "Base.h"
#include "RaytracerCore.h"

namespace VRaytracer
{
	class Renderer
	{
    public:
        static bool Init();
        static void Release();
        
		static void Render(const std::shared_ptr<FrameBuffer>& frameBuffer);

		static void Clear(float r, float g, float b, float a);
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static uint32_t GetRenderTextureID() { return s_RenderTextureID; }

	private:
        static uint32_t s_RenderTextureID;
	};
}