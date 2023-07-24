// This is a single header file. It contains all of the functionalities of a software raytracer(path-tracing).

#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <limits>
#include <cstdlib>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

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

    using Point3 = Vector3; // 3D Point

    using Color  = Vector3; // RGB Color
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
        Ray(const Point3& origin, const Vector3& direction) : m_Origin(origin), m_Direction(direction) {}

        Point3  Origin() const { return m_Origin; }
        Vector3 Direction() const { return m_Direction; }

        Point3 At(double t) const { return m_Origin + t * m_Direction; }

    private:
        Point3  m_Origin;
        Vector3 m_Direction;
    };

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
               double  focusDistance)
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
        }

        Ray GetRay(double s, double t) const
        {
            Vector3 rd     = m_LensRadius * GetRandomInUnitDisk();
            Vector3 offset = m_U * rd.x() + m_V * rd.y();
            return Ray(m_Origin + offset, m_LowerLeftCorner + s * m_Horizontal + t * m_Vertical - m_Origin - offset);
        }

    private:
        Point3  m_Origin;
        Point3  m_LowerLeftCorner;
        Vector3 m_Horizontal;
        Vector3 m_Vertical;
        Vector3 m_U, m_V, m_W;
        double  m_LensRadius;
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
    };

    class HittableList : public Hittable
    {
    public:
        HittableList() {}
        HittableList(std::shared_ptr<Hittable> object) {}

        void Clear() { m_Objects.clear(); }
        void Add(std::shared_ptr<Hittable> object) { m_Objects.push_back(object); }

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

    private:
        std::vector<std::shared_ptr<Hittable>> m_Objects;
    };

    class Material
    {
    public:
        virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const = 0;
    };

    class Lambertian : public Material
    {
    public:
        Lambertian(const Color& albedo) : m_Albedo(albedo) {}

        virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
        {
            auto scatterDirection = rec.Normal + GetRandomUnitVector();

            // Catch degenerate scatter direction
            if (scatterDirection.IsNearZero())
                scatterDirection = rec.Normal;

            scattered   = Ray(rec.Point, scatterDirection);
            attenuation = m_Albedo;
            return true;
        }

    private:
        Color m_Albedo;
    };

    class Metal : public Material
    {
    public:
        Metal(const Color& albedo, double fuzz) : m_Albedo(albedo), m_Fuzz(fuzz < 1 ? fuzz : 1) {}

        virtual bool Scatter(const Ray& rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override
        {
            Vector3 reflected = Reflect(Normalize(rIn.Direction()), rec.Normal);
            scattered         = Ray(rec.Point, reflected + m_Fuzz * GetRandomInUnitSphere());
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

            scattered = Ray(rec.Point, direction);
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

    class Sphere : public Hittable
    {
    public:
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
            rec.MaterialPtr = m_MaterialPtr;

            return true;
        }

    private:
        Point3                    m_Center;
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

    struct RenderConfig
    {
        uint32_t RenderTargetWidth;
        uint32_t RenderTargetHeight;
        uint32_t RenderTileSize  = 16;
        uint32_t SamplesPerPixel = 10;
        uint32_t MaxDepth = 4;
    };

    static ThreadPool Pool(std::thread::hardware_concurrency() - 1);
    static std::mutex TileMutex;

	class RaytracerCore
	{
    public:
        const std::shared_ptr<FrameBuffer>& GetFrameBuffer() const { return m_FrameBuffer; }

        void Render(RenderConfig config)
        {
            auto        frameBufferWidth  = config.RenderTargetWidth;
            auto        frameBufferHeight = config.RenderTargetHeight;
            auto        tileSize          = config.RenderTileSize;
            auto        samplesPerPixel   = config.SamplesPerPixel;
            auto        maxDepth          = config.MaxDepth;

            m_FrameBuffer = std::make_shared<FrameBuffer>(
                std::vector<PixelColor>(frameBufferWidth * frameBufferHeight), frameBufferWidth, frameBufferHeight);

            // Init World
            InitRandomScene();

            // Camera
            Point3  lookFrom(13, 2, 3);
            Point3  lookAt(0, 0, 0);
            Vector3 viewUp(0, 1, 0);
            double  distanceToFocus = 10.0;
            double  aperture        = 0.1;
            m_Camera                = {
                lookFrom, lookAt, viewUp, 20.0, (double)frameBufferWidth / frameBufferWidth, aperture, distanceToFocus};

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
                            color += GetRayColor(r, m_World, maxDepth);
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
                        m_FrameBuffer->Data[index]  = pixelColor;
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
                                 finishedTileCount,
                                 totalTileCount);
                }
            }
        }

    private:
        Color GetRayColor(const Ray& r, const HittableList& world, int depth)
        {
            HitRecord rec;

            // If we've exceeded the ray bounce limit, no more light is gathered.
            if (depth <= 0)
                return Black;

            if (world.Hit(r, 0.001, Infinity, rec))
            {
                Ray   scattered;
                Color attenuation;
                if (rec.MaterialPtr->Scatter(r, rec, attenuation, scattered))
                    return attenuation * GetRayColor(scattered, world, depth - 1);
                return Black;
            }

            Vector3 unitDirection = Normalize(r.Direction());
            double  t             = 0.5 * (unitDirection.y() + 1.0);
            return (1.0 - t) * White + t * LightBlue;
        }

        void  InitRandomScene()
        {
            auto materialGround = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
            m_World.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, materialGround));

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

    private:
        std::shared_ptr<FrameBuffer> m_FrameBuffer;
        HittableList                 m_World;
        Camera                       m_Camera;
	};
}