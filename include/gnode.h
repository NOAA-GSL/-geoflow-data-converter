//==============================================================================
// Date        : 3/23/21 (SG)
// Description : Stores the location and variable data of a GeoFLOW node.
// Copyright   : Copyright 2021. Regents of the University of Colorado.
//               All rights reserved.
//==============================================================================

#ifndef GNODE_H
#define GNODE_H

#include <iostream>
#include <vector>
#include <array>
#include <map>

#include "gtypes.h"
#include "logger.h"

using namespace std;

template <class T>
class GNode
{
public:
     /*!
     * Constructor for initializeing a GeoFLOW node.
     * 
     * @param x cartesian x coordinate
     * @param y cartesian y coordinate
     * @param z cartesian z coordinate
     * @param nodeID original node ID when GF file was read in
     * @param elemLayerID element layer index the node resides on
     */
    GNode(T x, T y, T z, GSIZET nodeID, GSIZET elemLayerID)
    {
        // Initialize
        _pos = {{x, y, z}};
        _nodeID = nodeID;
        _elemLayerID = elemLayerID;
    }
    ~GNode() {}

    // Access
    GSIZET nodeID() const { return _nodeID; }
    GSIZET elemLayerID() const { return _elemLayerID; }
    void elemLayerID(GSIZET id) { _elemLayerID = id; }
    array<T, 3> pos() const { return _pos; }
    void pos(const array<T, 3>& pos) { _pos = pos; }
    T var(const GString& varName) const
    {
        try
        {
            return _varMap.at(varName);
        }
        catch(const std::out_of_range& e)
        {
            std::string msg = "Could not find the variable (" + varName + \
                              ") in the node's variable list.";
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }
    void var(const GString& varName, const T& value)
    {
        _varMap[varName] = value;
    }

    /*
     *
     */
    bool operator < (const GNode<T>& node) const
    {
        return (_elemLayerID < node.elemLayerID());
    }

    // Print
    void printNode()
    {
        // Print position and layer info
        cout << "Node: [" << _nodeID << "] - ";
        cout << "x,y,z: (" << _pos[0] << ", " << _pos[1] << ", " << _pos[2] 
             << ") | " << "elem ID: (" << _elemLayerID << ") | ";

        // Print all variables in the node
        typename map<GString, T>::const_iterator it; 
        for (it = _varMap.begin(); it != _varMap.end(); ++it)
        {
            cout << it->first << ": (" << it->second << ") | ";
        }
        cout << endl;
    }

private:
    GSIZET _nodeID;          // original GF node ID
    array<T, 3> _pos;        // x,y,z cartesian coordinate of node
    GSIZET _elemLayerID;     // GeoFLOW element layer # node resides on
    map<GString, T> _varMap; // pairs of variable names and their values

};

#endif
