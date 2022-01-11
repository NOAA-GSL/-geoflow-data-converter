//==============================================================================
// Date         : 3/24/21 (SG)
// Description  : Stores a face (i.e., a polygon) enclosed by a set of GeoFLOW
//                nodes.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

#ifndef GFACE_H
#define GFACE_H

#include <iostream>
#include <vector>

#include <gtypes.h>

using namespace std;

class GFace
{
public:
    /*!
     * Constructor for initializing a GeoFLOW face.
     *
     * @param indices an ordered list of indices that make up a face; the 
     *                indices index into a list of nodes
     */ 
    GFace(const vector<GSIZET>& indices)
    {
        _indices = indices;
    }

    ~GFace() {}

    // Access
    vector<GSIZET> indices() const { return _indices; }
    void indices(const vector<GSIZET> indices)
    {
        _indices.clear();
        _indices.shrink_to_fit();
        _indices = indices;
    }

    /*!
     * Print the node indices of the face.
     */
    void printFace()
    {
        cout << "Face: (";
        for (auto i : _indices)
        {
            cout << i << ",";
        }
        cout << ")" << endl;
    }

private:
    vector<GSIZET> _indices;   // indices of nodes from a global node list
};

#endif