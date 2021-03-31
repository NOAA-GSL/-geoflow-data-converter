//==============================================================================
// Date         : 3/17/21 (SG)
// Description  : Driver program for GeoFLOW data converter.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

#include "gfile_reader.h"
#include "gnode.h"
#include "gface.h"
#include "glayer.h"
#include "gvolume.h"
#include "math_util.h"

#define GDATATYPE GDOUBLE
#define NODES_PER_FACE 4

#define FILE_XGRID "data/xgrid.000000.out"
#define FILE_YGRID "data/ygrid.000000.out"
#define FILE_ZGRID "data/zgrid.000000.out"
#define FILE_VAL "data/dtotal.000000.out"

int main()
{
    // Read x,y,z locations and the data value at that location
    GFileReader<GDATATYPE> xgrid(FILE_XGRID);
    GFileReader<GDATATYPE> ygrid(FILE_YGRID);
    GFileReader<GDATATYPE> zgrid(FILE_ZGRID);
    GFileReader<GDATATYPE> val(FILE_VAL);

    // Get header info
    GHeaderInfo h = xgrid.header();
    GFileReader<GDATATYPE>::printHeader(h);

    // Read x,y,z and value into a collection of nodes
    vector<GNode<GDATATYPE>> nodes;
    for (auto i = 0u; i < h.nNodes; ++i)
    {
        GNode<GDATATYPE> node(xgrid.data()[i],
                              ygrid.data()[i], 
                              zgrid.data()[i],
                              val.data()[i],
                              0);

        // Convert x,y,z to lat,lon,radius
        MathUtil::xyzToLatLonRadius<GDATATYPE>(node);

        // Save node
        nodes.push_back(node);
    }

    ////////////////////////////////////////
    // TODO: Sort the nodes into layers etc.
    ////////////////////////////////////////

    // Read nodes into a collection of fake faces
    vector<GFace<GDATATYPE>> faces;
    for (auto i = 0u; i < nodes.size(); i += NODES_PER_FACE)
    {
        GFace<GDATATYPE> face(NODES_PER_FACE);
        face.nodes(nodes, i);
        faces.push_back(face);
    }

    // Read faces into a collection of layers
    vector<GLayer<GDATATYPE>> layers;
    GLayer<GDATATYPE> layer;
    layer.faces(faces);
    layers.push_back(layer);
    
    // Read layers into a volume.
    GVolume<GDATATYPE> volume;
    volume.layers(layers);

    // Print volume
    volume.printVolume();

    // Write UGRID file using data stored in volume

    return 0;
}
