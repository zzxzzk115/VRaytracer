#pragma once

#include "Hittable.h"
#include "Vector3.h"

class Sphere : public Hittable
{
public:
    Sphere() {}
    Sphere(Point3 center, double radius, std::shared_ptr<Material> materialPtr) :
        m_Center(center), m_Radius(radius), m_MaterialPtr(materialPtr) {};

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override;

private:
    Point3                    m_Center;
    double                    m_Radius;
    std::shared_ptr<Material> m_MaterialPtr;
};

inline bool Sphere::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
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
    rec.MaterialPtr = m_MaterialPtr;

    return true;
}

inline bool Sphere::BoundingBox(double time0, double time1, AABB& outputBox) const
{
    outputBox =
        AABB(m_Center - Vector3(m_Radius, m_Radius, m_Radius), m_Center + Vector3(m_Radius, m_Radius, m_Radius));
    return true;
}