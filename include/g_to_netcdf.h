//==============================================================================
// Date        : 4/5/21 (SG)
// Description : Converts a GeoFLOW dataset to a NetCDF file. An input JSON file
//               with dimensions, variable definitions and attributes is used to
//               to write the NetCDF metadata, and a collection of GNodes is
//               used to write the data values.
// Copyright   : Copyright 2021. Regents of the University of Colorado.
//               All rights reserved.
//==============================================================================

#ifndef GTONETCDF_H
#define GTONETCDF_H

#include <map>
#include <vector>
#include <netcdf>

#include "gtypes.h"
#include "pt_util.h"

using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;

class GToNetCDF
{
public:
    /*!
     * Initialize the GeoFLOW to NetCDF file writer.
     * 
     * @param ptFilename property tree file with metadata needed for
     *                   conversion; file format is JSON
     * @param ncFilename name of NetCDF file to write to with file extension
     *                   (ex. myfile.nc)
     * @param mode read (file exists, open read-only)
     *             write (file exists, open for writing)
     *             replace (create new file, even if it exists)
     *             newFile (create new file, fail if already exists)
     */
    GToNetCDF(const GString& ptFilename,
             const GString& ncFilename,
             NcFile::FileMode mode);
    ~GToNetCDF() {}

    /*!
     * Convert a GeoFLOW data type to a NetCDF NcType.
     * 
     * @param gType GeoFLOW data type
     */
    NcType toNcType(const GString& gType);

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
    void fillDimensions(const map<GString, GSIZET>& dims);

    /*!
     * Read the "dimensions" array in the property tree and write each
     * dimension object to the NetCDF file. A dimension gets written in the
     * form: dimName = dimValue
     */
    void writeDimensions();

    /*!
     * Read the "variables" array in the property tree and write each
     * variable object to the NetCDF file. A variable gets written in the form:
     * varType varName(dim1, dim2, ...)
     *
     */
    void writeVariables();

    /*!
     * Read the "attributes" array in each object of the "variables" array
     * in the property tree and write the attributes to the NetCDF file. An
     * attribute gets written in the form: varName:attrName = "attrValue"
     */
    void writeAttributes();

    /*!
     *
     * 
     */
    void writeData(const GString& varName);

private:
    pt::ptree _ptRoot; // root of the property tree
    NcFile _nc;        // NetCDF file handle
};

#endif