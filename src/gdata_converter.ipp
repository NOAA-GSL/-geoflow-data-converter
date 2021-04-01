//==============================================================================
// Date      : 4/1/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include <fstream>

#include "gfile_reader.h"
#include "math_util.h"
#include "logger.h"

template <class T>
GDataConverter<T>::GDataConverter(const GString& xFilename,
                                  const GString& yFilename,
                                  const GString& zFilename)
{
    // Read GeoFLOW x,y,z files
    GFileReader<T> xgrid(xFilename);
    GFileReader<T> ygrid(yFilename);
    GFileReader<T> zgrid(zFilename);

    // Get header info. The header is the same for each x,y,z file so just
    // use the xgrid header.
    _header = xgrid.header();
    GFileReader<T>::printHeader(_header);

    // Read location (x,y,z) and location's element layer ID into a collection
    // of nodes. The IDs are the same for each x,y,z data value so just use the
    // IDs from xgrid.
    vector<GNode<T>> nodes;
    for (auto i = 0u; i < _header.nNodes; ++i)
    {
        GNode<T> node(xgrid.data()[i],
                      ygrid.data()[i], 
                      zgrid.data()[i],
                      xgrid.elementLayerIDs()[i]);

        // Convert x,y,z to lat,lon,radius
        MathUtil::xyzToLatLonRadius<T>(node);

        // Save node
        _nodes.push_back(node);
    }
}

template <class T>
void GDataConverter<T>::readVariable(const GString& filename)
{
    // Read GeoFLOW variable file
    GFileReader<T> var(filename);

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
