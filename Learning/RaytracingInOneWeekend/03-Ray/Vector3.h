#pragma once

#include <iostream>
#include <cmath>

class Vector3
{
public:
    Vector3() : m_Comp {0, 0, 0} {}
    Vector3(double c1, double c2, double c3) : m_Comp {c1, c2, c3} {}

    double x() const { return m_Comp[0]; }
    double y() const { return m_Comp[1]; }
    double z() const { return m_Comp[2]; }

    Vector3 operator-() const { return Vector3(-m_Comp[0], -m_Comp[1], -m_Comp[2]); }
    double  operator[](int i) const { return m_Comp[i]; }
    double& operator[](int i) { return m_Comp[i]; }

    Vector3 operator+=(const Vector3& v)
    {
        m_Comp[0] += v.m_Comp[0];
        m_Comp[1] += v.m_Comp[1];
        m_Comp[2] += v.m_Comp[2];
        return *this;
    }

    Vector3& operator*=(const double t)
	{
    	m_Comp[0] *= t;
        m_Comp[1] *= t;
        m_Comp[2] *= t;
        return *this;
    }

    Vector3& operator/=(const double t) { return *this *= 1 / t; }

    double LengthSquared() const { return m_Comp[0] * m_Comp[0] + m_Comp[1] * m_Comp[1] + m_Comp[2] * m_Comp[2]; }
    double Length() const { return std::sqrt(LengthSquared()); }

private:
    double m_Comp[3];
};

using Point3 = Vector3; // 3D Point

// Vector3 Utility Functions

inline std::ostream& operator<<(std::ostream& out, const Vector3& v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline Vector3 operator+(const Vector3& u, const Vector3& v)
{
    return Vector3(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}

inline Vector3 operator-(const Vector3& u, const Vector3& v)
{
    return Vector3(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
}

inline Vector3 operator*(const Vector3& u, const Vector3& v)
{
    return Vector3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
}

inline Vector3 operator*(double t, const Vector3& v) { return Vector3(t * v.x(), t * v.y(), t * v.z()); }

inline Vector3 operator/(Vector3 v, double t) { return (1 / t) * v; }

inline double DotProduct(const Vector3& u, const Vector3& v) { return u.x() * v.x() + u.y() * v.y() + u.z() * v.z(); }

inline Vector3 CrossProduct(const Vector3& u, const Vector3& v)
{
    return Vector3(u.y() * v.z() - u.z() * v.y(), u.z() * v.x() - u.x() * v.z(), u.x() * v.y() - u.y() * v.x());
}

inline Vector3 Normalize(Vector3 v) { return v / v.Length(); }