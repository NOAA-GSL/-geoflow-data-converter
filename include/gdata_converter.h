//=============================================================================
// Date         : 4/1/21 (SG)
// Description  : Reads a GeoFLOW dataset (x, y, z files and variable files) 
//              : and converts it to a set of NetCDF files (like UGRID or CF).
// Copyright    : Copyright 2021. Regents of the University of Colorado. 
//                All rights reserved.
//=============================================================================

#ifndef GCONVERTER_H
#define GCONVERTER_H

#include <vector>

#include "gheader_info.h"
#include "gnode.h"
#include "gface.h"
#include "g_to_netcdf.h"
#include "pt_util.h"

using namespace std;

template <class T>
class GDataConverter
{
public:
    GDataConverter() {}
    /*!
     * Constructor: Reads a property tree file that contains metadata for a 
     * given GeoFLOW dataset. Metadata includes the GeoFLOW x,y,z grid 
     * filenames and variable filenames to read in, and other metadata needed 
     * to write to NetCDF files.
     * 
     * @param filename name of property tree; file format is JSON
     */
    GDataConverter(const GString& ptFilename);

    ~GDataConverter();

    // Access
    GString outputDir() const { return _outputDir; }
    GString inputDir() const { return _inputDir; }
    GUINT numTimesteps() const { return _numTimesteps; }
    const vector<GString>& fieldVarNames() const { return _fieldVarNames; }
    const vector<GString>& allVarNames() const { return _allVarNames; }
    const vector<GNode<T>>& nodes() const { return _nodes; }
    const vector<GFace>& faces() const { return _faces; }

    /*!
     * Get the names of the grid and timestepped variables.
     */
    void readVariableNames();

    /*!
     * Create a directory if it does not exist.
     * 
     * @param dirName name of the directory to create
     */
    void makeDirectory(const GString& dirName);

    /*!
     * Check if a file exists.
     * 
     * @param filename name of file to check for
     * @return true if file exists; false otherwise
     */
    bool fileExists(const GString& filename);

    /*!
     * Get the index corresponding to the variable name.
     *
     * @param varName name of the variable 
     * @return index corresponding to variable name
     */
    GUINT toVarIndex(const GString& varName);

    /*!
     * Reads the GeoFLOW x,y,z grid filenames specified in the property tree,  
     * convert x,y,z to lat,lon,rad and store in a collection of nodes. A 
     * GeoFLOW element layer ID is also set for each node based on data in the 
     * header of the GeoFLOW file.
     * 
     * @param latVarName name of latitude variable in property tree
     * @param lonVarName name of longitude variable in property tree
     * @param radVarName name of radius variable in property tree
     * 
     * @return the header info for the grid files read in (the header should 
     *         be the same for each time-invariant x,y,z file)
     */
    GHeaderInfo readGFGridToLatLonRadNodes(const GString& latVarName,
                                           const GString& lonVarName,
                                           const GString& radVarName);

    /*!
     * Read the GeoFLOW x,y,z grid filenames specified in the property tree  
     * and store x,y,z into a collection of nodes. x,y corresponds to the 2D 
     * horizontal mesh layer, and z corresponds to the upward/elevation axis.  
     * A GeoFLOW element layer ID is also set for each node based on data in 
     * the header of the GeoFLOW file.
     * 
     * @param xVarName name of x variable in property tree
     * @param yVarName name of y variable in property tree
     * @param zVarName name of z/elevation variable in property tree
     * 
     * @return the header info for the grid files read in (the header should 
     *         be the same for each time-invariant x,y,z file)
     */
    GHeaderInfo readGFGridToBoxNodes(const GString& xVarName, 
                                     const GString& yVarName, 
                                     const GString& zVarName);

    /*!
     * Read GeoFLOW variable file and store data in nodes. Assumes the 
     * correct number of nodes have already been initialized by the readGrid() 
     * method.
     * 
     * @param gfFilename GeoFLOW variable filename
     * @param varName name of variable in nodes to store the data into
     * @return the header info for the file read in
     */
    GHeaderInfo readGFVariableToNodes(const GString& gfFilename,
                                      const GString& varName);

    /*!
     * Sort nodes by the nodes' GeoFLOW element IDs (bottom to top).
     */
    void sortNodesByElemID();

    /*!
     * Sort nodes by 2D mesh layers (bottom to top) based on the nodes' sort 
     * keys.
     */
    void sortNodesBy2DMeshLayer();

    /*!
     * Create a list of face to node mappings for one mesh layer (all mesh 
     * layers have the same mapping).
     */
    void faceToNodes();

    /*!
     * Get the timestep from the timestepped variable name.
     *
     * @param varName name of the variable
     * @return timestep (e.g., 000001)
     */
    GString extractTimestep(GString varName);

    /*!
     * Get the root variable name from the timestepped variable name.
     *
     * @param varName name of the timestepped variable (e.g., v1.000001)
     * @return root variable name (e.g., v1)
     */
    GString extractRootVarName(GString varName);

    /*!
     * Replace any 0-valued dimensions in the property tree with the matching 
     * dimensions specified in the input dimensions map. A 0-valued dimension 
     * means the dimension's value must be computed during runtime after 
     * reading a GeoFLOW data file. The name of a dimension in the map must 
     * match the name of a 0-valued dimension in the property tree.
     * 
     * @param dims map of key-value pairs of any dimensions that must be 
     *             computed dynamically during runtime
     */
    void setDimensions(const map<GString, GSIZET>& dims);

    /*!
     * Initialize a GToNetCDF object (makes NetCDF API calls) with the 
     * converter's property tree and the NetCDF file to write to. This file
     * becomes the active NetCDF file for writing until closeNC() is called.
     *
     * @param ncFilename name of NetCDF file to write to; the file will be 
     *                   placed in the output directory listed in the 
     *                   property tree 
     * @param mode NcFile::FileMode::read (file exists, open read-only), 
     *             NcFile::FileMode::write (file exists, open for writing), 
     *             NcFile::FileMode::replace (create new file, even if it 
     *             exists), 
     *             NcFile::FileMode::newFile (create new file, fail if already 
     *             exists)
     */
    void initNC(const GString& ncFilename, NcFile::FileMode mode);

    /*!
     * Closes the active NetCDF file and deletes the GToNetCDF object.
     */
    void closeNC();

    /*!
     * Use dimensions in the property tree to write the dataset's dimensions 
     * to the active NetCDF file.
     *
     */
    void writeNCDimensions();

    /*!
     * Write the variable definition and variable attributes of a dummy  
     * variable to the active NetCDF file. A dummy variable has no data 
     * associated with it.
     * 
     * @param varName name of a variable in the property tree
     */
    void writeNCDummyVariable(const GString& varName);

    /*!
     * Write the variable definition, variable attributes, and variable data 
     * to the active NetCDF file. The data comes from the converter's 
     * collection of nodes that match the input varName.
     * 
     * @param rootVarName root name of variable in the property tree
     * @param fullVarName full name of variable (i.e., rootVarName.timestep) 
     *                    (should be the same name as a variable name stored 
     *                    in the nodes)
     */
    void writeNCNodeVariable(const GString& rootVarName, 
                             const GString& fullVarName);

    /*!
     * Write the variable definition, variable attributes, and a single data  
     * value to the active NetCDF file.
     * 
     * @param varName name of variable in the property tree
     * @param varValue single data value to write for the variable
     */
    template <typename U>
    void writeNCVariable(const GString& varName, const U& varValue);

    /*!
     * Write the variable definition, variable attributes, and variable data 
     * to the active NetCDF file.
     * 
     * @param varName name of variable in the property tree
     * @param values vector of values to write for the variable
     */
    template<typename U>
    void writeNCVariable(const GString& varName, const vector<U>& values);

private:
    GString _ptFilename;     // filename that contains the property tree
    pt::ptree _ptRoot;       // root of property tree
    GToNetCDF *_nc;          // handle to NetCDF writer 
    GHeaderInfo _header;     // header of a GeoFLOW grid file
    vector<GNode<T>> _nodes; // location and variable data for every node in 
                             // the GeoFLOW dataset
    vector<GFace> _faces;    // the faces that make up one 2D layer (x,y ref 
                             // dir) of the GeoFLOW dataset
    GString _inputDir;       // directory name of input GeoFLOW files
    GString _outputDir;      // directory name of output NetCDF files
    GUINT _numTimesteps;     // number of timesteps to convert
    vector<GString> _allVarNames; // all var names (grid & timestepped field)
    vector<GString> _fieldVarNames; // timestepped field variables names
                                    // (i.e., root_name.timestep)    
};

#include "../src/gdata_converter.ipp"

#endif

