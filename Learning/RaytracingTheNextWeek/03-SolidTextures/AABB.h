#pragma once

#include "Common.h"

class AABB
{
public:
    AABB() {}
    AABB(const Point3& a, const Point3& b) :
        m_Min(a), m_Max(b) {}

    Point3 GetMin() const { return m_Min; }
    Point3 GetMax() const { return m_Max; }

    bool Hit(const Ray& r, double tMin, double tMax) const
    {
        for (int a = 0; a < 3; a++)
        {
            /*double t0 =
                std::fmin((m_Min[a] - r.Origin()[a]) / r.Direction()[a], (m_Max[a] - r.Origin()[a]) / r.Direction()[a]);
            double t1 =
                std::fmax((m_Min[a] - r.Origin()[a]) / r.Direction()[a], (m_Max[a] - r.Origin()[a]) / r.Direction()[a]);

            tMin = std::fmax(t0, tMin);
            tMax = std::fmin(t1, tMax);*/

            double invD = 1.0 / r.Direction()[a];
            double t0   = (m_Min[a] - r.Origin()[a]) * invD;
            double t1   = (m_Max[a] - r.Origin()[a]) * invD;

            if (invD < 0.0)
            {
                std::swap(t0, t1);
            }

            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;

            if (tMax <= tMin)
            {
                return false;
            }
        }

        return true;
    }

private:
    Point3 m_Min;
    Point3 m_Max;
};

inline AABB GetSurroundingBox(AABB box0, AABB box1)
{
    Point3 small(std::fmin(box0.GetMin().x(), box1.GetMin().x()),
                 std::fmin(box0.GetMin().y(), box1.GetMin().y()),
                 std::fmin(box0.GetMin().z(), box1.GetMin().z()));

    Point3 big(std::fmin(box0.GetMax().x(), box1.GetMax().x()),
               std::fmin(box0.GetMax().y(), box1.GetMax().y()),
               std::fmin(box0.GetMax().z(), box1.GetMax().z()));

    return AABB(small, big);
}