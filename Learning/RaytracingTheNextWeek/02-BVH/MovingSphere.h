#pragma once

#include "Common.h"
#include "Hittable.h"

class MovingSphere : public Hittable
{
public:
    MovingSphere() {}
    MovingSphere(Point3                    center0,
                 Point3                    center1,
                 double                    time0,
                 double                    time1,
                 double                    radius,
                 std::shared_ptr<Material> material) :
        m_Center0(center0),
        m_Center1(center1), m_Time0(time0), m_Time1(time1), m_Radius(radius), m_MaterialPtr(material)
    {}

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override;

    Point3 GetCenter(double time) const;

private:
    Point3 m_Center0, m_Center1;
    double m_Time0, m_Time1;
    double m_Radius;
    std::shared_ptr<Material> m_MaterialPtr;
};

inline bool MovingSphere::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
{
    Vector3 oc    = r.Origin() - GetCenter(r.Time());
    double  a     = r.Direction().LengthSquared();
    double  halfB = DotProduct(oc, r.Direction());
    double  c     = oc.LengthSquared() - m_Radius * m_Radius;

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

    rec.T                 = root;
    rec.Point             = r.At(rec.T);
    Vector3 outwardNormal = (rec.Point - GetCenter(r.Time())) / m_Radius;
    rec.SetFaceNormal(r, outwardNormal);
    rec.MaterialPtr = m_MaterialPtr;

    return true;
}

inline bool MovingSphere::BoundingBox(double time0, double time1, AABB& outputBox) const
{
    AABB box0(GetCenter(time0) - Vector3(m_Radius, m_Radius, m_Radius),
              GetCenter(time0) + Vector3(m_Radius, m_Radius, m_Radius));

    AABB box1(GetCenter(time1) - Vector3(m_Radius, m_Radius, m_Radius),
              GetCenter(time1) + Vector3(m_Radius, m_Radius, m_Radius));

    outputBox = GetSurroundingBox(box0, box1);
    return true;
}


Point3 MovingSphere::GetCenter(double time) const
{
    return m_Center0 + ((time - m_Time0) / (m_Time1 - m_Time0)) * (m_Center1 - m_Center0);
}