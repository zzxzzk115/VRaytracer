#pragma once

#include "Vector3.h"

class Ray
{
public:
    Ray() {}
    Ray(const Point3& origin, const Vector3& direction) : m_Origin(origin), m_Direction(direction) {}

    Point3  Origin() const { return m_Origin; }
    Vector3 Direction() const { return m_Direction; }

    Point3 At(double t) const { return m_Origin + t * m_Direction; }

private:
	Point3  m_Origin;
    Vector3 m_Direction;
};

// Ray Utility Functions

inline double HitSphere(const Point3& center, double radius, const Ray& r)
{
    Vector3   oc    = r.Origin() - center;
    double    a     = DotProduct(r.Direction(), r.Direction());
    double    b     = 2.0 * DotProduct(oc, r.Direction());
    double    c     = DotProduct(oc, oc) - radius * radius;
    double    delta = b * b - 4 * a * c;

    if (delta < 0)
        return -1;
    return (-b - std::sqrt(delta)) / (2.0 * a);
}

inline Color GetRayColor(const Ray& r)
{
    // If hit the sphere, return Normal mapped color.
    double t = HitSphere(Point3(0, 0, -1), 0.5, r);
    if (t > 0.0)
    {
        Vector3 normal = Normalize(r.At(t) - Point3(0, 0, -1));
        return 0.5 * Color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
    }

	Vector3 unitDirection = Normalize(r.Direction());

    // t [0, 1]
    t = 0.5 * (unitDirection.y() + 1.0);

    // Linear Blend: White & Blue
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}