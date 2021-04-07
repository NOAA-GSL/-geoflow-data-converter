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

#include <vector>
#include <netcdf>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "gtypes.h"

using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;
namespace json = boost::property_tree;

class GToNetCDF
{
public:
    /*!
     * Initialize the GeoFLOW to NetCDF file writer.
     * 
     * @param jsonFilename JSON filename with metadata needed for conversion
     * @param ncFilename name of NetCDF file to write to
     * @param mode read (file exists, open read-only)
     *             write (file exists, open for writing)
     *             replace (create new file, even if it exists)
     *             newFile (create new file, fail if already exists)
     */
    GToNetCDF(const GString& jsonFilename,
             const GString& ncFilename,
             NcFile::FileMode mode);
    ~GToNetCDF() {}

    void writeDimensions();
    void writeVariables(GBOOL doWriteAttributes);
    void writeData(const GString& varName);

private:
    json::ptree _root;   // root of json file that contains NetCDF metadata
    NcFile _nc;          // NetCDF file handle
};

#endif