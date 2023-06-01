#pragma once

#include "Common.h"

class Camera
{
public:
    Camera(Point3 lookFrom, Point3 lookAt, Vector3 viewUp, double verticalFOV, double aspectRatio)
    {
        double       theta          = Degrees2Radians(verticalFOV);
        double       h              = std::tan(theta / 2);
	    const double viewportHeight = 2.0 * h;
	    const double viewportWidth  = aspectRatio * viewportHeight;

        Vector3 w = Normalize(lookFrom - lookAt);
        Vector3 u = Normalize(CrossProduct(viewUp, w));
        Vector3 v = CrossProduct(w, u);

        m_Origin = lookFrom;
        m_Horizontal      = viewportWidth * u;
        m_Vertical        = viewportHeight * v;
        m_LowerLeftCorner = m_Origin - m_Horizontal / 2 - m_Vertical / 2 - w;
    }

    Ray GetRay(double s, double t) const
    {
        return Ray(m_Origin, m_LowerLeftCorner + s * m_Horizontal + t * m_Vertical - m_Origin);
    }

private:
    Point3  m_Origin;
    Point3  m_LowerLeftCorner;
    Vector3 m_Horizontal;
    Vector3 m_Vertical;
};