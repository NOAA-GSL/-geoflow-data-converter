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

    /*!
     * Reads the GeoFLOW x,y,z grid filenames specified in the property tree
     * and passes the filenames to the parameterized readGrid(...) method.
     */
    void readGFGrid();

    /*!
     * Read GeoFLOW x,y,z grid files and store data in a collection of nodes.
     * A GeoFLOW element layer ID is also set for each node.
     * 
     * @param xFilename GeoFLOW x grid filename
     * @param yFilename GeoFLOW y grid filename
     * @param zFilename GeoFLOW z grid filename
     */
    void readGFGrid(const GString& xFilename,
                    const GString& yFilename,
                    const GString& zFilename);

    /*!
     * Read GeoFLOW variable file and store data in nodes.
     * 
     * @param filename filename variable data
     */
    void readGFVariable(const GString& filename);

    /*!
     * Compute a lat,lon,radius for each node and store results in node.
     */
    void xyzToLatLonRadius();

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
     *
     */
    void initNC(const GString& ncFilename, NcFile::FileMode mode);

    /*
     *
     */
    void closeNC();

    /*
     *
     */
    void writeNCDimensions();

    /*
     *
     */
    template <class U>
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
};

#include "../src/gdata_converter.ipp"

#endif

