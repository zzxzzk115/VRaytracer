#pragma once

#include "Common.h"
#include "Hittable.h"

class XYRect : public Hittable
{
public:
    XYRect() {}
    XYRect(double x0, double x1, double y0, double y1, double k, std::shared_ptr<Material> material) :
        m_X0(x0), m_X1(x1), m_Y0(y0), m_Y1(y1), m_K(k), m_Material(material)
    {}

    virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override;
    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
    {
        // The bounding box must have non-zero width in each dimension, so pad the Z
        // dimension a small amount.
        outputBox = AABB(Point3(m_X0, m_Y0, m_K - 0.0001), Point3(m_X1, m_Y1, m_K + 0.0001));
        return true;
    }

private:
    std::shared_ptr<Material> m_Material;
    double                    m_X0, m_X1, m_Y0, m_Y1, m_K;
};

bool XYRect::Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const
{
    double t = (m_K - ray.Origin().z()) / ray.Direction().z();
    if (t < tMin || t > tMax)
    {
        return false;
    }

    double x = ray.Origin().x() + t * ray.Direction().x();
    double y = ray.Origin().y() + t * ray.Direction().y();
    if (x < m_X0 || x > m_X1 || y < m_Y0 || y > m_Y1)
    {
        return false;
    }

    rec.U = (x - m_X0) / (m_X1 - m_X0);
    rec.V = (y - m_Y0) / (m_Y1 - m_Y0);
    rec.T = t;

    Vector3 outwardNormal = Vector3(0, 0, 1);
    rec.SetFaceNormal(ray, outwardNormal);
    rec.MaterialPtr = m_Material;
    rec.Point       = ray.At(t);

    return true;
}

class XZRect : public Hittable
{
public:
    XZRect() {}
    XZRect(double x0, double x1, double z0, double z1, double k, std::shared_ptr<Material> material) :
        m_X0(x0), m_X1(x1), m_Z0(z0), m_Z1(z1), m_K(k), m_Material(material)
    {}

    virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override;
    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
    {
        // The bounding box must have non-zero width in each dimension, so pad the Y
        // dimension a small amount.
        outputBox = AABB(Point3(m_X0, m_K - 0.0001, m_Z0), Point3(m_X1, m_K + 0.0001, m_Z1));
        return true;
    }

private:
    std::shared_ptr<Material> m_Material;
    double                    m_X0, m_X1, m_Z0, m_Z1, m_K;
};

bool XZRect::Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const
{
    double t = (m_K - ray.Origin().y()) / ray.Direction().y();
    if (t < tMin || t > tMax)
    {
        return false;
    }

    double x = ray.Origin().x() + t * ray.Direction().x();
    double z = ray.Origin().z() + t * ray.Direction().z();
    if (x < m_X0 || x > m_X1 || z < m_Z0 || z > m_Z1)
    {
        return false;
    }

    rec.U = (x - m_X0) / (m_X1 - m_X0);
    rec.V = (z - m_Z0) / (m_Z1 - m_Z0);
    rec.T = t;

    Vector3 outwardNormal = Vector3(0, 1, 0);
    rec.SetFaceNormal(ray, outwardNormal);
    rec.MaterialPtr = m_Material;
    rec.Point       = ray.At(t);

    return true;
}

class YZRect : public Hittable
{
public:
    YZRect() {}
    YZRect(double y0, double y1, double z0, double z1, double k, std::shared_ptr<Material> material) :
        m_Y0(y0), m_Y1(y1), m_Z0(z0), m_Z1(z1), m_K(k), m_Material(material)
    {}

    virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override;
    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
    {
        // The bounding box must have non-zero width in each dimension, so pad the X
        // dimension a small amount.
        outputBox = AABB(Point3(m_K - 0.0001, m_Y0, m_Z0), Point3(m_K + 0.0001, m_Y1, m_Z1));
        return true;
    }

private:
    std::shared_ptr<Material> m_Material;
    double                    m_Y0, m_Y1, m_Z0, m_Z1, m_K;
};

bool YZRect::Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const
{
    double t = (m_K - ray.Origin().x()) / ray.Direction().x();
    if (t < tMin || t > tMax)
    {
        return false;
    }

    double y = ray.Origin().y() + t * ray.Direction().y();
    double z = ray.Origin().z() + t * ray.Direction().z();
    if (y < m_Y0 || y > m_Y1 || z < m_Z0 || z > m_Z1)
    {
        return false;
    }

    rec.U = (y - m_Y0) / (m_Y1 - m_Y0);
    rec.V = (z - m_Z0) / (m_Z1 - m_Z0);
    rec.T = t;

    Vector3 outwardNormal = Vector3(1, 0, 0);
    rec.SetFaceNormal(ray, outwardNormal);
    rec.MaterialPtr = m_Material;
    rec.Point       = ray.At(t);

    return true;
}