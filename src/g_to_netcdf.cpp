//==============================================================================
// Date      : 4/6/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include "g_to_netcdf.h"
#include "logger.h"

GToNetCDF::GToNetCDF(const GString& jsonFilename,
                     const GString& ncFilename,
                     NcFile::FileMode mode)
{
    // Open and read the json file
    try
    {
        cout << "Reading JSON file: " << jsonFilename << endl;

        json::read_json(jsonFilename, _root);
    }
    catch (const boost::property_tree::json_parser_error& e)
    {
        std::string msg = "JSON file error: " + GString(e.what());
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // Open the NetCDF file
    // TODO: Add error checking.
    cout << "Opening NetCDF file for writing: " << ncFilename << endl;
    _nc.open(ncFilename, mode);
}

void GToNetCDF::writeDimensions()
{
    try
    {
        cout << "Writing NetCDF dimensions" << endl;

        auto d = _root.get_child("dimensions");

        // For each object in the dimensions array...
        for (auto it = d.begin(); it != d.end(); ++it)
        {
            // Get the name and value of the dimension
            GString dName = (it->second).get<GString>("name");
            GUINT dValue = (it->second).get<GUINT>("value");
            cout << "dim name is: " << dName << ", dim value is: " << dValue
                 << endl;
            
            // Write the dimension to the NetCDF file
            _nc.addDim(dName, dValue);
        }
    }
    catch(const boost::property_tree::ptree_bad_path& e)
    {
        std::string msg = "JSON file error: " + GString(e.what());
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }
}

void GToNetCDF::writeVariables(GBOOL doWriteAttributes)
{
    try
    {
        cout << "Writing NetCDF variables" << endl;

        auto d = _root.get_child("variables");

        // For each object in the variables array...
        for (auto it = d.begin(); it != d.end(); ++it)
        {
            // Get the name, type, and args of the variable
            GString vName = (it->second).get<GString>("name");
            GString vType = (it->second).get<GString>("type");
            //vector<GString> vArgs = ;
            cout << "var name is: " << vName << ", var type is: " << vType
                 << endl;
        }
    }
    catch(const boost::property_tree::ptree_bad_path& e)
    {
        std::string msg = "JSON file error: " + GString(e.what());
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }
}

void GToNetCDF::writeData(const GString& varName)
{

}