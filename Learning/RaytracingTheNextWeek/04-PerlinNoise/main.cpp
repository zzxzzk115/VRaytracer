#include "Camera.h"
#include "Color.h"
#include "Common.h"
#include "HittableList.h"
#include "Material.h"
#include "Sphere.h"
#include "MovingSphere.h"

#include <iostream>

// Image
const double AspectRatio     = 16.0 / 9.0;
const int    ImageWidth      = 400;
const int    ImageHeight     = static_cast<int>(ImageWidth / AspectRatio);
const int    SamplesPerPixel = 20;
const int    MaxDepth        = 8;

Color frameBuffer[ImageWidth][ImageWidth];

HittableList GenerateTwoPerlinSpheres()
{
	HittableList objects;

    auto pertext = std::make_shared<NoiseTexture>(4);
    objects.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    objects.Add(std::make_shared<Sphere>(Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    return objects;
}

Color GetRayColor(const Ray& r, const Hittable& world, int depth)
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

HittableList GetRandomScene()
{
    HittableList world;

    auto checker = std::make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    world.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(checker)));

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
                    Point3 center2 = center + Vector3(0, GetRandomDouble(0, 0.5), 0);
                    world.Add(std::make_shared<MovingSphere>(center, center2, 0.0, 1.0, 0.2, materialSphere));
                }
                else if (materialChosen < 0.95)
                {
                    // Metal
                    Color  albedo  = Color::GetRandom(0.5, 1);
                    double fuzz    = GetRandomDouble(0, 0.5);
                    materialSphere = std::make_shared<Metal>(albedo, fuzz);
                    world.Add(std::make_shared<Sphere>(center, 0.2, materialSphere));
                }
                else
                {
                    // Glass
                    materialSphere = std::make_shared<Dielectric>(1.5);
                    world.Add(std::make_shared<Sphere>(center, 0.2, materialSphere));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.Add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.Add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.Add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
}

int main()
{
    // World
    auto world = GenerateTwoPerlinSpheres();

    // Camera
    Point3  lookFrom(13, 2, 3);
    Point3  lookAt(0, 0, 0);
    Vector3 viewUp(0, 1, 0);
    double  distanceToFocus = 10.0;
    double  aperture        = 0.1;
    Camera  cam(lookFrom, lookAt, viewUp, 20.0, AspectRatio, aperture, distanceToFocus, 0.0, 1.0);

    // Render
    std::cout << "P3\n" << ImageWidth << ' ' << ImageHeight << "\n255\n";

    for (int j = ImageHeight - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

        // Add OpenMP support
#pragma omp parallel for
        for (int i = 0; i < ImageWidth; ++i)
        {
            Color pixelColor(0, 0, 0);
            for (int s = 0; s < SamplesPerPixel; ++s)
            {
                double u = (i + GetRandomDouble()) / (ImageWidth - 1);
                double v = (j + GetRandomDouble()) / (ImageHeight - 1);
                Ray    r = cam.GetRay(u, v);
                pixelColor += GetRayColor(r, world, MaxDepth);
            }
            frameBuffer[i][j] = pixelColor;
        }
    }

    // Post Write Color
    for (int j = ImageHeight - 1; j >= 0; --j)
    {
        for (int i = 0; i < ImageWidth; ++i)
        {
            WriteColor(std::cout, frameBuffer[i][j], SamplesPerPixel);
        }
    }

    std::cerr << "\nDone.\n";
    return 0;
}