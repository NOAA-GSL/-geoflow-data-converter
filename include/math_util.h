//==============================================================================
// Date        : 3/24/21 (SG)
// Description : Various math helper functions.
// Copyright   : Copyright 2021. Regents of the University of Colorado.
//               All rights reserved.
//==============================================================================

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <iostream>
#include <vector>
#include <array>

using namespace std;

class MathUtil
{
public:
    MathUtil() {}
    ~MathUtil() {}

    // TODO:
    /*!
     * Floating point comparison: From The Art of Computer Programming by Knuth
     * 
     * @param a first number in comparison
     * @param b second number in comparison
     * @return true if essentially equal, false if not
     */
    /*template <typename T>
    bool essentiallyEqual(float a, float b);*/
     
     /*!
     * Compute radius of a 3D Cartesian coordinate on a sphere.
     * Assumes center of sphere is at (0, 0, 0).
     * 
     * @param p 3D coordinate
     * @param c location of sphere center
     * @return the radius of the coordinate
     */
    template <typename T>
    static T radius(const array<T, 3>& p, const array<T, 3>& c);

    /*!
     * Compute magnitude of a 3D Cartesian coordinate on a sphere.
     * 
     * @param p 3D coordinate
     * @return the magnitude of the coordinate
     */
    template <typename T>
    static T magnitude(const array<T, 3>& p);

     /*!
     * Normalize a 3D coordinate.
     * 
     * @param p 3D coordinate to normalize
     * @return the normalized 3D coordinate
     */
    template <typename T>
    static array<T, 3> normalize(const array<T, 3>& p);

     /*!
     * Convert radians to degrees.
     * 
     * @param v value in radians
     * @return the value in degrees
     */ 
    template <typename T>
    static T toDegrees(const T& v);

     /*!
     * Convert a 3D cartesian coordinate to a spherical lat,lon coordinate.
     * 
     * @param p a normalized 3D coordinate
     * @param radius radius of 3D coordinate
     * @return the lat,lon coordinate in radians
     */ 
    template <typename T>
    static array<T, 2> toLatLon(const array<T, 3>& p, T radius);

    /*!
     * Compute a lat,lon,radius from a 3D Cartesian coordinate. The center 
     * point of the sphere is assumed to be (0, 0, 0)
     * 
     * @param pos 3D Cartesian coordinate
     * @return the corresponding lat,lon,radius
     */
    template <typename T>
    static array<T, 3> xyzToLatLonRadius(array<T, 3> pos);
};

#include "../src/math_util.ipp"

#endif