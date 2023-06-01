#pragma once

#include "Hittable.h"

class Material
{
public:
    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material
{
public:
    Lambertian(const Color& albedo) : m_Albedo(albedo) {}

    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
    {
        auto scatterDirection = rec.Normal + GetRandomUnitVector();

        // Catch degenerate scatter direction
        if (scatterDirection.IsNearZero())
            scatterDirection = rec.Normal;

        scattered   = Ray(rec.Point, scatterDirection);
        attenuation = m_Albedo;
        return true;
    }

private:
    Color m_Albedo;
};

class Metal : public Material
{
public:
    Metal(const Color& albedo, double fuzz) : m_Albedo(albedo), m_Fuzz(fuzz < 1 ? fuzz : 1) {}

    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
    {
        Vector3 reflected = Reflect(Normalize(rIn.Direction()), rec.Normal);
        scattered         = Ray(rec.Point, reflected + m_Fuzz * GetRandomInUnitSphere());
        attenuation       = m_Albedo;
        return DotProduct(scattered.Direction(), rec.Normal) > 0;
    }

private:
    Color  m_Albedo;
    double m_Fuzz;
};