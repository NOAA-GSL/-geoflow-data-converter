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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "gheader_info.h"
#include "gnode.h"
#include "g_to_netcdf.h"

using namespace std;
namespace json = boost::property_tree;

template <class T>
class GDataConverter
{
public:
    GDataConverter() {}
    /*!
     * Read the json file that contains metadata for a given GeoFLOW dataset.
     * Metadata includes the GeoFLOW x,y,z grid filenames and variable
     * filenames to read in, and other metadata needed to write to a specific
     * NetCDF file format.
     * 
     * @param filename filename of json file
     */
    GDataConverter(const GString& filename);
    ~GDataConverter() {}

    // Access
    const vector<GNode<T>>& nodes() const { return _nodes; }

    /*!
     * Reads the GeoFLOW x,y,z grid filenames specified in the json file and
     * passes the filenames to the parameterized readGrid(...) method.
     */
    void readGrid();

    /*!
     * Read GeoFLOW x,y,z grid files and store data in a collection of nodes.
     * A GeoFLOW element layer ID is also set for each node.
     * 
     * @param xFilename GeoFLOW x grid filename
     * @param yFilename GeoFLOW y grid filename
     * @param zFilename GeoFLOW z grid filename
     */
    void readGrid(const GString& xFilename,
                  const GString& yFilename,
                  const GString& zFilename);

    /*!
     * Read GeoFLOW variable file and store data in nodes.
     * 
     * @param filename filename variable data
     */
    void readVariable(const GString& filename);

    /*!
     * Compute a lat,lon,radius for each node and store results in node.
     */
    void xyzToLatLonRadius();

    /*!
     * Write to a NetCDF file.
     */
    void writeData()
    {
        // Temp test
        GToNetCDF g(_jsonFilename, "temp.nc", NcFile::FileMode::replace);
        g.writeDimensions();
    }

    // Print
    void printHeader();

private:
    GString _jsonFilename;   // JSON filename
    json::ptree _root;       // root of JSON file that contains metadata
    GHeaderInfo _header;     // GeoFLOW file's header & other metadata
    vector<GNode<T>> _nodes; // location and variable for every node in the
                             // GeoFLOW dataset
};

#include "../src/gdata_converter.ipp"

#endif

