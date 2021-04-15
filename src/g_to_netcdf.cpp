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
    if (gType == "GFLOAT")       { return ncFloat; }
    else if (gType == "GDOUBLE") { return ncDouble; }
    else if (gType == "GINT")    { return ncInt; }
    else if (gType == "GUINT")   { return ncUint; }
    else if (gType == "GString") { return ncString; }
    else if (gType == "data_type")
    {
        GString t = PTUtil::getValue<GString>(_ptRoot, "data_type");
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

void GToNetCDF::fillDimensions(const map<GString, GSIZET>& dims)
{
    cout << "Setting mesh dimensions in the property tree from GeoFLOW data"
         << endl;

    // Get the dimensions array
    pt::ptree& dimArr = PTUtil::getArrayRef(_ptRoot, "dimensions");

    // For each dimension in the dimensions array...
    for (pt::ptree::iterator it = dimArr.begin(); it != dimArr.end(); ++it)
    {
        // Get the values of the specified keys
        GString dimName = PTUtil::getValue<GString>(it->second, "name");
        GSIZET dimValue = PTUtil::getValue<GSIZET>(it->second, "value");

        // If the value of the dimension in the property tree is 0, the value
        // needs to be set using the value in the input dimensions map
        if (dimValue == 0)
        {
            // Look for the dimension name in the dimensions map
            map<GString, GSIZET>::const_iterator itMap;
            itMap = dims.find(dimName);
            if (itMap != dims.end())
            {
               // Write the value found in the map to the dimension value in
               // the property tree
               PTUtil::putValue<GSIZET>(it->second, "value", dims.at(dimName));
            }
            else {
                std::string msg = "Cannot find dimension (" + dimName + ") " + \
                                  "in the input dimensions.";
                Logger::error(__FILE__, __FUNCTION__, msg);
                exit(EXIT_FAILURE);
            }
        }
    }
}                               

void GToNetCDF::writeDimensions()
{
    cout << "Writing NetCDF dimensions" << endl;

    // Get the dimensions array
    pt::ptree dimArr = PTUtil::getArray(_ptRoot, "dimensions");

    // For each dimension in the dimensions array...
    for (pt::ptree::iterator it = dimArr.begin(); it != dimArr.end(); ++it)
    {
        // Get the values of the specified keys
        GString name = PTUtil::getValue<GString>(it->second, "name");
        GUINT value = PTUtil::getValue<GUINT>(it->second, "value");

        // For debugging
        cout << "dimension [name = " << name << ", value = " << value << "]"
             << endl;
        
        // Write the dimension to the NetCDF file. The dimension gets written
        // in the form: dimName = dimValue
        _nc.addDim(name, value);
    }
}

void GToNetCDF::writeVariables()
{
    cout << "Writing NetCDF variables" << endl;

    // Get the variables array
    pt::ptree varArr = PTUtil::getArray(_ptRoot, "variables");

    // For each variable in the variables array...
    for (pt::ptree::iterator it = varArr.begin(); it != varArr.end(); ++it)
    {
        // Get the values of the specified keys
        GString name = PTUtil::getValue<GString>(it->second, "name");
        GString type = PTUtil::getValue<GString>(it->second, "type");
        pt::ptree argsArr = PTUtil::getArray(it->second, "args");
        vector<GString> args = PTUtil::getValues<GString>(argsArr);

        // For debugging
        cout << "variable [name = " << name << ", type = " << type << ", "
             << "args = ";
        for (auto a : args)
        {
            cout << a << ",";
        }
        cout << endl;

        // Convert the GeoFLOW type to an NcType
        NcType ncType = toNcType(type);

        // Collect the args into a vector of NetCDF dimensions
        vector<NcDim> ncDims;
        for (auto a : args)
        {
            ncDims.push_back(_nc.getDim(a));
        }
        
        // Write the variable definition to the NetCDF file. The definition
        // gets written in the form: varType varName(dim1, dim2, ...)
        _nc.addVar(name, ncType, ncDims);
    }
}

void GToNetCDF::writeAttributes()
{
    cout << "Writing NetCDF variable attributes" << endl;

    // Get the variables array
    pt::ptree varArr = PTUtil::getArray(_ptRoot, "variables");

    // For each variable in the variables array...
    for (pt::ptree::iterator itVar = varArr.begin();
         itVar != varArr.end(); 
         ++itVar)
    {
        // Get the attributes array
        pt::ptree attArr = PTUtil::getArray(itVar->second, "attributes");

        // For each attribute in the attributes array...
        for (pt::ptree::iterator itAtt = attArr.begin();
             itAtt != attArr.end();
             ++itAtt)
        {
            // Get the values of the specified keys
            GString name = PTUtil::getValue<GString>(itAtt->second, "name");
            GString value = PTUtil::getValue<GString>(itAtt->second, "value");

            // For debugging
            cout << "variable [name = " << name << ", type = " << value << "]"
                 << endl;

            // Get the variable whose attributes we are iterating on
            GString varName = PTUtil::getValue<GString>(itVar->second, "name");
            NcVar ncVar = _nc.getVar(varName);

            // Write the variable's attribute to the NetCDF file. The attribute
            // gets written in the form: var_name:att_name = att_value
            ncVar.putAtt(name, value);
        }
    }
}

void GToNetCDF::writeData(const GString& varName)
{

}