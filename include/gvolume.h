//==============================================================================
// Date         : 3/31/21 (SG)
// Description  : Stores a volume made up of GeoFLOW layers.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

#ifndef GVOLUME_H
#define GVOLUME_H

#include <iostream>
#include <vector>

#include "glayer.h"
#include "logger.h"

using namespace std;

template <class T>
class GVolume
{
public:
    GVolume() {}
    ~GVolume() {}

    // Access
    vector<GLayer<T>> layers() const { return _layers; }
    void layers(const vector<GLayer<T>>& layers) { _layers = layers; }

    // Print
    void printVolume()
    {
        cout << "Volume" << endl;
        for (auto l : _layers)
        {
            l.printLayer();
        }
    }

private:
    vector<GLayer<T>> _layers;
    GUINT _nLayers;
    vector<GNode<T>> _nodes;
};

#endif