//==============================================================================
// Module       : gheader_info.h
// Date         : 4/17/21 (SG)
// Description  : Read header info from a binary GeoFLOW data file
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
// Derived From : none
//==============================================================================

#ifndef GHEADERINFO_H
#define GHEADERINFO_H

#include "gtypes.h"
#include <vector>

#define DEFAULT_DATA_SIZE 8

using namespace std;

struct GHeaderInfo
{
    GUINT         version;      // version number
    GUINT         dim;            // dimension of elements (2 or 3)
    GSIZET        numElems;       // number of elements
    vector<GUINT> polyOrders;     // poly order of each dim, array of sz dim
    GUINT         gridType;       // grid type of GeoFLOW type GElemType
    GSIZET        timeCycle;      // time cycle stamp
    GSIZET        timeStamp;      // time stamp
    GUINT         hasMultVars;    // multiple fields in file?
    GSIZET        numHeaderBytes; // total byte size of header
    GSIZET        dataSize;       // size of data value in bytes (4 or 8)
};

#endif
