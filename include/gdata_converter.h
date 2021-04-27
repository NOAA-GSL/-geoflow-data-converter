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

#include <vector>

#include "gheader_info.h"
#include "gnode.h"
#include "g_to_netcdf.h"
#include "pt_util.h"

using namespace std;

template <class T>
class GDataConverter
{
public:
    GDataConverter() {}
    /*!
     * Reads a property tree file that contains metadata for a given GeoFLOW
     * dataset. Metadata includes the GeoFLOW x,y,z grid filenames and variable
     * filenames to read in, and other metadata needed to write to a specific
     * NetCDF file format.
     * 
     * @param filename name of property tree; file format is JSON
     */
    GDataConverter(const GString& ptFilename);
    ~GDataConverter();

    // Access
    const vector<GNode<T>>& nodes() const { return _nodes; }
    const GHeaderInfo header() const { return _header; }
    vector<GString> fieldVarNames() const { return _fieldVarNames; }
    GUINT numTimesteps() const { return _numTimesteps; }

    /*!
     * Create a directory if it does not exist.
     * 
     * @param dirName name of the directory to create
     */
    void makeDirectory(const GString& dirName);

    /*!
     * Get a list of absolute filenames in the given directory. Does not search
     * recursively at the moment.
     * 
     * @param dirName the directory to read
     * @return a list of absolute filenames in the directory
     */
    vector<GString> getFilenames(const GString& dirName);

    /*!
     * Reads the GeoFLOW x,y,z grid filenames specified in the property tree
     * and passes the filenames to the parameterized readGrid(...) method.
     */
    void readGFGrid();

    /*!
     * Read GeoFLOW x,y,z grid files and store data in a collection of nodes.
     * A GeoFLOW element layer ID is also set for each node.
     * 
     * @param gfXFilename GeoFLOW x grid filename
     * @param gfYFilename GeoFLOW y grid filename
     * @param gfZFilename GeoFLOW z grid filename
     */
    void readGFGrid(const GString& gfXFilename,
                    const GString& gfYFilename,
                    const GString& gfZFilename);

    /*!
     * Read GeoFLOW variable file and store data in nodes.
     * 
     * @param gfFilename GeoFLOW variable filename
     * @param varName name of variable in nodes used to store the data
     */
    void readGFVariable(const GString& gfFilename, const GString& varName);

    /*!
     * Compute a lat,lon,radius for each node. A new variable for each
     * lat,lon,radius with the input varNames gets created in the node
     * if it does not exist.
     * 
     * @param latVarName name of latitude variable
     * @param lonVarName name of longitude variable
     * @param radVarName name of radius variable
     */
    void xyzToLatLonRadius(const GString& latVarName,
                           const GString& lonVarName,
                           const GString& radVarName);

    /*!
     * Replace any 0-valued dimensions in the property tree with the matching
     * dimensions specified in the dimensions map. A 0-valued dimension means
     * the dimension's value must be computed during runtime after reading a
     * GeoFLOW data file. The name of a dimension in the map must match the
     * name of a 0-valued dimension in the property tree.
     * 
     * @param dims map of key-value pairs of any dimensions that must be
     *             computed dynamically during runtime
     */
    void setDimensions(const map<GString, GSIZET>& dims);

    /*!
     * Initialize a GToNetCDF object with the converter's property tree.
     *
     * @param ncFilename name of NetCDF file to write to; the file will be
     *                   placed in the property tree's output directory
     * @param mode NcFile::FileMode::read (file exists, open read-only)
     *             NcFile::FileMode::write (file exists, open for writing)
     *             NcFile::FileMode::replace (create new file, even if it
     *             exists)
     *             NcFile::FileMode::newFile (create new file, fail if already
     *             exists)
     */
    void initNC(const GString& ncFilename, NcFile::FileMode mode);

    /*!
     * Clean memory for the GToNetCDF object
     */
    void closeNC();

    /*!
     * Write the dataset's dimensions to the current NetCDF file
     *
     */
    void writeNCDimensions();

    /*!
     * Write the variable definition, variable attributes, and variable data to
     * the current NetCDF file.
     * 
     * @param varName name of variable in the property tree
     */
    void writeNCVariable(const GString& varName);

    // Print
    void printHeader();

private:
    GString _ptFilename;     // name of file that contains the property tree
    pt::ptree _ptRoot;       // root of property tree
    GHeaderInfo _header;     // GeoFLOW file's header & other metadata
    GToNetCDF *_nc;          // handle to NetCDF writer 
    vector<GNode<T>> _nodes; // location and variable for every node in the
                             // GeoFLOW dataset
    GString _inputDir;       // name of directory that holds all GeoFLOW files
    GString _outputDir;      // name of directory to put the converted files in
    GUINT _numTimesteps;     // number of timesteps to convert
    vector<GString> _fieldVarNames;  // names of the field variables (i.e.,
                                     // prefix of the field variable filenames)
    vector<GString> _fieldFilenames; // absolute pathnames of input GeoFLOW
                                     // field files
};

#include "../src/gdata_converter.ipp"

#endif

