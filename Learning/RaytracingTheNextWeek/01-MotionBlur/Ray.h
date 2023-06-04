#pragma once

#include "Vector3.h"

class Ray
{
public:
    Ray() {}
    Ray(const Point3& origin, const Vector3& direction, double time) :
        m_Origin(origin), m_Direction(direction), m_Time(time)
    {}

    Point3  Origin() const { return m_Origin; }
    Vector3 Direction() const { return m_Direction; }
    double  Time() const { return m_Time; }

    Point3 At(double t) const { return m_Origin + t * m_Direction; }

private:
	Point3  m_Origin;
    Vector3 m_Direction;
    double  m_Time;
};