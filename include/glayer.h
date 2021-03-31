//==============================================================================
// Date         : 3/31/21 (SG)
// Description  : Stores a layer made up of GeoFLOW faces.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

#ifndef GLAYER_H
#define GLAYER_H

#include <iostream>
#include <vector>

#include "gface.h"
#include "logger.h"

using namespace std;

template <class T>
class GLayer
{
public:
    GLayer() {}
    ~GLayer() {}

    // Access
    vector<GFace<T>> faces() const { return _faces; }
    void faces(const vector<GFace<T>>& faces) { _faces = faces; }

    // Print
    void printLayer()
    {
        cout << "Layer faces" << endl;
        for (auto f : _faces)
        {
            f.printFaces();
        }
    }

private:
    vector<GFace<T>> _faces;
    GSIZET _nFaces;
};

#endif