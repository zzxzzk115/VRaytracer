#pragma once

#include "Common.h"

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