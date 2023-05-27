#include "Vector3.h"
#include "Color.h"

#include <iostream>

const int ImageWidth  = 256;
const int ImageHeight = 256;

int main()
{
    std::cout << "P3\n" << ImageWidth << ' ' << ImageHeight << "\n255\n";

    for (int j = ImageHeight - 1; j >= 0; --j)
    {
        for (int i = 0; i < ImageWidth; ++i)
        {
            Color pixelColor(double(i) / (ImageWidth - 1), double(j) / (ImageHeight - 1), 0.25);
            WriteColor(std::cout, pixelColor);
        }
    }

    return 0;
}