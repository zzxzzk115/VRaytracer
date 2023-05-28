#pragma once

#include "Vector3.h"

using Color = Vector3; // RGB Color

#include <iostream>

inline void WriteColor(std::ostream& out, Color pixelColor, int samplesPerPixel)
{
    double r = pixelColor.x();
    double g = pixelColor.y();
    double b = pixelColor.z();

    // Divide the color by the number of samples
    double scale = 1.0 / samplesPerPixel;
    r *= scale;
    g *= scale;
    b *= scale;

	// Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * Clamp(r, 0.0, 0.999)) << ' ' << static_cast<int>(256 * Clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * Clamp(b, 0.0, 0.999)) << '\n';
}

const Color White     = Color(1, 1, 1);
const Color LightBlue = Color(0.5, 0.7, 1.0);