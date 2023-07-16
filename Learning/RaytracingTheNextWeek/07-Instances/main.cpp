#include "Camera.h"
#include "Color.h"
#include "Common.h"
#include "HittableList.h"
#include "Material.h"
#include "Sphere.h"
#include "MovingSphere.h"
#include "AARect.h"
#include "Box.h"

#include <iostream>

// Image
const double AspectRatio     = 1;
const int    ImageWidth      = 600;
const int    ImageHeight     = static_cast<int>(ImageWidth / AspectRatio);
const int    SamplesPerPixel = 200;
const int    MaxDepth        = 8;

Color frameBuffer[ImageWidth][ImageWidth];

HittableList GetEarth()
{
    HittableList objects;

	auto earthTexture = std::make_shared<ImageTexture>("earthmap.jpg");
    auto earthSurface = std::make_shared<Lambertian>(earthTexture);
    auto globe        = std::make_shared<Sphere>(Point3(0, 0, 0), 2, earthSurface);
    objects.Add(globe);

    return objects;
}

HittableList GenerateTwoPerlinSpheres()
{
	HittableList objects;

    auto pertext = std::make_shared<NoiseTexture>(4);
    objects.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    objects.Add(std::make_shared<Sphere>(Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    return objects;
}

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

HittableList GetSimpleLight()
{ 
    HittableList objects;

    auto pertext = std::make_shared<NoiseTexture>(4);
    objects.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    objects.Add(std::make_shared<Sphere>(Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    auto diffLight = std::make_shared<DiffuseLight>(Color(4, 4, 4));
    objects.Add(std::make_shared<XYRect>(3, 5, 1, 3, -2, diffLight));

    return objects;
}

HittableList GetCornellBox()
{
    HittableList objects;

    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

    objects.Add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
    objects.Add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
    objects.Add(std::make_shared<XZRect>(213, 343, 227, 332, 554, light));
    objects.Add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
    objects.Add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
    objects.Add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));

    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1                           = std::make_shared<RotateY>(box1, 15);
    box1                           = std::make_shared<Translate>(box1, Vector3(265, 0, 295));
    objects.Add(box1);

    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2                           = std::make_shared<RotateY>(box2, -18);
    box2                           = std::make_shared<Translate>(box2, Vector3(130, 0, 65));
    objects.Add(box2);

    return objects;
}


int main()
{
    // World
    auto  world           = GetCornellBox();
    Color backgroundColor = Black;

    // Camera
    Point3  lookFrom(278, 278, -800);
    Point3  lookAt(278, 278, 0);
    Vector3 viewUp(0, 1, 0);
    double  distanceToFocus = 10.0;
    double  aperture        = 0.1;
    Camera  cam(lookFrom, lookAt, viewUp, 40.0, AspectRatio, aperture, distanceToFocus);

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
                pixelColor += GetRayColor(r, backgroundColor, world, MaxDepth);
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