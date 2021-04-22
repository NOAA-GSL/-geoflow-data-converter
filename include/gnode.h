//==============================================================================
// Date        : 3/23/21 (SG)
// Description : Stores the location and data value of a GeoFLOW node.
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
     * @param elemLayerID ID of layer the GeoFLOW element resides on
     */
    GNode(T x, T y, T z, GSIZET elemLayerID)
    {
        // Initialize
        _pos = {{x, y, z}};
        _elemLayerID = elemLayerID;
    }
    ~GNode() {}

    // Access
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

    // Print
    void printNode()
    {
        // Print position and layer info
        cout << "Node: ";
        cout << "x, y, z: (" << _pos[0] << ", " 
                               << _pos[1] << ", "
                               << _pos[2] << ") | ";
        cout << "elemLayerID: (" << _elemLayerID << ") | ";

        // Print all variables in the node
        typename map<GString, T>::const_iterator it;
        for (it = _varMap.begin(); it != _varMap.end(); ++it)
        {
            cout << it->first << ": (" << it->second << ") | ";
        }
        cout << endl;
    }

private:
    array<T, 3> _pos;        // cartesian coordinate x,y,z of node
    GSIZET _elemLayerID;     // element layer index the node resides on
    map<GString, T> _varMap; // pairs of variable names and their values
};

#endif
