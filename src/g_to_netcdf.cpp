//==============================================================================
// Date      : 4/6/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include "g_to_netcdf.h"
#include "logger.h"

GToNetCDF::GToNetCDF(pt::ptree& ptRoot,
                     const GString& ncFilename,
                     NcFile::FileMode mode)
{
    // Initialize
    _ptRoot = ptRoot;

    // Open the NetCDF file
    cout << "Opening NetCDF file for writing: " << ncFilename << endl;
    _nc.open(ncFilename, mode);
}

NcType GToNetCDF::toNcType(const GString& gType)
{
    // Get NetCDF type from GeoFLOW type
    if (gType == "GString")      { return ncString; }
    else if (gType == "GFLOAT")  { return ncFloat; }
    else if (gType == "GDOUBLE") { return ncDouble; }
    else if (gType == "GINT")    { return ncInt; }
    else if (gType == "GUINT")   { return ncUint; }
    else if (gType == "data_type")
    {
        GString t = PTUtil::getValue<GString>(_ptRoot, "data_type");
        return toNcType(t);
    }
    else
    {
        std::string msg = "Unable to convert data type (" + gType + ") " + \
                          "to a NetCDF data type.";
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }
}

void GToNetCDF::putAttribute(const NcVar& ncVar, 
                             const GString& name,
                             const GString& value,
                             const NcType& ncType)
{
    // Write an attribute to the NetCDF file
    if (ncType == ncString)
    {
        ncVar.putAtt(name, value);
    }
    else if (ncType == ncFloat)
    {
        ncVar.putAtt(name, ncType, stof(value));
    }
    else if (ncType == ncDouble)
    {
        ncVar.putAtt(name, ncType, stod(value));
    }
    else if (ncType == ncInt)
    {
        ncVar.putAtt(name, ncType, stoi(value));
    }
    else if (ncType == ncUint)
    {
        ncVar.putAtt(name, ncType, (unsigned int)(stoul(value)));
    }
    else 
    {
        std::string msg = "The input NetCDF data type is not supported.";
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }
}

NcType GToNetCDF::getVariableType(const GString& varName)
{
    NcVar ncVar = _nc.getVar(varName);
    return ncVar.getType();
}

void GToNetCDF::writeDimensions()
{
    cout << "Writing NetCDF dimensions" << endl;

    pt::ptree dimArr = PTUtil::getArray(_ptRoot, "dimensions");

    // For each dimension in the dimensions array...
    for (pt::ptree::iterator it = dimArr.begin(); it != dimArr.end(); ++it)
    {
        // Get the values of the specified keys
        GString name = PTUtil::getValue<GString>(it->second, "name");
        GUINT value = PTUtil::getValue<GUINT>(it->second, "value");

        // For debugging
        cout << "--- [name = " << name << ", value = " << value << "]"
             << endl;
        
        // Write the dimension to the NetCDF file. The dimension gets written 
        // in the form: dimName = dimValue
        _nc.addDim(name, value);
    }
}

void GToNetCDF::writeVariableDefinition(const GString& varName)
{
    cout << "Writing NetCDF variable definition for: " << varName << endl;

    pt::ptree varArr = PTUtil::getArray(_ptRoot, "variables");

    // For each variable in the variables array...
    for (pt::ptree::iterator it = varArr.begin(); it != varArr.end(); ++it)
    {
        // Get the values of the specified key
        GString name = PTUtil::getValue<GString>(it->second, "name");

        // If the input variable is found...
        if (name == varName)
        {
            // Get the values of the specified keys
            GString type = PTUtil::getValue<GString>(it->second, "type");
            pt::ptree argsArr = PTUtil::getArray(it->second, "args");
            vector<GString> args = PTUtil::getValues<GString>(argsArr);

            // Convert the GeoFLOW type to an NcType
            NcType ncType = toNcType(type);

            // Collect the args into a vector of NetCDF dimensions
            vector<NcDim> ncDims;
            for (auto a : args)
            {
               ncDims.push_back(_nc.getDim(a));
            }
        
            // Write the variable definition to the NetCDF file. The 
            // definition gets written in the form: 
            // varType varName(dim1, dim2, ...)
            _nc.addVar(name, ncType, ncDims);

            // For debugging
            cout << "--- [name = " << name << ", type = " << type << ", "
                 << "args = ";
            for (auto a : args)
            {
                cout << a << ",";
            }
            cout << "]" << endl;

            // Found variable - exit loop
            return;
        }
    }

    // Reached error - could not find the input variable name
    std::string msg = "Could not find the variable name (" + varName + ") " \
                      "in the property tree.";
    Logger::error(__FILE__, __FUNCTION__, msg);
    exit(EXIT_FAILURE);
}

void GToNetCDF::writeVariableAttribute(const GString& varName)
{
    cout << "Writing NetCDF variable attributes for: " << varName << endl;

    pt::ptree varArr = PTUtil::getArray(_ptRoot, "variables");

    // For each variable in the variables array...
    for (pt::ptree::iterator itVar = varArr.begin();
         itVar != varArr.end(); 
         ++itVar)
    {
        // Get the values of the specified key
        GString name = PTUtil::getValue<GString>(itVar->second, "name");

        // If the input variable is found...
        if (name == varName)
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
                GString gType = "GString"; // default type for an attribute value

                // If a type key is specified for this attribute, get the value
                if (PTUtil::findKey(itAtt->second, "type"))
                {
                    gType = PTUtil::getValue<GString>(itAtt->second, "type");
                }

                // Get the NetCDF variable whose attributes we are iterating on
                GString varName = PTUtil::getValue<GString>(itVar->second, "name");
                NcVar ncVar = _nc.getVar(varName);

                // Write the variable's attribute to the NetCDF file. The 
                // attribute gets written in the form: 
                // var_name:att_name = att_value
                putAttribute(ncVar, name, value, toNcType(gType));

                // For debugging
                cout << "--- [name = " << name << ", value = " << value
                     << ", gtype = " << gType << "]" << endl;
            }

            // Found variable - exit loop
            return;
        }
    }

    // Reached error - could not find the input variable name
    std::string msg = "Could not find the variable name (" + varName + ") " \
                      "in the property tree.";
    Logger::error(__FILE__, __FUNCTION__, msg);
    exit(EXIT_FAILURE);
}