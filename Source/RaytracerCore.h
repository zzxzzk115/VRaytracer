// This is a single header file. It contains all of the functionalities of a software raytracer(path-tracing).

#pragma once

#include <vector>
#include <memory>

namespace VRaytracer
{
	struct Color
	{
        Color() {}
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : R(r), G(g), B(b), A(a) {}

        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;
	};

	struct FrameBuffer
	{
        FrameBuffer(Color* data, int width, int height) : Data(data), Width(width), Height(height) {}
        size_t GetSize() { return Width * Height; }

        Color*   Data;
        uint32_t Width;
        uint32_t Height;
	};

    struct RenderConfig
    {
        uint32_t RenderTargetWidth;
        uint32_t RenderTargetHeight;
    };

	class RaytracerCore
	{
    public:
        std::shared_ptr<FrameBuffer> Render(RenderConfig config)
        {
            // Test code
            Color* data = new Color[config.RenderTargetWidth * config.RenderTargetHeight];
            
            for (int i = 0; i < config.RenderTargetWidth * config.RenderTargetHeight; ++i)
            {
                data[i] = {(uint8_t)(rand() % 255), (uint8_t)(rand() % 255), (uint8_t)(rand() % 255), 255};
            }

            m_FrameBuffer = std::make_shared<FrameBuffer>(data, config.RenderTargetWidth, config.RenderTargetHeight);

            // TODO: implement

            return m_FrameBuffer;
        }

    private:
        std::shared_ptr<FrameBuffer> m_FrameBuffer;
	};
}