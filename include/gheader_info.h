//==============================================================================
// Date        : 3/17/21 (SG)
// Description : Stores header and auxiliary info from a binary GeoFLOW data
//               file.
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
    GSIZET        timeCycle;     // time cycle stamp
    GSIZET        timeStamp;     // time stamp
    GUINT         hasMultVars;   // multiple fields in file?

    // Auxiliary data derived from header
    GSIZET        nHeaderBytes;  // total byte size of header
    GUINT         nNodesPerElem; // num nodes per element (incl. sub nodes)
    GSIZET        nNodes;        // total nodes in file (incl. sub nodes)
};

#endif
