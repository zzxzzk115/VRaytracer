#pragma once

#include "Common.h"
#include "Perlin.h"

// Disable pedantic warnings for this external library.
#ifdef _MSC_VER
// Microsoft Visual C++ Compiler
#pragma warning(push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Restore warning levels.
#ifdef _MSC_VER
// Microsoft Visual C++ Compiler
#pragma warning(pop)
#endif

#include <iostream>

class Texture
{
public:
    virtual Color GetValue(double u, double v, const Point3& point) const = 0;
};

class SolidColor : public Texture
{
public:
    SolidColor() {}
    SolidColor(Color color) : m_ColorValue(color) {}
    SolidColor(double red, double green, double blue) :
        SolidColor(Color(red, green, blue)) {}

    virtual Color GetValue(double u, double v, const Point3& point) const override { return m_ColorValue; }

private:
    Color m_ColorValue;
};

class CheckerTexture : public Texture
{
public:
    CheckerTexture() { }

    CheckerTexture(std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd) :
        m_Even(std::move(even)), m_Odd(std::move(odd))
    {}

    CheckerTexture(Color c1, Color c2) :
        m_Even(std::make_shared<SolidColor>(c1)), m_Odd(std::make_shared<SolidColor>(c2))
    {}

    virtual Color GetValue(double u, double v, const Point3& point) const override
    {
        auto sines = sin(10 * point.x()) * sin(10 * point.y()) * sin(10 * point.z());
        if (sines < 0)
            return m_Odd->GetValue(u, v, point);
        else
            return m_Even->GetValue(u, v, point);
    }

private:
	std::shared_ptr<Texture> m_Even;
    std::shared_ptr<Texture> m_Odd;
};

class NoiseTexture : public Texture
{
public:
    NoiseTexture() { }
    NoiseTexture(double scale) : m_Scale(scale) { }

    virtual Color GetValue(double u, double v, const Point3& point) const override
    {
        return Color(1, 1, 1) * 0.5 * (1 + sin(m_Scale*point.z() + 10 * m_Noise.Turb(point)));
    }

private:
    Perlin m_Noise;
    double m_Scale;
};

class ImageTexture : public Texture
{
public:
    ImageTexture() : m_Data(nullptr), m_Width(0), m_Height(0), m_BytesPerScanline(0) {}
    ImageTexture(const char* fileName)
    {
    	int componentsPerPixel = BytesPerPixel;

        m_Data = stbi_load(fileName, &m_Width, &m_Height, &componentsPerPixel, componentsPerPixel);

        if (!m_Data)
        {
            std::cerr << "ERROR: Could not load texture image file '" << fileName << "'.\n";
            m_Width = m_Height = 0;
        }

        m_BytesPerScanline = BytesPerPixel * m_Width;
    }

    ~ImageTexture() { delete m_Data; }

    Color GetValue(double u, double v, const Point3& point) const override
    {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (m_Data == nullptr)
            return Color(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = Clamp(u, 0.0, 1.0);
        v = 1.0 - Clamp(v, 0.0, 1.0); // Flip V to image coordinates

        auto i = static_cast<int>(u * m_Width);
        auto j = static_cast<int>(v * m_Height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= m_Width)
            i = m_Width - 1;
        if (j >= m_Height)
            j = m_Height - 1;

        const double   colorScale = 1.0 / 255.0;
        unsigned char* pixel       = m_Data + j * m_BytesPerScanline + i * BytesPerPixel;

        return Color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
    }

private:
    unsigned char* m_Data;
    int            m_Width;
    int            m_Height;
    int            m_BytesPerScanline;

private:
    const static int BytesPerPixel = 3;
};