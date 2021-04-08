//==============================================================================
// Date      : 4/6/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include "g_to_netcdf.h"
#include "pt_util.h"
#include "logger.h"

GToNetCDF::GToNetCDF(const GString& ptFilename,
                     const GString& ncFilename,
                     NcFile::FileMode mode)
{
    // Read and load the property tree file
    PTUtil::readJSONFile(ptFilename, _ptRoot);

    // Open the NetCDF file
    // TODO: Add error checking.
    cout << "Opening NetCDF file for writing: " << ncFilename << endl;
    _nc.open(ncFilename, mode);
}

NcType GToNetCDF::toNcType(const GString& gType)
{
    // TODO
    if (gType == "GFLOAT")       { return ncFloat; }
    else if (gType == "GDOUBLE") { return ncDouble; }
    else if (gType == "GINT")    { return ncInt; }
    else if (gType == "GUINT")   { return ncUint; }
    //else if (gType == "GSIZET")  { return ncUint64; }
    else if (gType == "GString") { return ncString; }
    else if (gType == "data_type")
    {
        GString t = PTUtil::readProperty<GString>(_ptRoot, "data_type");
        return toNcType(t);
    }
    else
    {
        std::string msg = "Unable to convert data type (" + gType + ") " + \
                          "read from property tree to a NetCDF data type.";
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }
}

void GToNetCDF::writeDimensions()
{
    cout << "Writing NetCDF dimensions" << endl;

    pt::ptree dimTree = PTUtil::readSubtree(_ptRoot, "dimensions");

    // For each object in the dimensions subtree...
    BOOST_FOREACH(pt::ptree::value_type obj, dimTree)
    {
        GString name = PTUtil::readProperty<GString>(obj, "name");
        GUINT value = PTUtil::readProperty<GUINT>(obj, "value");

        cout << "dimension [name = " << name << ", value = " << value << "]"
             << endl;
        
        // Write the dimension to the NetCDF file
        _nc.addDim(name, value);
    }
}

void GToNetCDF::writeVariables(GBOOL doWriteAttributes)
{
    cout << "Writing NetCDF variables" << endl;

    pt::ptree varTree = PTUtil::readSubtree(_ptRoot, "variables");

    // For each object in the variables subtree...
    BOOST_FOREACH(pt::ptree::value_type obj, varTree)
    {
        GString name = PTUtil::readProperty<GString>(obj, "name");
        GString type = PTUtil::readProperty<GString>(obj, "type");

        cout << "variable [name = " << name << ", type = " << type << "]"
             << endl;
        
        // Convert the type to an NcType
        //NcType ncType = toNcType(type);

        // Write the variable to the NetCDF file
    }
}

void GToNetCDF::writeData(const GString& varName)
{

}