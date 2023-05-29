#pragma once

#include "Vector3.h"

using Color = Vector3; // RGB Color

#include <iostream>

inline void WriteColor(std::ostream& out, Color pixelColor, int samplesPerPixel)
{
    double r = pixelColor.x();
    double g = pixelColor.y();
    double b = pixelColor.z();

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    double scale = 1.0 / samplesPerPixel;
    r            = sqrt(scale * r);
    g            = sqrt(scale * g);
    b            = sqrt(scale * b);

	// Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * Clamp(r, 0.0, 0.999)) << ' ' << static_cast<int>(256 * Clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * Clamp(b, 0.0, 0.999)) << '\n';
}

const Color Black     = Color(0, 0, 0);
const Color White     = Color(1, 1, 1);
const Color LightBlue = Color(0.5, 0.7, 1.0);