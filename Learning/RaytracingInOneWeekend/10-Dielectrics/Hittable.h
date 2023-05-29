#pragma once

#include "Common.h"

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
};