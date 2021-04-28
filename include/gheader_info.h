//==============================================================================
// Date        : 3/17/21 (SG)
// Description : Stores header and auxiliary info read from a binary GeoFLOW
//               data file.
// Copyright   : Copyright 2021. Regents of the University of Colorado.
//               All rights reserved.
//==============================================================================

#ifndef GHEADERINFO_H
#define GHEADERINFO_H

#include <vector>

#include "gtypes.h"

using namespace std;

struct GHeaderInfo
{
    // Data stored in GeoFLOW file header
    GUINT         version;       // version num
    GUINT         dim;           // dimension of elements (2 or 3)
    GSIZET        nElems;        // number of elements
    vector<GUINT> polyOrder;     // poly order of each dim, array of sz dim
    GUINT         gridType;      // grid type of GeoFLOW type GElemType
    GSIZET        timeCycle;     // output time loop index
    GDOUBLE       timeStamp;     // output time
    GUINT         hasMultVars;   // multiple fields in file?

    // Auxiliary data derived from GeoFLOW header
    GSIZET        nHeaderBytes;     // total byte size of GeoFLOW header
    GSIZET        nNodesPerVolume;  // num nodes (incl. sub nodes) in volume
    GUINT         nNodesPer2DElem;  // num nodes (incl. sub nodes) per 2D \
                                       GeoFLOW element (x and y ref dir)
    GSIZET        nNodesPer2DLayer; // num nodes (incl. sub nodes) per 2D layer
    GSIZET        nFacesPer2DLayer; // num faces (incl. sub faces) per 2D layer
    GSIZET        n2DLayers;        // num 2D layers in entire volume
    GSIZET        nElemLayers;      // num GeoFLOW element layers
    GSIZET        nElemPerElemLayer; // num GeoFLOW elements per GeoFLOW
                                     // element layer

    void printHeader()
    {
       cout << endl;
       cout << "-----------" << endl;
       cout << "Header Info" << endl;
       cout << "-----------" << endl;
       cout << "IO version: " << version << endl;
       cout << "Dimension: " << dim << endl;
       cout << "Num elements: " << nElems << endl;
       cout << "Poly orders: "; 
       for (const auto& p : polyOrder)
       {
           cout << p << " ";
       }
       cout << endl;
       cout << "Grid type: " << gridType << endl;
       cout << "Time cycle: " << timeCycle << endl;
       cout << "Time stamp: " << timeStamp << endl;
       cout << "Has mult fields?: " << hasMultVars << endl;

       // Print derived header info
       cout << endl;
       cout << "------------------------" << endl;
       cout << "Derived Info from Header" << endl;
       cout << "------------------------" << endl;
       cout << "Num header bytes: " << nHeaderBytes << endl;
       cout << "Num nodes per volume: " << nNodesPerVolume << endl;
       cout << "Num nodes per 2D GeoFLOW element: " << nNodesPer2DElem << endl;
       cout << "Num element layers: " << nElemLayers << endl;
       cout << "Num elements per layer: " << nElemPerElemLayer << endl;
       cout << "Num nodes per 2D layer: " << nNodesPer2DLayer << endl;
       cout << "Num 2D layers: " << n2DLayers << endl;
       cout << "Num faces per 2D layer: " << nFacesPer2DLayer << endl;
       cout << endl;
   }
};

#endif
