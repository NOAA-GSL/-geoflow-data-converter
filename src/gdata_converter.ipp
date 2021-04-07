//==============================================================================
// Date      : 4/1/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include <fstream>

#include "gfile_reader.h"
#include "g_to_netcdf.h"
#include "math_util.h"
#include "logger.h"

template <class T>
GDataConverter<T>::GDataConverter(const GString& filename)
{
    // Initialize
    _jsonFilename = filename;

    // Load the json file
    try
    {
        cout << "Reading JSON file: " << filename << endl;
        json::read_json(filename, _root);
    }
    catch (const boost::property_tree::json_parser_error& e)
    {
        std::string msg = "JSON file error: " + GString(e.what());
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }
}

template <class T>
void GDataConverter<T>::readGrid()
{
    // Read the x,y,z GeoFLOW grid files specified in the json file
    try
    {
        string x = _root.get<string>("grid_filenames.x"); 
        string y = _root.get<string>("grid_filenames.y"); 
        string z = _root.get<string>("grid_filenames.z");

        readGrid(x, y, z);
    }
    catch(const boost::property_tree::ptree_bad_path& e)
    {
        std::string msg = "JSON file error: " + GString(e.what());
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }
}

template <class T>
void GDataConverter<T>::readGrid(const GString& xFilename,
                                 const GString& yFilename,
                                 const GString& zFilename)
{
    // Read GeoFLOW x,y,z grid files
    GFileReader<T> x(xFilename);
    GFileReader<T> y(yFilename);
    GFileReader<T> z(zFilename);

    // Verify data size
    if (!(x.data().size() == y.data().size() && 
          y.data().size() == z.data().size()))
    {
        string msg = "The number of values in the x grid (" + \
                     to_string(x.data().size()) + "), y grid (" + \
                     to_string(y.data().size()) + ") and z grid (" + \
                     to_string(z.data().size()) + ") differ.";

        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // Get header info. The header is the same for each x,y,z file so just
    // use the x grid header.
    _header = x.header();

    // Read each x,y,z location value and element layer ID into a collection of
    // nodes. The IDs are the same for each x,y,z triplet so just use the
    // IDs from the x grid.
    vector<GNode<T>> nodes;
    for (auto i = 0u; i < _header.nNodes; ++i)
    {
        GNode<T> node(x.data()[i],
                      y.data()[i], 
                      z.data()[i],
                      x.elementLayerIDs()[i]);

        // Save node
        _nodes.push_back(node);
    }
}

template <class T>
void GDataConverter<T>::readVariable(const GString& filename)
{
    // Read a GeoFLOW variable file
    GFileReader<T> var(filename);

    // Verify data size
    if (var.data().size() != _nodes.size())
    {
        string msg = "The size of " + filename + " data (" + \
                     to_string(var.data().size()) + ") is different than " \
                     "the size of nodes (" + to_string(_nodes.size()) + ")";
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // Store variable data into nodes
    for (auto i = 0u; i < _header.nNodes; ++i)
    {
        _nodes[i].var(var.data()[i]);
    }    
}

template <class T>
void GDataConverter<T>::xyzToLatLonRadius()
{
    cout << "For each node, computing lat,lon,radius from x,y,z" << endl;

    for (auto& n : _nodes)
    {
        // Convert x,y,z to lat,lon,radius
        MathUtil::xyzToLatLonRadius<T>(n);
    } 
}

template <class T>
void GDataConverter<T>::printHeader()
{
    GFileReader<T>::printHeader(_header);
}