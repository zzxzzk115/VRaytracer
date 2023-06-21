#pragma once

#include "Common.h"

class Perlin
{
public:
    Perlin() {
        m_RanVec = new Vector3[PointCount];
        for (int i = 0; i < PointCount; ++i)
        {
            m_RanVec[i] = Normalize(Vector3::GetRandom(-1, 1));
        }

        m_PermX = GeneratePerlinPerm();
        m_PermY = GeneratePerlinPerm();
        m_PermZ = GeneratePerlinPerm();
    }

    ~Perlin()
    {
        delete[] m_RanVec;
        delete[] m_PermX;
        delete[] m_PermY;
        delete[] m_PermZ;
    }

    double Turb(const Point3& point, int depth = 7) const
    {
        double accum     = 0.0;
        Point3 tempPoint = point;
        double weight    = 1.0;

        for (int i = 0; i < depth; ++i)
        {
            accum += weight * Noise(tempPoint);
            weight *= 0.5;
            tempPoint *= 2;
        }

        return fabs(accum);
    }

    double Noise(const Point3& point) const
    {
        double u = point.x() - floor(point.x());
        double v = point.y() - floor(point.y());
        double w = point.z() - floor(point.z());

        int i = static_cast<int>(floor(point.x()));
        int j = static_cast<int>(floor(point.y()));
        int k = static_cast<int>(floor(point.z()));

        Vector3 c[2][2][2];

        for (int di = 0; di < 2; ++di)
        {
            for (int dj = 0; dj < 2; ++dj)
            {
                for (int dk = 0; dk < 2; ++dk)
                {
                    c[di][dj][dk] =
                        m_RanVec[m_PermX[(i + di) & 255] ^ m_PermY[(j + dj) & 255] ^ m_PermZ[(k + dk) & 255]];
                }
            }
        }

        return PerlinInterpolate(c, u, v, w);
    }

private:
    static int* GeneratePerlinPerm()
    {
        auto p = new int[PointCount];

        // Init
        for (int i = 0; i < PointCount; ++i)
        {
            p[i] = i;
        }

        Permute(p, PointCount);

        return p;
    }

    static void Permute(int* p, int n)
    {
        for (int i = n - 1; i > 0; --i)
        {
            int target = GetRandomInt(0, i);
            int tmp    = p[i];
            p[i]       = p[target];
            p[target]  = tmp;
        }
    }

    static double PerlinInterpolate(Vector3 c[2][2][2], double u, double v, double w)
    {
        // Hermitian Smoothing
        u            = u * u * (3 - 2 * u);
        v            = v * v * (3 - 2 * v);
        w            = w * w * (3 - 2 * w);
        double accum = 0.0;

        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    Vector3 weightVector(u - i, v - j, w - k);
                    accum += (i * u + (1 - i) * (1 - u)) * (j * v + (1 - j) * (1 - v)) * (k * w + (1 - k) * (1 - w)) *
                             DotProduct(c[i][j][k], weightVector);
                }
            }
        }

        return accum;
    }

private:
	static const int PointCount = 256;

    Vector3* m_RanVec;
    int*     m_PermX;
    int*     m_PermY;
    int*     m_PermZ;
};