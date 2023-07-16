#pragma once

#include "Common.h"
#include "AABB.h"

class Material;

/*
 * A struct that records the hit info.
 */
struct HitRecord
{
    Point3                    Point;
    Vector3                   Normal;
    std::shared_ptr<Material> MaterialPtr;
    double                    T;
    double                    U;
    double                    V;
    bool                      IsFrontFace;

    inline void SetFaceNormal(const Ray& r, const Vector3& outwardNormal)
    {
        IsFrontFace = DotProduct(r.Direction(), outwardNormal) < 0;
        Normal      = IsFrontFace ? outwardNormal : -outwardNormal;
    }
};

/*
 * An abstraction of hittable objects.
 */
class Hittable
{
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const    = 0;
};

class Translate : public Hittable
{
public:
    Translate(std::shared_ptr<Hittable> ptr, const Vector3& displacement) : m_Ptr(ptr), m_Offset(displacement) {}

    virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override
    {
        Ray movedR(ray.Origin() - m_Offset, ray.Direction(), ray.Time());
        if (!m_Ptr->Hit(movedR, tMin, tMax, rec))
        {
            return false;
        }

        rec.Point += m_Offset;
        rec.SetFaceNormal(movedR, rec.Normal);

        return true;
    }

    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
    {
        if (!m_Ptr->BoundingBox(time0, time1, outputBox))
        {
            return false;
        }

        outputBox = AABB(outputBox.GetMin() + m_Offset, outputBox.GetMax() + m_Offset);

        return true;
    }

private:
    std::shared_ptr<Hittable> m_Ptr;
    Vector3                   m_Offset;
};

class RotateY : public Hittable
{
public:
    RotateY(std::shared_ptr<Hittable> ptr, double angle) : m_Ptr(ptr) 
    { 
        double radians = Degrees2Radians(angle);
        m_SinTheta     = sin(radians);
        m_CosTheta     = cos(radians);
        m_HasBox       = m_Ptr->BoundingBox(0, 1, m_BBox);

        Point3 min(Infinity, Infinity, Infinity);
        Point3 max(-Infinity, -Infinity, -Infinity);

        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    double x = i * m_BBox.GetMax().x() + (1 - i) * m_BBox.GetMin().x();
                    double y = j * m_BBox.GetMax().y() + (1 - j) * m_BBox.GetMin().y();
                    double z = k * m_BBox.GetMax().z() + (1 - k) * m_BBox.GetMin().z();

                    double newX = m_CosTheta * x + m_SinTheta * z;
                    double newZ = -m_SinTheta * x + m_CosTheta * z;

                    Vector3 tester(newX, y, newZ);

                    for (int c = 0; c < 3; ++c)
                    {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }
    }

    virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override
    {
        auto origin    = ray.Origin();
        auto direction = ray.Direction();

        origin[0] = m_CosTheta * ray.Origin()[0] - m_SinTheta * ray.Origin()[2];
        origin[2] = m_SinTheta * ray.Origin()[0] + m_CosTheta * ray.Origin()[2];

        direction[0] = m_CosTheta * ray.Direction()[0] - m_SinTheta * ray.Direction()[2];
        direction[2] = m_SinTheta * ray.Direction()[0] + m_CosTheta * ray.Direction()[2];

        Ray rotatedR(origin, direction, ray.Time());

        if (!m_Ptr->Hit(rotatedR, tMin, tMax, rec))
        {
            return false;
        }

        auto point  = rec.Point;
        auto normal = rec.Normal;

        point[0] = m_CosTheta * rec.Point[0] + m_SinTheta * rec.Point[2];
        point[2] = -m_SinTheta * rec.Point[0] + m_CosTheta * rec.Point[2];

        normal[0] = m_CosTheta * rec.Normal[0] + m_SinTheta * rec.Normal[2];
        normal[2] = -m_SinTheta * rec.Normal[0] + m_CosTheta * rec.Normal[2];

        rec.Point = point;
        rec.SetFaceNormal(rotatedR, normal);

        return true;
    }

    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
    {
        outputBox = m_BBox;

        return m_HasBox;
    }

private:
    std::shared_ptr<Hittable> m_Ptr;
    double                    m_SinTheta;
    double                    m_CosTheta;
    bool                      m_HasBox;
    AABB                      m_BBox;
};