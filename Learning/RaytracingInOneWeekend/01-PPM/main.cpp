#include <iostream>

const int ImageWidth  = 256;
const int ImageHeight = 256;

// https://en.wikipedia.org/wiki/Netpbm#PPM_example
int main()
{
    std::cout << "P3\n" << ImageWidth << ' ' << ImageHeight << "\n255\n";

    for (int j = ImageHeight - 1; j >= 0; --j)
    {
        for (int i = 0; i < ImageWidth; ++i)
        {
        	auto r = static_cast<double>(i) / (ImageWidth - 1);
        	auto g = static_cast<double>(j) / (ImageHeight - 1);
        	auto b = 0.25;

	        const int ir = static_cast<int>(255.999 * r);
	        const int ig = static_cast<int>(255.999 * g);
	        const int ib = static_cast<int>(255.999 * b);

            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

	return 0;
}