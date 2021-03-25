//==============================================================================
// Date        : 3/24/21 (SG)
// Copyright   : Copyright 2021. Regents of the University of Colorado.
//               All rights reserved.
//==============================================================================

using namespace std;

// TODO:
/*template <typename T>
bool essentiallyEqual(float a, float b) {
    // Use machine precision for floats
       return essentiallyEqual(a, b, FLT_EPSILON);
}*/
     
template <typename T>
T MathUtil::radius(const array<T, 3>& p, const array<T, 3>& c)
{
    // Radius = sqrt((px-cx)^2 + (py-cy)^2 + (pz-cz)^2)
    return sqrt((p[0]-c[0])*(p[0]-c[0]) + 
                (p[1]-c[1])*(p[1]-c[1]) + 
                (p[2]-c[2])*(p[2]-c[2]));
}

template <typename T>
T MathUtil::magnitude(const array<T, 3>& p)
{
    // Magnitude = sqrt(x^2 + y^2 + z^2)
    return sqrt((p[0]*p[0]) + (p[1]*p[1]) + (p[2]*p[2]));
}

template <typename T>
array<T, 3> MathUtil::normalize(const array<T, 3>& p)
{
    // TODO: Implement templated essentially equal check
    // Compute magnitude
    T mag = magnitude(p);
    if (mag == T(0))
    {
        cerr << "Error: Cannot normalize coordinate - length is < 0."
             << endl;
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
    // Compute lat, lon
    array<T, 2> latlon;
    latlon[0] = toDegrees(asin(p[2] / radius));
    latlon[1] = toDegrees(atan2(p[1], p[0]));
        
    return latlon;
}

template <typename T>
void MathUtil::xyzToLatLonRadius(GNode<T>& node)
{
    // Compute radius of coordinate
    T r = radius(node.pos(), array<T, 3> {0, 0, 0});

    // Normalize coordinate
    array<T, 3> n = normalize(node.pos());
    //cout << "n is: " << n[0] << ", " << n[1] << ", " << n[2] << endl;

    // Compute lat,lon of coordinate
    array<T, 2> ll = toLatLon(n, r);
   
    // Update node
    node.radius(r);
    node.latlon(ll);
    node.latlon()[1] = 5;
}