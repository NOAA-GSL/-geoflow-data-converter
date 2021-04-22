//==============================================================================
// Date         : 3/17/21 (SG)
// Description  : Driver program for GeoFLOW data converter.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

// TODO across project
// - Add error handling for NetCDF calls
// - Modify nElem in gfile_reader.h (right now, only accounts for one layer of
//   2D elements); also compute n2DLayers - right now hard-coded; also compute
//   setElementLayerIDs
// - MathUtil essentiallyEqual method
// - Remove nNodesPerFace from json file and hard code it? Assumption is
//   poly order same for each element in any one ref direction

#include "gdata_converter.h"

#define GDATATYPE GDOUBLE
#define FILE_JSON "data/ugrid.json"

int main()
{
    // Read the JSON file that specifies the GeoFLOW dataset's metadata and
    // UGRID metadata needed for converting the GeoFLOW dataset to a specific
    // NetCDF file format
    GDataConverter<GDATATYPE> gdc(FILE_JSON);
  
    // Read the x,y,z grid files specified in the JSON file into a collection
    // of nodes
    gdc.readGFGrid();

    // Convert x,y,z to lat,lon,radius and store in nodes
    gdc.xyzToLatLonRadius();

    // Set any 0-valued dimensions in the JSON file dynamcailly with the info
    // read in from the header of a GeoFLOW dataset file.
    map<GString, GSIZET> dims;
    dims["nMeshNodes"] = (gdc.header()).nNodesPer2DLayer;
    dims["nMeshFaces"] = (gdc.header()).nFacesPer2DLayer;
    dims["meshLayers"] = (gdc.header()).n2DLayers;
    gdc.setDimensions(dims);

    // Write NetCDF grid (i.e., all time invariant coordinate variables)
    gdc.initNC("grid.nc", NcFile::FileMode::replace);
    gdc.writeNCDimensions();
    gdc.writeNCVariable("mesh_face_nodes"); // Error need a template on this function vs. class level
    gdc.writeNCVariable("mesh_node_x");
    gdc.writeNCVariable("mesh_node_y");
    gdc.writeNCVariable("mesh_depth");
    gdc.closeNC();

    // For debugging
    gdc.printHeader();

    // For debugging
    /*for (auto n : gdc.nodes())
    {
        n.printNode();
    }*/

    return 0;
}