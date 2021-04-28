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
// - Add custom enum in place of second param for NcFile mode?
// - Documentation: Write min requirements template for JSON file
//   Write assumptions (i.e., everything goes in/out of in/out dirs specified
//   in JSON file, including grid files)
//   Define property tree = json file
// - Make readFiles method recursive?
// - Doxygen comments for GFileReader

#include "gdata_converter.h"

#define GDATATYPE GDOUBLE
#define FILE_JSON "data/ugrid.json"

int main()
{
    // Initialize the GeoFLOW data converter with the JSON file (property 
    // tree) that contains GeoFLOW dataset metadata and NetCDF metadata
    GDataConverter<GDATATYPE> gdc(FILE_JSON);
  
    //////////////////////////
    ////// CONVERT GRID //////
    //////////////////////////

    // Read the x,y,z grid files specified in the JSON file into a collection 
    // of nodes
    GHeaderInfo gridHeader = gdc.readGFGrid();
    gridHeader.printHeader();

    // Convert x,y,z to lat,lon,radius and store in nodes. The arguments 
    // passed in correspond to the variable names in the JSON file that store 
    // lat,lon,radius values. (For example, "mesh_node_x" corresponds to the 
    // grid's latitude values)
    gdc.xyzToLatLonRadius("mesh_node_x", "mesh_node_y", "mesh_depth");

    // Set any 0-valued dimensions in the JSON file with the info read in from 
    // the header of a GeoFLOW file
    map<GString, GSIZET> dims;
    dims["nMeshNodes"] = gridHeader.nNodesPer2DLayer;
    dims["nMeshFaces"] = gridHeader.nFacesPer2DLayer;
    dims["meshLayers"] = gridHeader.n2DLayers;
    gdc.setDimensions(dims);

    // Initialize a NetCDF file to store all time-invariant grid variables
    gdc.initNC("grid.nc", NcFile::FileMode::replace);
    gdc.writeNCDimensions();

    // Write the grid variables to the NetCDF file
    //gdc.writeNCVariable("mesh_face_nodes");
    gdc.writeNCNodeVariable("mesh_node_x");
    gdc.writeNCNodeVariable("mesh_node_y");
    gdc.writeNCNodeVariable("mesh_depth");
    gdc.closeNC();

    ///////////////////////////////
    ////// CONVERT VARIABLES //////
    ///////////////////////////////

    // For each timestep...
    for (auto i = 0u; i < gdc.numTimesteps(); ++i)
    {
        bool wroteTimeStamp = false;

        // Get timestep as a string
        stringstream ss;
        ss << std::setfill('0') << std::setw(6) << i;
        GString timestep = ss.str();

        // Initialize a NetCDF file for this timestep to store all the field 
        // variables
        GString ncFilename = "vars." + timestep + ".nc";
        gdc.initNC(ncFilename, NcFile::FileMode::replace);
        gdc.writeNCDimensions();

        // For each field variable in this timestep...
        for (auto fieldName : gdc.fieldVarNames())
        {
            cout << "Converting GeoFLOW variable: " << fieldName << " for " \
                 << "timestep: " << timestep << endl;

            // Verify file - omit this code section if want to fail if file not 
            // found
            GString gfFilename = fieldName + "." + timestep + ".out";
            GString gfFullPath = gdc.inputDir() + "/" + gfFilename;
            if (!gdc.fileExists(gfFullPath))
            {
                GString msg = "Cannot find file (" + gfFullPath + "). " + \
                              "Skipping.";
                Logger::warning(__FILE__, __FUNCTION__, msg);
                continue;
            }

            // Read the GeoFLOW field variable into collection of nodes
            GHeaderInfo fieldHeader = gdc.readGFVariable(gfFilename, fieldName);

            // Write the time stamp variable to the NetCDF file; only want do
            // this once per timestep file
            if (!wroteTimeStamp)
            {
                gdc.writeNCVariable("time", fieldHeader.timeStamp);
                wroteTimeStamp = true;
            }

            // Write the variable to the NetCDF file
            gdc.writeNCNodeVariable(fieldName);
        }
        
        // Close the NetCDF file
        gdc.closeNC();
    }

    // For debugging
    /*for (auto n : gdc.nodes())
    {
        n.printNode();
    }*/

    return 0;
}