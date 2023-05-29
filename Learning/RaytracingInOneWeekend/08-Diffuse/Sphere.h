#pragma once

#include "Hittable.h"
#include "Vector3.h"

class Sphere : public Hittable
{
public:
    Sphere() {}
    Sphere(Point3 center, double radius) : m_Center(center), m_Radius(radius) {}

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;

private:
    Point3 m_Center;
    double m_Radius;
};

bool Sphere::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
{
    Vector3 oc     = r.Origin() - m_Center;
    double  a      = r.Direction().LengthSquared();
    double  halfB  = DotProduct(oc, r.Direction());
    double  c      = oc.LengthSquared() - m_Radius * m_Radius;

    double delta = halfB * halfB - a * c;
    if (delta < 0)
        return false;

    double sqrtd = std::sqrt(delta);

    // Find the nearest root that lies in the acceptable range.
    double root = (-halfB - sqrtd) / a;
    if (root < tMin || tMax < root)
    {
        root = (-halfB + sqrtd) / a;
        if (root < tMin || tMax < root)
            return false;
    }

    rec.T = root;
    rec.Point = r.At(rec.T);
    Vector3 outwardNormal = (rec.Point - m_Center) / m_Radius;
    rec.SetFaceNormal(r, outwardNormal);

    return true;
}