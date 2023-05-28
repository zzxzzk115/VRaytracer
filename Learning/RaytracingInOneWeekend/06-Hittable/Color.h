#pragma once

#include "Vector3.h"

using Color = Vector3; // RGB Color

#include <iostream>

inline void WriteColor(std::ostream& out, Color pixelColor)
{
	// Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixelColor.x()) << ' ' << static_cast<int>(255.999 * pixelColor.y()) << ' '
        << static_cast<int>(255.999 * pixelColor.z()) << '\n';
}

const Color White     = Color(1, 1, 1);
const Color LightBlue = Color(0.5, 0.7, 1.0);