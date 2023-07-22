// This is a single header file. It contains all of the functionalities of a software raytracer(path-tracing).

#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <memory>

namespace VRaytracer
{
    class Vector3
    {
    public:
        Vector3() : m_Comp {0, 0, 0} {}
        Vector3(double c1, double c2, double c3) : m_Comp {c1, c2, c3} {}

        double x() const { return m_Comp[0]; }
        double y() const { return m_Comp[1]; }
        double z() const { return m_Comp[2]; }

        Vector3 operator-() const { return Vector3(-m_Comp[0], -m_Comp[1], -m_Comp[2]); }
        double  operator[](int i) const { return m_Comp[i]; }
        double& operator[](int i) { return m_Comp[i]; }

        Vector3 operator+=(const Vector3& v)
        {
            m_Comp[0] += v.m_Comp[0];
            m_Comp[1] += v.m_Comp[1];
            m_Comp[2] += v.m_Comp[2];
            return *this;
        }

        Vector3& operator*=(const double t)
        {
            m_Comp[0] *= t;
            m_Comp[1] *= t;
            m_Comp[2] *= t;
            return *this;
        }

        Vector3& operator/=(const double t) { return *this *= 1 / t; }

        double LengthSquared() const { return m_Comp[0] * m_Comp[0] + m_Comp[1] * m_Comp[1] + m_Comp[2] * m_Comp[2]; }
        double Length() const { return std::sqrt(LengthSquared()); }

    private:
        double m_Comp[3];
    };

    using Point3 = Vector3; // 3D Point
    using Color  = Vector3; // RGB Color

    // Vector3 Utility Functions

    inline std::ostream& operator<<(std::ostream& out, const Vector3& v)
    {
        return out << v.x() << ' ' << v.y() << ' ' << v.z();
    }

    inline Vector3 operator+(const Vector3& u, const Vector3& v)
    {
        return Vector3(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
    }

    inline Vector3 operator-(const Vector3& u, const Vector3& v)
    {
        return Vector3(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
    }

    inline Vector3 operator*(const Vector3& u, const Vector3& v)
    {
        return Vector3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
    }

    inline Vector3 operator*(double t, const Vector3& v) { return Vector3(t * v.x(), t * v.y(), t * v.z()); }

    inline Vector3 operator/(Vector3 v, double t) { return (1 / t) * v; }

    inline double DotProduct(const Vector3& u, const Vector3& v)
    {
        return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
    }

    inline Vector3 CrossProduct(const Vector3& u, const Vector3& v)
    {
        return Vector3(u.y() * v.z() - u.z() * v.y(), u.z() * v.x() - u.x() * v.z(), u.x() * v.y() - u.y() * v.x());
    }

    inline Vector3 Normalize(Vector3 v) { return v / v.Length(); }

    class Ray
    {
    public:
        Ray() {}
        Ray(const Point3& origin, const Vector3& direction) : m_Origin(origin), m_Direction(direction) {}

        Point3  Origin() const { return m_Origin; }
        Vector3 Direction() const { return m_Direction; }

        Point3 At(double t) const { return m_Origin + t * m_Direction; }

    private:
        Point3  m_Origin;
        Vector3 m_Direction;
    };

    // Ray Utility Functions
    inline Color GetRayColor(const Ray& r)
    {
        Vector3 unitDirection = Normalize(r.Direction());

        // t [0, 1]
        double t = 0.5 * (unitDirection.y() + 1.0);

        // Linear Blend: White & Blue
        return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
    }

	struct PixelColor
	{
        PixelColor() {}
        PixelColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : R(r), G(g), B(b), A(a) {}
        PixelColor(const Color& color) 
        { 
            R = static_cast<uint8_t>(255.999 * color.x());
            G = static_cast<uint8_t>(255.999 * color.y());
            B = static_cast<uint8_t>(255.999 * color.z());
            A = 255;
        }

        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;
	};

	struct FrameBuffer
	{
        FrameBuffer(PixelColor* data, int width, int height) : Data(data), Width(width), Height(height) {}
        size_t GetSize() { return Width * Height; }

        PixelColor* Data;
        uint32_t    Width;
        uint32_t    Height;
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
            auto        frameBufferWidth  = config.RenderTargetWidth;
            auto        frameBufferHeight = config.RenderTargetHeight;
            auto        data              = new PixelColor[frameBufferWidth * frameBufferHeight];
            
            for (int j = frameBufferHeight - 1; j >= 0; --j)
            {
                for (int i = 0; i < frameBufferWidth; ++i)
                {
                    Color pixelColor(double(i) / (frameBufferWidth - 1), double(j) / (frameBufferHeight - 1), 0.25);
                    int   index = j * frameBufferWidth + i;
                    data[index] = pixelColor;
                }
            }

            m_FrameBuffer = std::make_shared<FrameBuffer>(data, config.RenderTargetWidth, config.RenderTargetHeight);

            return m_FrameBuffer;
        }

    private:
        std::shared_ptr<FrameBuffer> m_FrameBuffer;
	};
}