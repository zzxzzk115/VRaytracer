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

        scattered   = Ray(rec.Point, scatterDirection, rIn.Time());
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
        scattered         = Ray(rec.Point, reflected + m_Fuzz * GetRandomInUnitSphere(), rIn.Time());
        attenuation       = m_Albedo;
        return DotProduct(scattered.Direction(), rec.Normal) > 0;
    }

private:
    Color  m_Albedo;
    double m_Fuzz;
};

class Dielectric : public Material
{
public:
    Dielectric(double indexOfRefraction) : m_IR(indexOfRefraction) {}

    virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
    {
        attenuation = White;
        double refractionRatio = rec.IsFrontFace ? (1.0 / m_IR) : m_IR;

        Vector3 unitDirection = Normalize(rIn.Direction());
        double  cosTheta      = fmin(DotProduct(-unitDirection, rec.Normal), 1.0);
        double  sinTheta      = std::sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = refractionRatio * sinTheta > 1.0;
        Vector3 direction;

        if (cannotRefract || GetReflectance(cosTheta, refractionRatio) > GetRandomDouble())
        {
	        // Must Reflect
            direction = Reflect(unitDirection, rec.Normal);
        }
        else
        {
            direction = Refract(unitDirection, rec.Normal, refractionRatio);
        }

        scattered = Ray(rec.Point, direction, rIn.Time());
        return true;
    }

private:
    static double GetReflectance(double cosine, double refIndex)
    {
	    // Use Schlick's approximation for reflectance.
        double r0 = (1 - refIndex) / (1 + refIndex);
        r0        = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

private:
    double m_IR; // Index of Refraction
};