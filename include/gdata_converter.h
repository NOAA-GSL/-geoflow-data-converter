//=============================================================================
// Date         : 4/1/21 (SG)
// Description  : Reads a GeoFLOW dataset (x, y, z files and a variable file)   
//              : and converts it to another file format (like UGRID or
//              : NetCDF-CF).
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//=============================================================================

#ifndef GCONVERTER_H
#define GCONVERTER_H

#include "gheader_info.h"
#include "gnode.h"

using namespace std;

template <class T>
class GDataConverter
{
public:
   /*!
     * Read GeoFLOW files that contain x,y,z loation data and store data into
     * a collection of nodes. The corresponding lat,lon,radius is computed for
     * each node. A GeoFLOW element layer ID is also stored for each node.
     * 
     * @param xFilename filename of x grid data
     * @param yFilename filename of y grid data
     * @param zFilename filename of z grid data
     */
    GDataConverter(const GString& xFilename,
                   const GString& yFilename,
                   const GString& zFilename);
    ~GDataConverter() {}

    // Access
    const vector<GNode<T>>& nodes() const { return _nodes; }

    /*!
     * Read GeoFLOW file that contains variable data and store data in nodes.
     * 
     * @param filename filename variable data
     */
    void readVariable(const GString& filename);

private:
    GHeaderInfo _header;     // GeoFLOW file's header & other meta data
    vector<GNode<T>> _nodes; // location and variable for every node in the
                             // GeoFLOW dataset
};

#include "../src/gdata_converter.ipp"

#endif

