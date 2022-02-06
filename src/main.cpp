//==============================================================================
// Date         : 3/17/21 (SG)
// Description  : Driver program for GeoFLOW data converter.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

#include "gdata_converter.h"
#include "timer.h"

#define GDATATYPE GDOUBLE
#define G_FILE_EXT ".out"
#define NC_FILE_EXT ".nc"

// Global variables
GString jsonFile;
GDOUBLE startTime = 0, endTime = 0;

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
    // specified in the JSON file, and store in a collection of nodes. The 
    // args passed in correspond to the grid variable names in the JSON file 
    // that will store grid values.
    startTime = Timer::getTime();
    GHeaderInfo gridHeader;
    if (gdc.is_spherical())
    {
        // Dataset is a spherical grid. x,y,z values will get converted to 
        // lat,lon,radius. (mesh_node_x=lon, mesh_node_y=lat, mesh_depth=radius)
        gridHeader = gdc.readGFGridToLatLonRadNodes("mesh_node_y", "mesh_node_x", "mesh_depth");
    }
    else
    {
        // Dataset is a box grid. No conversion of x,y,z value occurs.
        // (mesh_node_x=x-axis, mesh_node_y=y-axis, mesh_depth=radius) 
        gridHeader = gdc.readGFGridToBoxNodes("mesh_node_x", "mesh_node_y", "mesh_depth");
    }
    gridHeader.printHeader();
    endTime = Timer::getTime();
    Timer::printElapsedTime(startTime, endTime, "after reading GF grid to nodes");

    // Set any 0-valued dimensions in the JSON file with the info read in from 
    // the header of a GeoFLOW grid file
    map<GString, GSIZET> dims;
    dims["nMeshNodes"] = gridHeader.nNodesPer2DLayer;
    dims["nMeshFaces"] = gridHeader.nFacesPer2DLayer;
    dims["meshLayers"] = gridHeader.n2DLayers;
    gdc.setDimensions(dims);

    //////////////////////////////
    //// READ FIELD VARIABLES ////
    //////////////////////////////

    // Read the field variables specified in the JSON file into the collection 
    // of nodes. Save the headers for each timestep so time stamp can be 
    // extracted later on. The full variable names are of the form 
    // rootVarName.timestep
    map<GString, GHeaderInfo> timeHeaderMap;

    // For each GeoFLOW variable...
    startTime = Timer::getTime();
    for (auto fullVarName : gdc.fieldVarNames())
    {
        cout << "Reading GeoFLOW variable: " << fullVarName << endl;

        // Read the variable into the collection of nodes
        GString gfFilename = fullVarName + G_FILE_EXT;
        GString timestep = gdc.extractTimestep(fullVarName);
        timeHeaderMap[timestep] = gdc.readGFVariableToNodes(gfFilename, 
                                                            fullVarName);
    }
    endTime = Timer::getTime();
    Timer::printElapsedTime(startTime, endTime, "after reading all GF field variables to nodes");

    ////////////////////
    //// SORT NODES ////
    ////////////////////

    // Sort the nodes into ascending order of element ids
    startTime = Timer::getTime();
    gdc.sortNodesByElemID();
    endTime = Timer::getTime();
    Timer::printElapsedTime(startTime, endTime, "after sorting nodes by element ID");

    // Sort the nodes into ascending order of 2D mesh layers
    startTime = Timer::getTime();
    gdc.sortNodesBy2DMeshLayer();
    endTime = Timer::getTime();
    Timer::printElapsedTime(startTime, endTime, "after sorting nodes by 2D mesh layer");

    // Create a list of face to node mappings for one mesh layer (all mesh 
    // layers have the same mapping)
    startTime = Timer::getTime();
    gdc.faceToNodes();
    endTime = Timer::getTime();
    Timer::printElapsedTime(startTime, endTime, "after creating a list of face to nodes mappings");

    startTime = Timer::getTime();
    cout << "Creating a single list of face indices" << endl;
    vector<GUINT> faceList;
    for (auto f : gdc.faces())
    {
        for (auto i : f.indices())
        {
          faceList.push_back(i);
        }
    }
    endTime = Timer::getTime();
    Timer::printElapsedTime(startTime, endTime, "after creating a single list of face indices");

    ///////////////////////////////////////////
    //// WRITE GRID / COORDINATE VARIABLES ////
    ///////////////////////////////////////////

    startTime = Timer::getTime();
    // Initialize a NetCDF file to store all time-invariant grid variables
    gdc.initNC("grid.nc", NcFile::FileMode::replace);
    gdc.writeNCDimensions();

    // Write the grid variables to the active NetCDF file
    gdc.writeNCDummyVariable("mesh");
    gdc.writeNCVariable("mesh_face_nodes", faceList);
    gdc.writeNCNodeVariable("mesh_node_x", "mesh_node_x");
    gdc.writeNCNodeVariable("mesh_node_y", "mesh_node_y");
    gdc.writeNCNodeVariable("mesh_depth", "mesh_depth");

    // Close the active NetCDF file
    gdc.closeNC();
    endTime = Timer::getTime();
    Timer::printElapsedTime(startTime, endTime, "after writing the grid variables to an nc file");

    ///////////////////////////////////
    ////// WRITE FIELD VARIABLES //////
    ///////////////////////////////////

    startTime = Timer::getTime();
    // For a given timestep, write each field variable to a separate file
    if (gdc.do_write_separate_var_files())
    {
        // For each field variable...
        for (auto fullVarName : gdc.fieldVarNames())
        {
            cout << "Converting GeoFLOW variable to nc file: " << fullVarName 
                 << endl;

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
            for (auto fullVarName : gdc.fieldVarNames())
            {
                if (fullVarName.find(timestep) != string::npos)
                {
                    cout << "Converting GeoFLOW variable to nc file: " 
                         << fullVarName << endl;

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
    endTime = Timer::getTime();
    Timer::printElapsedTime(startTime, endTime, "after writing all field variables to (an) nc file(s)");

    // For debugging
    if (gdc.do_print_nodes())
    {
        cout << "Node List: #=sorted node pos | sortID=orig node pos | eID=GF element layer ID | grid and field vars\n"
             << "---------------------------------------------------------------------------------------------------\n";
        GSIZET count = 0;
        for (auto n : gdc.nodes())
        {    
            cout << count << " - ";
            n.printNode(gdc.allVarNames());
            ++count;
        }
    }

    return 0;
}

void parseCommandLine(int argc, char** argv)
{
    if (argc == 2)
    {
        jsonFile = argv[1];
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
}

void usage(char programName[])
{
    GString progName(programName);
    GString msg = "Usage: " + progName + " <JSON_FILENAME>";
    Logger::error(__FILE__, __FUNCTION__, msg);
}