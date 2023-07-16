#pragma once

#include "Common.h"
#include "AARect.h"
#include "HittableList.h"

class Box : public Hittable
{
public:
    Box() {}
    Box(const Point3& p0, const Point3& p1, std::shared_ptr<Material> ptr) : m_BoxMin(p0), m_BoxMax(p1)
    {
        m_Sides.Add(std::make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
        m_Sides.Add(std::make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

        m_Sides.Add(std::make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
        m_Sides.Add(std::make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

        m_Sides.Add(std::make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
        m_Sides.Add(std::make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
    }

    virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override 
    {
        return m_Sides.Hit(ray, tMin, tMax, rec);
    }

    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
    {
        outputBox = AABB(m_BoxMin, m_BoxMax);
        return true;
    }

private:
    Point3       m_BoxMin;
    Point3       m_BoxMax;
    HittableList m_Sides;
};