// This is a single header file. It contains all of the functionalities of a software raytracer(path-tracing).

#pragma once

#include <vector>
#include <memory>

namespace VRaytracer
{
	struct Color
	{
        Color() {}
        Color(int r, int g, int b, int a = 1) : R(r), G(g), B(b), A(a) {}

        int R;
        int G;
        int B;
        int A;
	};

	struct FrameBuffer
	{
        FrameBuffer(std::vector<Color> data, int width, int height) : Data(data), Width(width), Height(height) {}
        size_t GetSize() { return Data.size(); }

        std::vector<Color> Data;
        int                Width;
        int                Height;
	};

	class RaytracerCore
	{
    public:
        std::shared_ptr<FrameBuffer> Render()
        {
            // Test code
            std::vector<Color> data;

            for (int i = 0; i < 80000; ++i)
            {
                data.push_back({255, 255, 255, 255});
            }

            m_FrameBuffer = std::make_shared<FrameBuffer>(data, 400, 200);

            // TODO: implement

            return m_FrameBuffer;
        }

    private:
        std::shared_ptr<FrameBuffer> m_FrameBuffer;
	};
}