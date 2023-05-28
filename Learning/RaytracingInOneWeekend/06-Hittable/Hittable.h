#pragma once

#include "Ray.h"

/*
 * A struct that records the hit info.
 */
struct HitRecord
{
    Point3  Point;
    Vector3 Normal;
    double  T;
};

/*
 * An abstraction of hittable objects.
 */
class Hittable
{
public:
    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
};