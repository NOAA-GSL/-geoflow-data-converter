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
//   setElementLayerIDs when the element ID becomes available in header of GF
//   files
// - MathUtil essentiallyEqual method
// - Write of NCVariable: just writing all the nodes - need to divide into
//   layers
// - Handle timestamps - loop through filenames in input directory
//   Read in time value from header while iterating files and direct store in
//   current NcFile??
// - Add custom enum in place of second param for NcFile mode?
// - Documentation: Write min requirements template for JSON file
// - readFiles - make it recursive

#include "gdata_converter.h"

#define GDATATYPE GDOUBLE
#define FILE_JSON "data/ugrid.json"

int main()
{
    // Read the JSON file that specifies the GeoFLOW dataset's metadata and
    // NetCDF metadata needed for converting the GeoFLOW dataset to a specific
    // NetCDF file format (i.e., UGRID or CF)
    GDataConverter<GDATATYPE> gdc(FILE_JSON);
  
    //////////////////////////
    ////// CONVERT GRID //////
    //////////////////////////

    // Read the x,y,z grid files specified in the JSON file into a collection
    // of nodes
    gdc.readGFGrid();

    // Convert x,y,z to lat,lon,radius and store in nodes. The arguments
    // correspond to the variable names of the lat,lon,radius values and should
    // match the corresponding variable names in the JSON file. (For example,
    // "mesh_node_x" corresponds to the grid's latitude values)
    gdc.xyzToLatLonRadius("mesh_node_x", "mesh_node_y", "mesh_depth");

    // Set any 0-valued dimensions in the JSON file with the info read in from
    // the header of a GeoFLOW dataset file
    map<GString, GSIZET> dims;
    dims["nMeshNodes"] = (gdc.header()).nNodesPer2DLayer;
    dims["nMeshFaces"] = (gdc.header()).nFacesPer2DLayer;
    dims["meshLayers"] = (gdc.header()).n2DLayers;
    gdc.setDimensions(dims);

    // Write grid (i.e., all time invariant coord variables) to a NetCDF file
    gdc.initNC("grid.nc", NcFile::FileMode::replace);
    gdc.writeNCDimensions();
    //gdc.writeNCVariable("mesh_face_nodes");
    gdc.writeNCVariable("mesh_node_x");
    gdc.writeNCVariable("mesh_node_y");
    gdc.writeNCVariable("mesh_depth");
    gdc.closeNC();

    ///////////////////////////////
    ////// CONVERT VARIABLES //////
    ///////////////////////////////

    // Get a list of the field variables


    // For each timestep...
    /*for ()
    {
        // Create a NetCDF file for this timestep to hold all the field
        // variables
        GString ncFilename = "field_var.000000.nc";
        gdc.initNC(ncFilename, NcFile::FileMode::replace);
        gdc.writeNCDimensions();

        // For each field variable in this timestep
        for ()
        {
            // Read a GeoFLOW variable file specified in the JSON file into the
            // collection of nodes
            GString gfFilename = "data/dtotal.000000.out";
            GString fieldName = "dtotal";
            gdc.readGFVariable(gfFilename, fieldName);

            // Write the timestep
            //gdc.writeNCVariable("time");

            // Write the variable to the NetCDF file
            gdc.writeNCVariable(varName);
        }
        
        // Close the NetCDF file
        gdc.closeNC();
    }*/

    // For debugging
    gdc.printHeader();

    // For debugging
    for (auto n : gdc.nodes())
    {
        n.printNode();
    }

    return 0;
}