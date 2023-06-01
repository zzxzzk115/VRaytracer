#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

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

// Common Headers

#include "Ray.h"
#include "Vector3.h"