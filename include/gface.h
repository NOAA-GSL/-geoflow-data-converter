//==============================================================================
// Module       : gface.h
// Date         : 3/24/21 (SG)
// Description  : Stores a polygon made up of a set of GeoFLOW nodes
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
// Derived From : none
//==============================================================================

#ifndef GFACE_H
#define GFACE_H

#include <iostream>
#include <vector>

#include "gnode.h"
#include "logger.h"

using namespace std;

template <class T>
class GFace
{
public:
    GFace(GUINT nNodesPerFace) { _nNodesPerFace = nNodesPerFace; }
    ~GFace() {}

    // Access
    vector<GNode<T>> nodes() const { return _nodes; }
    //void nodes(const vector<GNode<T>>& nodes) { _nodes = nodes; }
    void nodes(const vector<GNode<T>>& nodes, GUINT first)
    {
        // Get last node index
        GUINT last = first + _nNodesPerFace - 1;
        if (first > (nodes.size() - 1u) || last > (nodes.size() - 1u))
        {
            string msg = "Node access (" + to_string(first) + " to " + \
                         to_string(last) + ") exceeds count of incoming " \
                         "nodes (" + to_string(nodes.size()) + ").";
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }

        // Copy specified interval of nodes
        typename vector<GNode<T>>::const_iterator it = nodes.begin() + first;
        copy(it, it + _nNodesPerFace, back_inserter(_nodes));

        // Copy specified interval of node indices
        for (auto i = first; i <= last; ++i)
        {
            _indices.push_back(i);
        }
    }

    // Print
    void printFaceNodes()
    {
        cout << "Face nodes" << endl;
        for (auto n : _nodes)
        {
            n.printNode();
        }
    }

    void printFaceIndices()
    {
        cout << "Face indices: ";
        for (auto i : _indices)
        {
            cout << i << ", ";
        }
        cout << endl;
    }

private:
    vector<GNode<T>> _nodes;
    vector<GINT> _indices;
    GUINT _nNodesPerFace;
};

#endif