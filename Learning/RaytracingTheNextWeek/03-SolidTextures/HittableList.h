#pragma once

#include "Hittable.h"

#include <memory>
#include <vector>

class HittableList : public Hittable
{
public:
    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) {}

    std::vector<std::shared_ptr<Hittable>> GetObjects() const { return m_Objects; }
    void Clear() { m_Objects.clear(); }
    void Add(std::shared_ptr<Hittable> object) { m_Objects.push_back(object); }

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;
    virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override;

private:
    std::vector<std::shared_ptr<Hittable>> m_Objects;
};

inline bool HittableList::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
{
	HitRecord tempRec;
    bool      hitAnything = false;
    double    closestSoFar = tMax;

    for (const auto& object : m_Objects)
    {
        if (object->Hit(r, tMin, closestSoFar, tempRec))
        {
            hitAnything = true;
            closestSoFar = tempRec.T;
            rec          = tempRec;
        }
    }

    return hitAnything;
}

inline bool HittableList::BoundingBox(double time0, double time1, AABB& outputBox) const
{
    if (m_Objects.empty())
        return false;

    AABB tempBox;
    bool isFirstBox = true;

    for (const auto& object : m_Objects)
    {
        if (!object->BoundingBox(time0, time1, tempBox))
            return false;

        outputBox = isFirstBox ? tempBox : GetSurroundingBox(outputBox, tempBox);
        isFirstBox = false;
    }

    return true;
}
