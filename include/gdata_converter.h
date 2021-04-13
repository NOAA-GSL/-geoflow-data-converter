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
    ~GDataConverter() {}

    // Access
    const vector<GNode<T>>& nodes() const { return _nodes; }

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
     * Write to a NetCDF file.
     */
    void writeData()
    {
        // Temp test
        GToNetCDF g(_ptFilename, "temp.nc", NcFile::FileMode::replace);
        g.writeDimensions();
        g.writeVariables();
        g.writeAttributes();
    }

    // Print
    void printHeader();

private:
    GString _ptFilename;     // name of file that contains the property tree
    pt::ptree _ptRoot;       // root of property tree
    GHeaderInfo _header;     // GeoFLOW file's header & other metadata
    vector<GNode<T>> _nodes; // location and variable for every node in the
                             // GeoFLOW dataset
};

#include "../src/gdata_converter.ipp"

#endif

