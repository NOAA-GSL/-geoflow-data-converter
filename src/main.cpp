//==============================================================================
// Date         : 3/17/21 (SG)
// Description  : Driver program for GeoFLOW data converter.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

/*
-------------------
TODO ACROSS PROJECT
-------------------
- Add error handling for NetCDF calls
- When element IDs become available in the GeoFLOW header files:
  - modify h.nElemLayers gfile_reader::readHeader (right now, only accounts
    one layer of GeoFLOW elements)
  - modify gfile_reader::setElementLayerIDs()
- g_to_netcdf::writeVariableData is allocating all node data but this is not 
  needed for all vars since, for ex, mesh_nodes_x is the same for each layer;
  it is getting correctly written to the NetCDF file, I'm guessing b/c of the
  dimensions that were alreay assigned to the variable; Writing of the single
  variable seems hacky - update.
- Implement MathUtil::essentiallyEqual() method
- Add custom enum in place of second param for NcFile mode to remove Nc
  dependency?
- Is custom g_to_netcdf::putAttribute() method actuall needed?
  - Check attribute values: some ints are set as strings and then converted?
- Documentation:
  - Write min requirements template for JSON file
  - Write assumptions (i.e., everything goes in/out of in/out dirs specified
    in JSON file, including grid files); Define property tree = json file
  - Doxygen comments for GFileReader
*/

#include "gdata_converter.h"

#define GDATATYPE GDOUBLE

int main(int argc, char** argv)
{
    // Read command line arguments
    GString jsonFile;
    if (argc > 1)
    {
        jsonFile = argv[1];
    }
    else
    {
        GString progName(argv[0]);
        GString msg = "No command line argument with input JSON file was " \
                      "provided.\nUsage: " + progName + " JSON_FILENAME";
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // Initialize the GeoFLOW data converter with the JSON file (property 
    // tree) that contains metadata for the GeoFLOW dataset and for writing 
    // NetCDF-UGRID files
    GDataConverter<GDATATYPE> gdc(jsonFile);
  
    //////////////////////////
    ////// CONVERT GRID //////
    //////////////////////////

    // Read the x,y,z GeoFLOW grid files specified in the JSON file into a 
    // collection of nodes
    GHeaderInfo gridHeader = gdc.readGFGrid();
    gridHeader.printHeader();

    // Convert the x,y,z values to lat,lon,radius and store in nodes. The 
    // arguments passed in correspond to the variable names in the JSON file 
    // that store lat,lon,radius values. (For example, "mesh_node_x" 
    // corresponds to the grid's latitude values)
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

    // Write the grid variables to the active NetCDF file
    //gdc.writeNCVariable("mesh_face_nodes"); // TODO
    gdc.writeNCNodeVariable("mesh_node_x");
    gdc.writeNCNodeVariable("mesh_node_y");
    gdc.writeNCNodeVariable("mesh_depth");

    // Close the active NetCDF file
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

        // Initialize a NetCDF file for this timestep to store all field 
        // variables
        GString ncFilename = "vars." + timestep + ".nc";
        gdc.initNC(ncFilename, NcFile::FileMode::replace);
        gdc.writeNCDimensions();

        // For each variable in this timestep...
        for (auto varName : gdc.varNames())
        {
            cout << "Converting GeoFLOW variable: " << varName << " for " \
                 << "timestep: " << timestep << endl;

            // Verify file - omit this code section if want to fail if file not 
            // found
            GString gfFilename = varName + "." + timestep + ".out";
            GString gfFullPath = gdc.inputDir() + "/" + gfFilename;
            if (!gdc.fileExists(gfFullPath))
            {
                GString msg = "Cannot find file (" + gfFullPath + "). " + \
                              "Skipping.";
                Logger::warning(__FILE__, __FUNCTION__, msg);
                continue;
            }

            // Read the GeoFLOW variable into the collection of nodes
            GHeaderInfo fieldHeader = gdc.readGFNodeVariable(gfFilename, varName);

            // Write the time stamp variable to the active NetCDF file; only 
            // want to do this once per timestep file
            if (!wroteTimeStamp)
            {
                gdc.writeNCVariable("time", fieldHeader.timeStamp);
                wroteTimeStamp = true;
            }

            // Write the variable to the active NetCDF file
            gdc.writeNCNodeVariable(varName);
        }
        
        // Close the active NetCDF file
        gdc.closeNC();
    }

    // For debugging
    /*for (auto n : gdc.nodes())
    {
        n.printNode();
    }*/

    return 0;
}