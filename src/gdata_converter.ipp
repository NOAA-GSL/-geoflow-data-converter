//==============================================================================
// Date      : 4/1/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include <fstream>

#include "gfile_reader.h"
#include "g_to_netcdf.h"
#include "math_util.h"
#include "pt_util.h"
#include "logger.h"

template <class T>
GDataConverter<T>::GDataConverter(const GString& ptFilename)
{
    // Initialize
    _ptFilename = ptFilename;

    // Load the property tree
    PTUtil::readJSONFile(_ptFilename, _ptRoot);
}

template <class T>
void GDataConverter<T>::readGFGrid()
{
    // Read the x,y,z GeoFLOW grid filenames from the property tree
    GString x = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.x");
    GString y = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.y");
    GString z = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.z");

    // Read the GeoFLOW x,y,z grid values into a collection of nodes
    readGFGrid(x, y, z);
}

template <class T>
void GDataConverter<T>::readGFGrid(const GString& xFilename,
                                   const GString& yFilename,
                                   const GString& zFilename)
{
    // Read the GeoFLOW x,y,z grid files into a collection of nodes
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
    for (auto i = 0u; i < _header.nNodesPer2DLayer; ++i)
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
void GDataConverter<T>::readGFVariable(const GString& filename)
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
    for (auto i = 0u; i < _header.nNodesPer2DLayer; ++i)
    {
        _nodes[i].var(var.data()[i]);
    }    
}

template <class T>
void GDataConverter<T>::xyzToLatLonRadius()
{
    cout << "For each node, computing lat,lon,radius from x,y,z" << endl;

    // For each node, convert x,y,z to lat,lon,radius...
    for (auto& n : _nodes)
    {
        MathUtil::xyzToLatLonRadius<T>(n);
    } 
}

template <class T>
void GDataConverter<T>::printHeader()
{
    GFileReader<T>::printHeader(_header);
}