#pragma once

#include "Hittable.h"

#include <memory>
#include <vector>

class HittableList : public Hittable
{
public:
    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) {}

    void Clear() { m_Objects.clear(); }
    void Add(std::shared_ptr<Hittable> object) { m_Objects.push_back(object); }

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override;

private:
    std::vector<std::shared_ptr<Hittable>> m_Objects;
};

bool HittableList::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const
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
