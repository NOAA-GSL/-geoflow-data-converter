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

    void fillDimensions(const map<GString, GSIZET>& dims);
    void writeDimensions();
    void writeVariables();
    void writeAttributes();
    void writeData(const GString& varName);

private:
    pt::ptree _ptRoot; // root of the property tree
    NcFile _nc;        // NetCDF file handle
};

#endif