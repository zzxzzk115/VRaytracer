// This is a single header file. It contains all of the functionalities of a software raytracer(path-tracing).

#pragma once

#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace VRaytracer
{
    // Constants

    const double Infinity = std::numeric_limits<double>::infinity();
    const double Pi       = 3.1415926535897932385;

    // Utility Functions

    inline double Degrees2Radians(double degrees) { return degrees * Pi / 180.0; }

    inline double GetRandomDouble()
    {
        // Returns a random real in [0, 1).
        return rand() / (RAND_MAX + 1.0);
    }

    inline double GetRandomDouble(double min, double max)
    {
        // Returns a random real in [min, max).
        return min + (max - min) * GetRandomDouble();
    }

    inline int GetRandomInt(int min, int max) { return static_cast<int>(GetRandomDouble(min, max + 1)); }

    inline double Clamp(double x, double min, double max)
    {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }

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

        bool IsNearZero() const
        {
            // Return true if the vector is close to zero in all dimensions.
            const double s = 1e-8;
            return (fabs(m_Comp[0]) < s) && (fabs(m_Comp[1]) < s) && (fabs(m_Comp[2]) < s);
        }

        static Vector3 GetRandom() { return {GetRandomDouble(), GetRandomDouble(), GetRandomDouble()}; }
        static Vector3 GetRandom(double min, double max)
        {
            return {GetRandomDouble(min, max), GetRandomDouble(min, max), GetRandomDouble(min, max)};
        }

    private:
        double m_Comp[3];
    };

    using Point3 = Vector3;          // 3D Point

    using Color           = Vector3; // RGB Color
    const Color Black     = Color(0, 0, 0);
    const Color White     = Color(1, 1, 1);
    const Color LightBlue = Color(0.5, 0.7, 1.0);

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

    inline Vector3 operator*(const Vector3& v, double t) { return Vector3(t * v.x(), t * v.y(), t * v.z()); }

    inline Vector3 operator/(Vector3 v, double t) { return (1 / t) * v; }

    inline double DotProduct(const Vector3& u, const Vector3& v)
    {
        return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
    }

    inline Vector3 CrossProduct(const Vector3& u, const Vector3& v)
    {
        return Vector3(u.y() * v.z() - u.z() * v.y(), u.z() * v.x() - u.x() * v.z(), u.x() * v.y() - u.y() * v.x());
    }

    inline Vector3 Normalize(Vector3 v) { return v / v.Length(); }

    inline Vector3 GetRandomInUnitSphere()
    {
        while (true)
        {
            auto p = Vector3::GetRandom(-1, 1);
            if (p.LengthSquared() >= 1)
                continue;
            return p;
        }
    }

    inline Vector3 GetRandomUnitVector() { return Normalize(GetRandomInUnitSphere()); }

    inline Vector3 GetRandomInHemisphere(const Vector3& normal)
    {
        Vector3 inUnitSphere = GetRandomInUnitSphere();
        if (DotProduct(inUnitSphere, normal) > 0.0) // In the same hemisphere as the normal
        {
            return inUnitSphere;
        }

        return -inUnitSphere;
    }

    inline Vector3 GetRandomInUnitDisk()
    {
        while (true)
        {
            Vector3 p = Vector3(GetRandomDouble(-1, 1), GetRandomDouble(-1, 1), 0);
            if (p.LengthSquared() >= 1)
                continue;
            return p;
        }
    }

    inline Vector3 Reflect(const Vector3& v, const Vector3& n) { return v - 2 * DotProduct(v, n) * n; }

    inline Vector3 Refract(const Vector3& uv, const Vector3& n, double refractionRatio)
    {
        double  cosTheta          = fmin(DotProduct(-uv, n), 1.0);
        Vector3 rOutPerpendicular = refractionRatio * (uv + cosTheta * n);
        Vector3 rOutParallel      = -std::sqrt(fabs(1.0 - rOutPerpendicular.LengthSquared())) * n;
        return rOutPerpendicular + rOutParallel;
    }

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

    class AABB
    {
    public:
        AABB() {}
        AABB(const Point3& a, const Point3& b) : m_Min(a), m_Max(b) {}

        Point3 GetMin() const { return m_Min; }
        Point3 GetMax() const { return m_Max; }

        bool Hit(const Ray& r, double tMin, double tMax) const
        {
            for (int a = 0; a < 3; a++)
            {
                /*double t0 =
                    std::fmin((m_Min[a] - r.Origin()[a]) / r.Direction()[a], (m_Max[a] - r.Origin()[a]) /
                r.Direction()[a]); double t1 = std::fmax((m_Min[a] - r.Origin()[a]) / r.Direction()[a], (m_Max[a] -
                r.Origin()[a]) / r.Direction()[a]);

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

    class Camera
    {
    public:
        Camera() {}
        Camera(Point3  lookFrom,
               Point3  lookAt,
               Vector3 viewUp,
               double  verticalFOV,
               double  aspectRatio,
               double  aperture,
               double  focusDistance,
               double  time0 = 0,
               double  time1 = 0)
        {
            double       theta          = Degrees2Radians(verticalFOV);
            double       h              = std::tan(theta / 2);
            const double viewportHeight = 2.0 * h;
            const double viewportWidth  = aspectRatio * viewportHeight;

            m_W = Normalize(lookFrom - lookAt);
            m_U = Normalize(CrossProduct(viewUp, m_W));
            m_V = CrossProduct(m_W, m_U);

            m_Origin          = lookFrom;
            m_Horizontal      = focusDistance * viewportWidth * m_U;
            m_Vertical        = focusDistance * viewportHeight * m_V;
            m_LowerLeftCorner = m_Origin - m_Horizontal / 2 - m_Vertical / 2 - focusDistance * m_W;
            m_LensRadius      = aperture / 2;

            m_Time0 = time0;
            m_Time1 = time1;
        }

        Ray GetRay(double s, double t) const
        {
            Vector3 rd     = m_LensRadius * GetRandomInUnitDisk();
            Vector3 offset = m_U * rd.x() + m_V * rd.y();
            return Ray(m_Origin + offset,
                       m_LowerLeftCorner + s * m_Horizontal + t * m_Vertical - m_Origin - offset,
                       GetRandomDouble(m_Time0, m_Time1));
        }

    private:
        Point3  m_Origin;
        Point3  m_LowerLeftCorner;
        Vector3 m_Horizontal;
        Vector3 m_Vertical;
        Vector3 m_U, m_V, m_W;
        double  m_LensRadius;
        double  m_Time0, m_Time1;
    };

    class Perlin
    {
    public:
        Perlin()
        {
            m_RanVec = new Vector3[PointCount];
            for (int i = 0; i < PointCount; ++i)
            {
                m_RanVec[i] = Normalize(Vector3::GetRandom(-1, 1));
            }

            m_PermX = GeneratePerlinPerm();
            m_PermY = GeneratePerlinPerm();
            m_PermZ = GeneratePerlinPerm();
        }

        ~Perlin()
        {
            delete[] m_RanVec;
            delete[] m_PermX;
            delete[] m_PermY;
            delete[] m_PermZ;
        }

        double Turb(const Point3& point, int depth = 7) const
        {
            double accum     = 0.0;
            Point3 tempPoint = point;
            double weight    = 1.0;

            for (int i = 0; i < depth; ++i)
            {
                accum += weight * Noise(tempPoint);
                weight *= 0.5;
                tempPoint *= 2;
            }

            return fabs(accum);
        }

        double Noise(const Point3& point) const
        {
            double u = point.x() - floor(point.x());
            double v = point.y() - floor(point.y());
            double w = point.z() - floor(point.z());

            int i = static_cast<int>(floor(point.x()));
            int j = static_cast<int>(floor(point.y()));
            int k = static_cast<int>(floor(point.z()));

            Vector3 c[2][2][2];

            for (int di = 0; di < 2; ++di)
            {
                for (int dj = 0; dj < 2; ++dj)
                {
                    for (int dk = 0; dk < 2; ++dk)
                    {
                        c[di][dj][dk] =
                            m_RanVec[m_PermX[(i + di) & 255] ^ m_PermY[(j + dj) & 255] ^ m_PermZ[(k + dk) & 255]];
                    }
                }
            }

            return PerlinInterpolate(c, u, v, w);
        }

    private:
        static int* GeneratePerlinPerm()
        {
            auto p = new int[PointCount];

            // Init
            for (int i = 0; i < PointCount; ++i)
            {
                p[i] = i;
            }

            Permute(p, PointCount);

            return p;
        }

        static void Permute(int* p, int n)
        {
            for (int i = n - 1; i > 0; --i)
            {
                int target = GetRandomInt(0, i);
                int tmp    = p[i];
                p[i]       = p[target];
                p[target]  = tmp;
            }
        }

        static double PerlinInterpolate(Vector3 c[2][2][2], double u, double v, double w)
        {
            // Hermitian Smoothing
            u            = u * u * (3 - 2 * u);
            v            = v * v * (3 - 2 * v);
            w            = w * w * (3 - 2 * w);
            double accum = 0.0;

            for (int i = 0; i < 2; ++i)
            {
                for (int j = 0; j < 2; ++j)
                {
                    for (int k = 0; k < 2; ++k)
                    {
                        Vector3 weightVector(u - i, v - j, w - k);
                        accum += (i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) *
                                 (k * w + (1 - k) * (1 - w)) * DotProduct(c[i][j][k], weightVector);
                    }
                }
            }

            return accum;
        }

    private:
        static const int PointCount = 256;

        Vector3* m_RanVec;
        int*     m_PermX;
        int*     m_PermY;
        int*     m_PermZ;
    };

    class Texture
    {
    public:
        virtual Color GetValue(double u, double v, const Point3& point) const = 0;
    };

    class SolidColor : public Texture
    {
    public:
        SolidColor() {}
        SolidColor(Color color) : m_ColorValue(color) {}
        SolidColor(double red, double green, double blue) : SolidColor(Color(red, green, blue)) {}

        virtual Color GetValue(double u, double v, const Point3& point) const override { return m_ColorValue; }

    private:
        Color m_ColorValue;
    };

    class CheckerTexture : public Texture
    {
    public:
        CheckerTexture() {}

        CheckerTexture(std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd) :
            m_Even(std::move(even)), m_Odd(std::move(odd))
        {}

        CheckerTexture(Color c1, Color c2) :
            m_Even(std::make_shared<SolidColor>(c1)), m_Odd(std::make_shared<SolidColor>(c2))
        {}

        virtual Color GetValue(double u, double v, const Point3& point) const override
        {
            auto sines = sin(10 * point.x()) * sin(10 * point.y()) * sin(10 * point.z());
            if (sines < 0)
                return m_Odd->GetValue(u, v, point);
            else
                return m_Even->GetValue(u, v, point);
        }

    private:
        std::shared_ptr<Texture> m_Even;
        std::shared_ptr<Texture> m_Odd;
    };

    class NoiseTexture : public Texture
    {
    public:
        NoiseTexture() {}
        NoiseTexture(double scale) : m_Scale(scale) {}

        virtual Color GetValue(double u, double v, const Point3& point) const override
        {
            return Color(1, 1, 1) * 0.5 * (1 + sin(m_Scale * point.z() + 10 * m_Noise.Turb(point)));
        }

    private:
        Perlin m_Noise;
        double m_Scale;
    };

    // need to link with stb_image
    extern unsigned char* stbi_load(const char* fileName, int* width, int* height, int* pcpp, int cpp);

    class ImageTexture : public Texture
    {
    public:
        ImageTexture() : m_Data(nullptr), m_Width(0), m_Height(0), m_BytesPerScanline(0) {}
        ImageTexture(const char* fileName)
        {
            int componentsPerPixel = BytesPerPixel;

            m_Data = stbi_load(fileName, &m_Width, &m_Height, &componentsPerPixel, componentsPerPixel);

            if (!m_Data)
            {
                std::cerr << "ERROR: Could not load texture image file '" << fileName << "'.\n";
                m_Width = m_Height = 0;
            }

            m_BytesPerScanline = BytesPerPixel * m_Width;
        }

        ~ImageTexture() { delete m_Data; }

        Color GetValue(double u, double v, const Point3& point) const override
        {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (m_Data == nullptr)
                return Color(0, 1, 1);

            // Clamp input texture coordinates to [0,1] x [1,0]
            u = Clamp(u, 0.0, 1.0);
            v = 1.0 - Clamp(v, 0.0, 1.0); // Flip V to image coordinates

            auto i = static_cast<int>(u * m_Width);
            auto j = static_cast<int>(v * m_Height);

            // Clamp integer mapping, since actual coordinates should be less than 1.0
            if (i >= m_Width)
                i = m_Width - 1;
            if (j >= m_Height)
                j = m_Height - 1;

            const double   colorScale = 1.0 / 255.0;
            unsigned char* pixel      = m_Data + j * m_BytesPerScanline + i * BytesPerPixel;

            return Color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
        }

    private:
        unsigned char* m_Data;
        int            m_Width;
        int            m_Height;
        int            m_BytesPerScanline;

    private:
        const static int BytesPerPixel = 3;
    };

    struct Material;

    /*
     * A struct that records the hit info.
     */
    struct HitRecord
    {
        Point3                    Point;
        Vector3                   Normal;
        std::shared_ptr<Material> MaterialPtr;
        double                    T;
        double                    U;
        double                    V;
        bool                      IsFrontFace;

        inline void SetFaceNormal(const Ray& r, const Vector3& outwardNormal)
        {
            IsFrontFace = DotProduct(r.Direction(), outwardNormal) < 0;
            Normal      = IsFrontFace ? outwardNormal : -outwardNormal;
        }
    };

    /*
     * An abstraction of hittable objects.
     */
    class Hittable
    {
    public:
        virtual ~Hittable()                                                            = default;
        virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
        virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const    = 0;
    };

    class Translate : public Hittable
    {
    public:
        Translate(std::shared_ptr<Hittable> ptr, const Vector3& displacement) : m_Ptr(ptr), m_Offset(displacement) {}

        virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override
        {
            Ray movedR(ray.Origin() - m_Offset, ray.Direction(), ray.Time());
            if (!m_Ptr->Hit(movedR, tMin, tMax, rec))
            {
                return false;
            }

            rec.Point += m_Offset;
            rec.SetFaceNormal(movedR, rec.Normal);

            return true;
        }

        virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
        {
            if (!m_Ptr->BoundingBox(time0, time1, outputBox))
            {
                return false;
            }

            outputBox = AABB(outputBox.GetMin() + m_Offset, outputBox.GetMax() + m_Offset);

            return true;
        }

    private:
        std::shared_ptr<Hittable> m_Ptr;
        Vector3                   m_Offset;
    };

    class RotateY : public Hittable
    {
    public:
        RotateY(std::shared_ptr<Hittable> ptr, double angle) : m_Ptr(ptr)
        {
            double radians = Degrees2Radians(angle);
            m_SinTheta     = sin(radians);
            m_CosTheta     = cos(radians);
            m_HasBox       = m_Ptr->BoundingBox(0, 1, m_BBox);

            Point3 min(Infinity, Infinity, Infinity);
            Point3 max(-Infinity, -Infinity, -Infinity);

            for (int i = 0; i < 2; ++i)
            {
                for (int j = 0; j < 2; ++j)
                {
                    for (int k = 0; k < 2; ++k)
                    {
                        double x = i * m_BBox.GetMax().x() + (1 - i) * m_BBox.GetMin().x();
                        double y = j * m_BBox.GetMax().y() + (1 - j) * m_BBox.GetMin().y();
                        double z = k * m_BBox.GetMax().z() + (1 - k) * m_BBox.GetMin().z();

                        double newX = m_CosTheta * x + m_SinTheta * z;
                        double newZ = -m_SinTheta * x + m_CosTheta * z;

                        Vector3 tester(newX, y, newZ);

                        for (int c = 0; c < 3; ++c)
                        {
                            min[c] = fmin(min[c], tester[c]);
                            max[c] = fmax(max[c], tester[c]);
                        }
                    }
                }
            }
        }

        virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override
        {
            auto origin    = ray.Origin();
            auto direction = ray.Direction();

            origin[0] = m_CosTheta * ray.Origin()[0] - m_SinTheta * ray.Origin()[2];
            origin[2] = m_SinTheta * ray.Origin()[0] + m_CosTheta * ray.Origin()[2];

            direction[0] = m_CosTheta * ray.Direction()[0] - m_SinTheta * ray.Direction()[2];
            direction[2] = m_SinTheta * ray.Direction()[0] + m_CosTheta * ray.Direction()[2];

            Ray rotatedR(origin, direction, ray.Time());

            if (!m_Ptr->Hit(rotatedR, tMin, tMax, rec))
            {
                return false;
            }

            auto point  = rec.Point;
            auto normal = rec.Normal;

            point[0] = m_CosTheta * rec.Point[0] + m_SinTheta * rec.Point[2];
            point[2] = -m_SinTheta * rec.Point[0] + m_CosTheta * rec.Point[2];

            normal[0] = m_CosTheta * rec.Normal[0] + m_SinTheta * rec.Normal[2];
            normal[2] = -m_SinTheta * rec.Normal[0] + m_CosTheta * rec.Normal[2];

            rec.Point = point;
            rec.SetFaceNormal(rotatedR, normal);

            return true;
        }

        virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
        {
            outputBox = m_BBox;

            return m_HasBox;
        }

    private:
        std::shared_ptr<Hittable> m_Ptr;
        double                    m_SinTheta;
        double                    m_CosTheta;
        bool                      m_HasBox;
        AABB                      m_BBox;
    };

    class HittableList : public Hittable
    {
    public:
        HittableList() {}
        HittableList(std::shared_ptr<Hittable> object) {}

        std::vector<std::shared_ptr<Hittable>> GetObjects() const { return m_Objects; }
        void                                   Clear() { m_Objects.clear(); }
        void                                   Add(std::shared_ptr<Hittable> object) { m_Objects.push_back(object); }

        virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override
        {
            HitRecord tempRec;
            bool      hitAnything  = false;
            double    closestSoFar = tMax;

            for (const auto& object : m_Objects)
            {
                if (object->Hit(r, tMin, closestSoFar, tempRec))
                {
                    hitAnything  = true;
                    closestSoFar = tempRec.T;
                    rec          = tempRec;
                }
            }

            return hitAnything;
        }

        virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
        {
            if (m_Objects.empty())
                return false;

            AABB tempBox;
            bool isFirstBox = true;

            for (const auto& object : m_Objects)
            {
                if (!object->BoundingBox(time0, time1, tempBox))
                    return false;

                outputBox  = isFirstBox ? tempBox : GetSurroundingBox(outputBox, tempBox);
                isFirstBox = false;
            }

            return true;
        }

    private:
        std::vector<std::shared_ptr<Hittable>> m_Objects;
    };

    class XYRect : public Hittable
    {
    public:
        XYRect() {}
        XYRect(double x0, double x1, double y0, double y1, double k, std::shared_ptr<Material> material) :
            m_X0(x0), m_X1(x1), m_Y0(y0), m_Y1(y1), m_K(k), m_Material(material)
        {}

        virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override
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

    class XZRect : public Hittable
    {
    public:
        XZRect() {}
        XZRect(double x0, double x1, double z0, double z1, double k, std::shared_ptr<Material> material) :
            m_X0(x0), m_X1(x1), m_Z0(z0), m_Z1(z1), m_K(k), m_Material(material)
        {}

        virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override
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

    class YZRect : public Hittable
    {
    public:
        YZRect() {}
        YZRect(double y0, double y1, double z0, double z1, double k, std::shared_ptr<Material> material) :
            m_Y0(y0), m_Y1(y1), m_Z0(z0), m_Z1(z1), m_K(k), m_Material(material)
        {}

        virtual bool Hit(const Ray& ray, double tMin, double tMax, HitRecord& rec) const override
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

    inline bool BoxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis)
    {
        AABB boxA, boxB;

        if (!a->BoundingBox(0, 0, boxA) || !b->BoundingBox(0, 0, boxB))
            std::cerr << "No bounding box in BVHNode constructor." << std::endl;

        return boxA.GetMin()[axis] < boxB.GetMin()[axis];
    }

    inline bool BoxCompareX(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return BoxCompare(a, b, 0);
    }

    inline bool BoxCompareY(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return BoxCompare(a, b, 1);
    }

    inline bool BoxCompareZ(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
    {
        return BoxCompare(a, b, 2);
    }

    class BVHNode : public Hittable
    {
    public:
        BVHNode() {}
        BVHNode(const HittableList& list, double time0, double time1) :
            BVHNode(list.GetObjects(), 0, list.GetObjects().size(), time0, time1)
        {}
        BVHNode(const std::vector<std::shared_ptr<Hittable>>& srcObjects,
                size_t                                        start,
                size_t                                        end,
                double                                        time0,
                double                                        time1)
        {
            auto objects = srcObjects; // Create a modifiable array of the source scene objects

            // chose an axis to compare
            int axis = GetRandomInt(0, 2);

            auto comparator = (axis == 0) ? BoxCompareX : (axis == 1) ? BoxCompareY : BoxCompareZ;

            size_t objectSpan = end - start;

            if (objectSpan == 1)
            {
                m_Left = m_Right = objects[start];
            }
            else if (objectSpan == 2)
            {
                if (comparator(objects[start], objects[start + 1]))
                {
                    m_Left  = objects[start];
                    m_Right = objects[start + 1];
                }
                else
                {
                    m_Left  = objects[start + 1];
                    m_Right = objects[start];
                }
            }
            else
            {
                std::sort(objects.begin() + start, objects.begin() + end, comparator);

                auto mid = start + objectSpan / 2;
                m_Left   = std::make_shared<BVHNode>(objects, start, mid, time0, time1);
                m_Right  = std::make_shared<BVHNode>(objects, mid, end, time0, time1);
            }

            AABB boxLeft, boxRight;

            if (!m_Left->BoundingBox(time0, time1, boxLeft) || !m_Right->BoundingBox(time0, time1, boxRight))
            {
                std::cerr << "No bounding box in BVHNode constructor." << std::endl;
            }

            m_Box = GetSurroundingBox(boxLeft, boxRight);
        }

        virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override
        {
            if (!m_Box.Hit(r, tMin, tMax))
                return false;

            bool hitLeft  = m_Left->Hit(r, tMin, tMax, rec);
            bool hitRight = m_Right->Hit(r, tMin, hitLeft ? rec.T : tMax, rec);

            return hitLeft || hitRight;
        }

        virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
        {
            outputBox = m_Box;
            return true;
        }

    private:
        std::shared_ptr<Hittable> m_Left;
        std::shared_ptr<Hittable> m_Right;
        AABB                      m_Box;
    };

    class Material
    {
    public:
        virtual bool  Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const = 0;
        virtual Color Emitted(double u, double v, const Point3& point) const { return Black; }
    };

    class Lambertian : public Material
    {
    public:
        Lambertian(const Color& albedo) : m_Albedo(std::make_shared<SolidColor>(albedo)) {}
        Lambertian(std::shared_ptr<Texture> albedo) : m_Albedo(std::move(albedo)) {}

        virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
        {
            auto scatterDirection = rec.Normal + GetRandomUnitVector();

            // Catch degenerate scatter direction
            if (scatterDirection.IsNearZero())
                scatterDirection = rec.Normal;

            scattered   = Ray(rec.Point, scatterDirection, rIn.Time());
            attenuation = m_Albedo->GetValue(rec.U, rec.V, rec.Point);
            return true;
        }

    private:
        std::shared_ptr<Texture> m_Albedo;
    };

    class Metal : public Material
    {
    public:
        Metal(const Color& albedo, double fuzz) : m_Albedo(albedo), m_Fuzz(fuzz < 1 ? fuzz : 1) {}

        virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
        {
            Vector3 reflected = Reflect(Normalize(rIn.Direction()), rec.Normal);
            scattered         = Ray(rec.Point, reflected + m_Fuzz * GetRandomInUnitSphere(), rIn.Time());
            attenuation       = m_Albedo;
            return DotProduct(scattered.Direction(), rec.Normal) > 0;
        }

    private:
        Color  m_Albedo;
        double m_Fuzz;
    };

    class Dielectric : public Material
    {
    public:
        Dielectric(double indexOfRefraction) : m_IR(indexOfRefraction) {}

        virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
        {
            attenuation            = White;
            double refractionRatio = rec.IsFrontFace ? (1.0 / m_IR) : m_IR;

            Vector3 unitDirection = Normalize(rIn.Direction());
            double  cosTheta      = fmin(DotProduct(-unitDirection, rec.Normal), 1.0);
            double  sinTheta      = std::sqrt(1.0 - cosTheta * cosTheta);

            bool    cannotRefract = refractionRatio * sinTheta > 1.0;
            Vector3 direction;

            if (cannotRefract || GetReflectance(cosTheta, refractionRatio) > GetRandomDouble())
            {
                // Must Reflect
                direction = Reflect(unitDirection, rec.Normal);
            }
            else
            {
                direction = Refract(unitDirection, rec.Normal, refractionRatio);
            }

            scattered = Ray(rec.Point, direction, rIn.Time());
            return true;
        }

    private:
        static double GetReflectance(double cosine, double refIndex)
        {
            // Use Schlick's approximation for reflectance.
            double r0 = (1 - refIndex) / (1 + refIndex);
            r0        = r0 * r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }

    private:
        double m_IR; // Index of Refraction
    };

    class DiffuseLight : public Material
    {
    public:
        DiffuseLight(std::shared_ptr<Texture> albedo) : m_Emit(albedo) {}
        DiffuseLight(Color color) : m_Emit(std::make_shared<SolidColor>(color)) {}

        virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
        {
            return false;
        }

        virtual Color Emitted(double u, double v, const Point3& point) const override
        {
            return m_Emit->GetValue(u, v, point);
        }

    private:
        std::shared_ptr<Texture> m_Emit;
    };

    class Sphere : public Hittable
    {
    public:
        Sphere() {}
        Sphere(Point3 center, double radius, std::shared_ptr<Material> materialPtr) :
            m_Center(center), m_Radius(radius), m_MaterialPtr(materialPtr) {};

        virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override
        {
            Vector3 oc    = r.Origin() - m_Center;
            double  a     = r.Direction().LengthSquared();
            double  halfB = DotProduct(oc, r.Direction());
            double  c     = oc.LengthSquared() - m_Radius * m_Radius;

            double delta = halfB * halfB - a * c;
            if (delta < 0)
                return false;

            double sqrtd = std::sqrt(delta);

            // Find the nearest root that lies in the acceptable range.
            double root = (-halfB - sqrtd) / a;
            if (root < tMin || tMax < root)
            {
                root = (-halfB + sqrtd) / a;
                if (root < tMin || tMax < root)
                    return false;
            }

            rec.T                 = root;
            rec.Point             = r.At(rec.T);
            Vector3 outwardNormal = (rec.Point - m_Center) / m_Radius;
            rec.SetFaceNormal(r, outwardNormal);
            GetSphereUV(outwardNormal, rec.U, rec.V);
            rec.MaterialPtr = m_MaterialPtr;

            return true;
        }

        virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
        {
            outputBox = AABB(m_Center - Vector3(m_Radius, m_Radius, m_Radius),
                             m_Center + Vector3(m_Radius, m_Radius, m_Radius));
            return true;
        }

    private:
        static void GetSphereUV(const Point3& point, double& u, double& v)
        {
            // point: a given point on the sphere of radius one, centered at the origin.
            // u:     returned value [0,1] of angle around the Y axis from X=-1.
            // v:     returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            double theta = acos(-point.y());
            double phi   = atan2(-point.z(), point.x()) + Pi;

            u = phi / (2 * Pi);
            v = theta / Pi;
        }

    private:
        Point3                    m_Center;
        double                    m_Radius;
        std::shared_ptr<Material> m_MaterialPtr;
    };

    class MovingSphere : public Hittable
    {
    public:
        MovingSphere() {}
        MovingSphere(Point3                    center0,
                     Point3                    center1,
                     double                    time0,
                     double                    time1,
                     double                    radius,
                     std::shared_ptr<Material> material) :
            m_Center0(center0),
            m_Center1(center1), m_Time0(time0), m_Time1(time1), m_Radius(radius), m_MaterialPtr(material)
        {}

        virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override
        {
            Vector3 oc    = r.Origin() - GetCenter(r.Time());
            double  a     = r.Direction().LengthSquared();
            double  halfB = DotProduct(oc, r.Direction());
            double  c     = oc.LengthSquared() - m_Radius * m_Radius;

            double delta = halfB * halfB - a * c;
            if (delta < 0)
                return false;

            double sqrtd = std::sqrt(delta);

            // Find the nearest root that lies in the acceptable range.
            double root = (-halfB - sqrtd) / a;
            if (root < tMin || tMax < root)
            {
                root = (-halfB + sqrtd) / a;
                if (root < tMin || tMax < root)
                    return false;
            }

            rec.T                 = root;
            rec.Point             = r.At(rec.T);
            Vector3 outwardNormal = (rec.Point - GetCenter(r.Time())) / m_Radius;
            rec.SetFaceNormal(r, outwardNormal);
            rec.MaterialPtr = m_MaterialPtr;

            return true;
        }

        virtual bool BoundingBox(double time0, double time1, AABB& outputBox) const override
        {
            AABB box0(GetCenter(time0) - Vector3(m_Radius, m_Radius, m_Radius),
                      GetCenter(time0) + Vector3(m_Radius, m_Radius, m_Radius));

            AABB box1(GetCenter(time1) - Vector3(m_Radius, m_Radius, m_Radius),
                      GetCenter(time1) + Vector3(m_Radius, m_Radius, m_Radius));

            outputBox = GetSurroundingBox(box0, box1);
            return true;
        }

        Point3 GetCenter(double time) const
        {
            return m_Center0 + ((time - m_Time0) / (m_Time1 - m_Time0)) * (m_Center1 - m_Center0);
        }

    private:
        Point3                    m_Center0, m_Center1;
        double                    m_Time0, m_Time1;
        double                    m_Radius;
        std::shared_ptr<Material> m_MaterialPtr;
    };

    class ThreadPool
    {
    public:
        // The constructor just launches some amount of workers
        ThreadPool(size_t threads) : m_Stop(false)
        {
            for (size_t i = 0; i < threads; ++i)
                m_Workers.emplace_back([this] {
                    for (;;)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(this->m_QueueMutex);
                            this->m_Condition.wait(lock, [this] { return this->m_Stop || !this->m_Tasks.empty(); });
                            if (this->m_Stop && this->m_Tasks.empty())
                                return;
                            task = std::move(this->m_Tasks.front());
                            this->m_Tasks.pop();
                        }

                        task();
                    }
                });
        }

        // Add new work item to the pool
        template<class F, class... Args>
        std::future<typename std::result_of<F(Args...)>::type> Enqueue(F&& f, Args&&... args)
        {
            using return_type = typename std::result_of<F(Args...)>::type;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));

            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);

                // don't allow enqueueing after stopping the pool
                if (m_Stop)
                    throw std::runtime_error("enqueue on stopped ThreadPool");

                m_Tasks.emplace([task]() { (*task)(); });
            }
            m_Condition.notify_one();
            return res;
        }

        // The destructor joins all threads
        ~ThreadPool()
        {
            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);
                m_Stop = true;
            }
            m_Condition.notify_all();
            for (std::thread& worker : m_Workers)
                worker.join();
        }

    private:
        // need to keep track of threads so we can join them
        std::vector<std::thread> m_Workers;
        // the task queue
        std::queue<std::function<void()>> m_Tasks;

        // synchronization
        std::mutex              m_QueueMutex;
        std::condition_variable m_Condition;
        bool                    m_Stop;
    };

    struct PixelColor
    {
        PixelColor() : R(0), G(0), B(0), A(0) {}
        PixelColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : R(r), G(g), B(b), A(a) {}
        PixelColor(const Color& color)
        {
            R = static_cast<uint8_t>(255.999 * color.x());
            G = static_cast<uint8_t>(255.999 * color.y());
            B = static_cast<uint8_t>(255.999 * color.z());
            A = 255;
        }

        uint8_t R;
        uint8_t G;
        uint8_t B;
        uint8_t A;
    };

    struct FrameBuffer
    {
        FrameBuffer(std::vector<PixelColor> data, int width, int height) : Data(data), Width(width), Height(height) {}

        size_t GetSize() { return Width * Height; }

        std::vector<PixelColor> Data;
        uint32_t                Width;
        uint32_t                Height;
    };

    struct RenderCameraConfiguration
    {
        Point3  LookFrom;
        Point3  LookAt;
        Vector3 ViewUp;
        double  DistanceToFocus;
        double  Aperture;
        double  FOV;
    };

    struct RenderConfiguration
    {
        uint32_t           RenderTargetWidth;
        uint32_t           RenderTargetHeight;
        RenderCameraConfiguration CameraConfig;
        Color              BackgroundColor = Black;
        uint32_t           RenderTileSize  = 16;
        uint32_t           SamplesPerPixel = 10;
        uint32_t           MaxDepth        = 4;
        uint32_t           SceneID         = 0;
    };

    static ThreadPool Pool(std::thread::hardware_concurrency() - 1);
    static std::mutex TileMutex;

    class RaytracerCore
    {
    public:
        RaytracerCore() {}
        const std::shared_ptr<FrameBuffer>& GetFrameBuffer() const { return m_FrameBuffer; }

        void Render(RenderConfiguration config)
        {
            auto frameBufferWidth  = config.RenderTargetWidth;
            auto frameBufferHeight = config.RenderTargetHeight;
            auto tileSize          = config.RenderTileSize;
            auto samplesPerPixel   = config.SamplesPerPixel;
            auto maxDepth          = config.MaxDepth;

            m_FrameBuffer = std::make_shared<FrameBuffer>(
                std::vector<PixelColor>(frameBufferWidth * frameBufferHeight), frameBufferWidth, frameBufferHeight);

            // Init World
            switch (config.SceneID)
            {
                case 0: {
                    InitRandomScene();
                    break;
                }

                case 1: {
                    InitSimpleCornellBox();
                }

                default:
                    break;
            }

            // Camera
            m_Camera = {config.CameraConfig.LookFrom,
                        config.CameraConfig.LookAt,
                        config.CameraConfig.ViewUp,
                        config.CameraConfig.FOV,
                        (double)frameBufferWidth / frameBufferWidth,
                        config.CameraConfig.Aperture,
                        config.CameraConfig.DistanceToFocus};

            // Tile-based multi-threaded rendering
            int xTiles = (frameBufferWidth + tileSize - 1) / tileSize;
            int yTiles = (frameBufferHeight + tileSize - 1) / tileSize;

            int totalTileCount    = xTiles * yTiles;
            int finishedTileCount = 0;

            auto renderTile = [&](int      xTileIndex,
                                  int      yTileIndex,
                                  uint32_t tileSize,
                                  uint32_t frameBufferWidth,
                                  uint32_t frameBufferHeight,
                                  uint32_t samplesPerPixel,
                                  uint32_t maxDepth,
                                  Color    backgroundColor,
                                  int      finishedTileCount,
                                  int      totalTileCount) {
                int xStart = xTileIndex * tileSize;
                int yStart = yTileIndex * tileSize;

                for (int j = yStart; j < yStart + tileSize; ++j)
                {
                    for (int i = xStart; i < xStart + tileSize; ++i)
                    {
                        // Bounds check
                        if (i >= frameBufferWidth || j >= frameBufferHeight)
                            continue;

                        Color color = Black;
                        for (int s = 0; s < samplesPerPixel; ++s)
                        {
                            double u = (i + GetRandomDouble()) / (frameBufferWidth - 1);
                            double v = (j + GetRandomDouble()) / (frameBufferHeight - 1);
                            Ray    r = m_Camera.GetRay(u, v);
                            color += GetRayColor(r, backgroundColor, m_World, maxDepth);
                        }

                        auto r = color.x();
                        auto g = color.y();
                        auto b = color.z();

                        // Divide the color by the number of samples and gamma-correct for gamma=2.0.
                        double scale = 1.0 / samplesPerPixel;
                        r            = sqrt(scale * r);
                        g            = sqrt(scale * g);
                        b            = sqrt(scale * b);

                        // Write Color
                        int        index           = j * frameBufferWidth + i;
                        PixelColor pixelColor      = {static_cast<uint8_t>(255.999 * r),
                                                      static_cast<uint8_t>(255.999 * g),
                                                      static_cast<uint8_t>(255.999 * b)};
                        m_FrameBuffer->Data[index] = pixelColor;
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(TileMutex);
                    finishedTileCount++;
                    if (finishedTileCount == totalTileCount)
                    {
                        std::cout << "Render Async Finished!" << std::endl;
                    }
                }
            };

            for (int i = 0; i < xTiles; ++i)
            {
                for (int j = 0; j < yTiles; ++j)
                {
                    Pool.Enqueue(renderTile,
                                 i,
                                 j,
                                 tileSize,
                                 frameBufferWidth,
                                 frameBufferHeight,
                                 samplesPerPixel,
                                 maxDepth,
                                 config.BackgroundColor,
                                 finishedTileCount,
                                 totalTileCount);
                }
            }
        }

    private:
        Color GetRayColor(const Ray& r, const Color& backgroundColor, const Hittable& world, int depth)
        {
            HitRecord rec;

            // If we've exceeded the ray bounce limit, no more light is gathered.
            if (depth <= 0)
            {
                return Black;
            }

            if (!world.Hit(r, 0.001, Infinity, rec))
            {
                return backgroundColor;
            }

            Ray   scattered;
            Color attenuation;
            Color emitted = rec.MaterialPtr->Emitted(rec.U, rec.V, rec.Point);

            if (!rec.MaterialPtr->Scatter(r, rec, attenuation, scattered))
            {
                return emitted;
            }

            return emitted + attenuation * GetRayColor(scattered, backgroundColor, world, depth - 1);
        }

        void InitRandomScene()
        {
            auto checker = std::make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
            m_World.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(checker)));

            for (int a = -11; a < 11; a++)
            {
                for (int b = -11; b < 11; b++)
                {
                    double materialChosen = GetRandomDouble();
                    Point3 center(a + 0.9 * GetRandomDouble(), 0.2, b + 0.9 * GetRandomDouble());

                    if ((center - Point3(4, 0.2, 0)).Length() > 0.9)
                    {
                        std::shared_ptr<Material> materialSphere;

                        if (materialChosen < 0.8)
                        {
                            // Diffuse
                            Color albedo   = Color::GetRandom() * Color::GetRandom();
                            materialSphere = std::make_shared<Lambertian>(albedo);
                        }
                        else if (materialChosen < 0.95)
                        {
                            // Metal
                            Color  albedo  = Color::GetRandom(0.5, 1);
                            double fuzz    = GetRandomDouble(0, 0.5);
                            materialSphere = std::make_shared<Metal>(albedo, fuzz);
                        }
                        else
                        {
                            // Glass
                            materialSphere = std::make_shared<Dielectric>(1.5);
                        }

                        m_World.Add(std::make_shared<Sphere>(center, 0.2, materialSphere));
                    }
                }
            }

            auto material1 = std::make_shared<Dielectric>(1.5);
            m_World.Add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

            auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
            m_World.Add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

            auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
            m_World.Add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));
        }

        void InitSimpleCornellBox()
        {
            auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
            auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
            auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
            auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

            m_World.Add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
            m_World.Add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
            m_World.Add(std::make_shared<XZRect>(213, 343, 227, 332, 554, light));
            m_World.Add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
            m_World.Add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
            m_World.Add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));

            std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
            box1                           = std::make_shared<RotateY>(box1, 15);
            box1                           = std::make_shared<Translate>(box1, Vector3(265, 0, 295));
            m_World.Add(box1);

            std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
            box2                           = std::make_shared<RotateY>(box2, -18);
            box2                           = std::make_shared<Translate>(box2, Vector3(130, 0, 65));
            m_World.Add(box2);
        }

    private:
        std::shared_ptr<FrameBuffer> m_FrameBuffer;
        HittableList                 m_World;
        Camera                       m_Camera;
    };
} // namespace VRaytracer