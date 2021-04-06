//==============================================================================
// Date         : 3/17/21 (SG)
// Description  : Driver program for GeoFLOW data converter.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

#include "gdata_converter.h"

#define GDATATYPE GDOUBLE
#define FILE_JSON "data/ugrid.json"

int main()
{
    // Read the json file that specifies the GeoFLOW data files and contains
    // other metadata needed for converting the GeoFLOW dataset to a specific
    // NetCDF file format
    GDataConverter<GDATATYPE> gdc(FILE_JSON);
  
    // Read the x,y,z grid into GNodes and convert to lat,lon,radius
    gdc.readGrid();

    // Convert x,y,z grid to lat,lon,radius and store in nodes
    gdc.xyzToLatLonRadius();

    // Print header
    gdc.printHeader();

    // Print nodes
    /*for (auto n : gdc.nodes())
    {
        n.printNode();
    }*/

    return 0;
}
