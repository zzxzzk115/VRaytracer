#pragma once

#include <cmath>
#include <limits>
#include <memory>

// Constants

const double Infinity = std::numeric_limits<double>::infinity();
const double Pi       = 3.1415926535897932385;

// Utility Functions

inline double Degrees2Radians(double degrees) { return degrees * Pi / 180.0; }

// Common Headers

#include "Ray.h"
#include "Vector3.h"