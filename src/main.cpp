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
- Is custom g_to_netcdf::putAttribute() method actually needed?
  - Check attribute values: some ints are set as strings and then converted?
  - See Fill Value for mesh_face_nodes (what should it's type be?)
- Documentation:
  - Write min requirements template for JSON file
  - Write assumptions (i.e., everything goes in/out of in/out dirs specified
    in JSON file, including grid files); Define property tree = json file
  - Doxygen comments for GFileReader
- Fill in missing values for variable attributes in json file
- Confirm timestamp
*/

#include "gdata_converter.h"

#define GDATATYPE GDOUBLE
#define G_FILE_EXT ".out"
#define NC_FILE_EXT ".nc"

// Global variables
GString jsonFile;
GINT writeOneFile = 0;

void parseCommandLine(int argc, char** argv);
void usage(char programName[]);

int main(int argc, char** argv)
{
    // Parse command line arguments
    parseCommandLine(argc, argv);

    // Initialize the GeoFLOW data converter with the JSON file (property 
    // tree) that contains metadata for the GeoFLOW dataset and for writing 
    // NetCDF-UGRID files
    GDataConverter<GDATATYPE> gdc(jsonFile);
  
    ///////////////////////////////////
    //// READ COORDINATE VARIABLES ////
    ///////////////////////////////////

    // Read the x,y,z GeoFLOW grid files (i.e., coordinate variables) 
    // specified in the JSON file into a collection of nodes
    GHeaderInfo gridHeader = gdc.readGFGridToNodes();
    gridHeader.printHeader();

    // Set any 0-valued dimensions in the JSON file with the info read in from 
    // the header of a GeoFLOW grid file
    map<GString, GSIZET> dims;
    dims["nMeshNodes"] = gridHeader.nNodesPer2DLayer;
    dims["nMeshFaces"] = gridHeader.nFacesPer2DLayer;
    dims["meshLayers"] = gridHeader.n2DLayers;
    gdc.setDimensions(dims);

    // Convert the x,y,z values to lat,lon,radius and store in nodes. The 
    // arguments passed in correspond to the variable names in the JSON file 
    // that store lat,lon,radius values. (For example, "mesh_node_x" 
    // corresponds to the grid's latitude values)
    gdc.xyzToLatLonRadius("mesh_node_x", "mesh_node_y", "mesh_depth");

    //////////////////////////////
    //// READ FIELD VARIABLES ////
    //////////////////////////////

    // Read the field variables specified in the JSON file into the collection 
    // of nodes. Save the headers for each timestep so time stamp can be 
    // extracted later on. The full variable names are of the form 
    // rootVarName.timestep
    map<GString, GHeaderInfo> timeHeaderMap;

    // For each GeoFLOW variable...
    for (auto fullVarName : gdc.fullVarNames())
    {
        cout << "Reading GeoFLOW variable: " << fullVarName << endl;

        // Read the variable into the collection of nodes
        GString gfFilename = fullVarName + G_FILE_EXT;
        GString timestep = gdc.extractTimestep(fullVarName);
        timeHeaderMap[timestep] = gdc.readGFVariableToNodes(gfFilename, 
                                                            fullVarName);
    }

    ////////////////////
    //// SORT NODES ////
    ////////////////////

    // Sort the nodes into ascending order of element ids
    gdc.sortNodesByElemID();

    // Sort the nodes into ascending order of 2D mesh layers
    gdc.sortNodesBy2DMeshLayer();

    // Create a list of face to node mappings for one mesh layer (all mesh 
    // layers have the same mapping)
    gdc.faceToNodes();
    vector<GUINT> faceList;
    for (auto f : gdc.faces())
    {
        for (auto i : f.indices())
        {
          faceList.push_back(i);
        }
    }

    ////////////////////////////////////
    //// WRITE COORDINATE VARIABLES ////
    ////////////////////////////////////

    // Initialize a NetCDF file to store all time-invariant grid variables
    gdc.initNC("grid.nc", NcFile::FileMode::replace);
    gdc.writeNCDimensions();

    // Write the grid variables to the active NetCDF file
    gdc.writeNCVariable("mesh_face_nodes", faceList);
    gdc.writeNCNodeVariable("mesh_node_x", "mesh_node_x");
    gdc.writeNCNodeVariable("mesh_node_y", "mesh_node_y");
    gdc.writeNCNodeVariable("mesh_depth", "mesh_depth");

    // Close the active NetCDF file
    gdc.closeNC();

    ///////////////////////////////////
    ////// WRITE FIELD VARIABLES //////
    ///////////////////////////////////

    // For a given timestep, write each field variable to a separate file
    if (!writeOneFile)
    {
        // For each field variable...
        for (auto fullVarName : gdc.fullVarNames())
        {
            cout << "Converting GeoFLOW variable: " << fullVarName << endl;

            // Initialize a NetCDF file for this timestep to store this field 
            // variable
            GString ncFilename = fullVarName + NC_FILE_EXT;
            gdc.initNC(ncFilename, NcFile::FileMode::replace);
            gdc.writeNCDimensions();

            // Write the time stamp variable to the active NetCDF file
            GString timestep = gdc.extractTimestep(fullVarName);
            gdc.writeNCVariable("time", timeHeaderMap[timestep].timeStamp);

            // Write the field variable to the active NetCDF file
            GString rootVarName = gdc.extractRootVarName(fullVarName);
            gdc.writeNCNodeVariable(rootVarName, fullVarName);
   
            // Close the active NetCDF file
            gdc.closeNC();
        }
    }
    else // for a given timestep, write all field variables to the same file
    {
        // For each timestep...
        for (auto t : timeHeaderMap)
        {
            // Get timestep as a string
            GBOOL wroteTimeStamp = false;
            GString timestep = t.first;

            // Initialize a NetCDF file for this timestep to store all the  
            // field variables
            GString ncFilename = "vars." + timestep + NC_FILE_EXT;
            gdc.initNC(ncFilename, NcFile::FileMode::replace);
            gdc.writeNCDimensions();

            // For each variable at this timestep...
            for (auto fullVarName : gdc.fullVarNames())
            {
                if (fullVarName.find(timestep) != string::npos)
                {
                    cout << "Converting GeoFLOW variable: " << fullVarName 
                         << endl;

                    if (!wroteTimeStamp)
                    {
                        // Write the time stamp variable to the active NetCDF 
                        // file; since all vars getting written to the same 
                        // file, only want to write the time stamp variable 
                        // once
                        gdc.writeNCVariable("time", (t.second).timeStamp);
                        wroteTimeStamp = true;
                    }

                    // Write the field variable to the active NetCDF file
                    GString rootVarName = gdc.extractRootVarName(fullVarName);
                    gdc.writeNCNodeVariable(rootVarName, fullVarName);
                }
            }
            
            // Close the active NetCDF file
            gdc.closeNC();
        }
    }

    // For debugging
    for (auto n : gdc.nodes())
    {    
        n.printNode();
    }

    return 0;
}

void parseCommandLine(int argc, char** argv)
{
    if (argc == 2)
    {
        jsonFile = argv[1];
    }
    else if (argc == 3)
    {
        jsonFile = argv[1];
        writeOneFile = atoi(argv[2]);
        if (strcmp(argv[2], "0") && strcmp(argv[2], "1"))
        {
            string arg(argv[2]);
            GString msg = "Invalid command line argument: " + arg;
            Logger::error(__FILE__, __FUNCTION__, msg);
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        GString progName(argv[0]);
        GString msg = "Missing command line arguments.";
        Logger::error(__FILE__, __FUNCTION__, msg);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    cout << "Using JSON file: " << jsonFile << endl;
    cout << "Writing field variables to one file (0=no, 1=yes)? " 
         << writeOneFile << endl;
}

void usage(char programName[])
{
    GString progName(programName);
    GString msg = "Usage: " + 
                  progName + 
                  " <JSON_FILENAME> " \
                  "[0|1] (optional: 0=default=write separate files, 1=write " \
                  "one file)";
    Logger::error(__FILE__, __FUNCTION__, msg);
}