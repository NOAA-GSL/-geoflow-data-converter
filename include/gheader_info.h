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
    GUINT          version;       // version num
    GUINT          dim;           // dimension of elements (2 or 3)
    GSIZET         nElems;        // number of elements
    vector<GUINT>  polyOrder;     // poly order of each dim, array of sz dim
    GUINT          gridType;      // grid type of GF type GElemType
    GSIZET         timeCycle;     // output time loop index
    GDOUBLE        timeStamp;     // output time
    GUINT          hasMultVars;   // multiple fields in file?
    vector<GSIZET> elemIDs;       // layer IDs for each element

    // Auxiliary data derived from GeoFLOW header
    GSIZET        nHeaderBytes;      // total byte size of GF header
    GSIZET        nNodesPerVolume;   // num nodes in volume
    GSIZET        nNodesPerElem;     // num nodes per GF element (2D or 3D)
    GUINT         nNodesPer2DElem;   // num nodes per 2D (x,y ref dir only) 
                                     // GF element
    GSIZET        nNodesPer2DLayer;  // num nodes per 2D mesh layer
    GSIZET        nFacesPer2DLayer;  // num faces per 2D mesh layer
    GSIZET        n2DLayers;         // num 2D mesh layers in entire volume
    GSIZET        nElemLayers;       // num GF element layers
    GSIZET        nElemPerElemLayer; // num GF elements per GF element layer

    /*!
     * Print the header info extracted from the GeoFLOW file, along with the 
     * derived header info.
     */
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
       cout << "Num nodes per element: " << nNodesPerElem << endl;
       cout << "Num nodes per volume: " << nNodesPerVolume << endl;
       cout << "Num nodes per 2D element (x,y ref dir only): " 
            << nNodesPer2DElem << endl;
       cout << "Num element layers: " << nElemLayers << endl;
       cout << "Num elements per layer: " << nElemPerElemLayer << endl;
       cout << "Num 2D mesh layers: " << n2DLayers << endl;
       cout << "Num nodes per 2D mesh layer: " << nNodesPer2DLayer << endl;
       cout << "Num faces per 2D mesh layer: " << nFacesPer2DLayer << endl;
       cout << endl;
   }
};

#endif
