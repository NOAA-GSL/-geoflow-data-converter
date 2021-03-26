//==============================================================================
// Date        : 3/24/21 (SG)
// Copyright   : Copyright 2021. Regents of the University of Colorado.
//               All rights reserved.
//==============================================================================

using namespace std;
     
template <typename T>
T MathUtil::radius(const array<T, 3>& p, const array<T, 3>& c)
{
    // Radius = sqrt((px-cx)^2 + (py-cy)^2 + (pz-cz)^2)
    return sqrt((p[0] - c[0]) * (p[0] - c[0]) + 
                (p[1] - c[1]) * (p[1] - c[1]) + 
                (p[2] - c[2]) * (p[2] - c[2]));
}

template <typename T>
T MathUtil::magnitude(const array<T, 3>& p)
{
    // Magnitude = sqrt(x^2 + y^2 + z^2)
    return sqrt((p[0] * p[0]) + (p[1] * p[1]) + (p[2] * p[2]));
}

template <typename T>
array<T, 3> MathUtil::normalize(const array<T, 3>& p)
{
    // Compute magnitude
    T mag = magnitude(p);
    if (mag == T(0))
    {
        string msg = "Cannot normalize coordinate because magnitude is 0.";
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // Normalize
    array<T, 3> n;
    n[0] = p[0] / mag;
    n[1] = p[1] / mag;
    n[2] = p[2] / mag;

    return n;
}

template <typename T>
T MathUtil::toDegrees(const T& v)
{
    return v * (180.0 / M_PI);
}

template <typename T>
array<T, 2> MathUtil::toLatLon(const array<T, 3>& p, T radius)
{
    // Compute lat,lon from 3D coordinate
    array<T, 2> latlon;
    latlon[0] = asin(p[2] / radius);
    latlon[1] = atan2(p[1], p[0]);
        
    return latlon;
}

template <typename T>
void MathUtil::xyzToLatLonRadius(GNode<T>& node)
{
    // Compute radius of coordinate
    T r = radius(node.pos(), array<T, 3> {0, 0, 0});

    // Normalize coordinate
    array<T, 3> n = normalize(node.pos());

    // Compute lat,lon of coordinate and convert to degrees.
    array<T, 2> ll = toLatLon(n, T(1.0));
    ll[0] = toDegrees(ll[0]);
    ll[1] = toDegrees(ll[1]);
   
    // Update node
    node.radius(r);
    node.latlon(ll);
}