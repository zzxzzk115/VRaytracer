#pragma once

#include "Common.h"

class Camera
{
public:
    Camera(Point3 lookFrom, Point3 lookAt, Vector3 viewUp, double verticalFOV, double aspectRatio, double aperture, double focusDistance)
    {
        double       theta          = Degrees2Radians(verticalFOV);
        double       h              = std::tan(theta / 2);
	    const double viewportHeight = 2.0 * h;
	    const double viewportWidth  = aspectRatio * viewportHeight;

        m_W = Normalize(lookFrom - lookAt);
        m_U = Normalize(CrossProduct(viewUp, m_W));
        m_V = CrossProduct(m_W, m_U);

        m_Origin = lookFrom;
        m_Horizontal      = focusDistance * viewportWidth * m_U;
        m_Vertical        = focusDistance * viewportHeight * m_V;
        m_LowerLeftCorner = m_Origin - m_Horizontal / 2 - m_Vertical / 2 - focusDistance * m_W;
        m_LensRadius      = aperture / 2;
    }

    Ray GetRay(double s, double t) const
    {
        Vector3 rd     = m_LensRadius * GetRandomInUnitDisk();
        Vector3 offset = m_U * rd.x() + m_V * rd.y();
        return Ray(m_Origin + offset, m_LowerLeftCorner + s * m_Horizontal + t * m_Vertical - m_Origin - offset);
    }

private:
    Point3  m_Origin;
    Point3  m_LowerLeftCorner;
    Vector3 m_Horizontal;
    Vector3 m_Vertical;
    Vector3 m_U, m_V, m_W;
    double  m_LensRadius;
};