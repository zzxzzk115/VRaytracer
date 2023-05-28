#pragma once

#include "Vector3.h"

class Ray
{
public:
    Ray() {}
    Ray(const Point3& origin, const Vector3& direction) : m_origin(origin), m_direction(direction) {}

    Point3  Origin() const { return m_origin; }
    Vector3 Direction() const { return m_direction; }

    Point3 At(double t) const { return m_origin + t * m_direction; }

private:
	Point3  m_origin;
    Vector3 m_direction;
};

// Ray Utility Functions
inline Color GetRayColor(const Ray& r)
{
	Vector3 unitDirection = Normalize(r.Direction());

    // t [0, 1]
    double  t             = 0.5 * (unitDirection.y() + 1.0);

    // Linear Blend: White & Blue
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}