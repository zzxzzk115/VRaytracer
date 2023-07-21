#pragma once

#include "Base.h"

namespace VRaytracer
{
	class Renderer
	{
    public:
        static bool Init();

		static void Clear(float r, float g, float b, float a);
        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	};
}