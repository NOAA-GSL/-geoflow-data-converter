//==============================================================================
// Date         : 3/17/21 (SG)
// Description  : Driver program for GeoFLOW data converter.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

#include "gdata_converter.h"

#define GDATATYPE GDOUBLE
#define FILE_XGRID "data/xgrid.000000.out"
#define FILE_YGRID "data/ygrid.000000.out"
#define FILE_ZGRID "data/zgrid.000000.out"
#define FILE_VARIABLE "data/dtotal.000000.out"

int main()
{
    // Read GeoFLOW x,y,z grid files into a collection of nodes
    GDataConverter<GDATATYPE> gdc(FILE_XGRID, FILE_YGRID, FILE_ZGRID);

    // Read a GeoFLOW variable file into the nodes
    gdc.readVariable(FILE_VARIABLE);

    // Print nodes
    for (auto n : gdc.nodes())
    {
        n.printNode();
    }

    return 0;
}
