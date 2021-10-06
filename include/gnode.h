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
     * @param lat latitude coordinate value
     * @param lonVarName name of longitude variable in property tree
     * @param lon longitude coordinate value
     * @param radVarName name of radius variable in property tree
     * @param rad radius coordinate value
     * @param nodeID original node ID when GF file was read in
     * @param elemLayerID element layer index the node resides on
     */
    GNode(const GString& latVarName, const T& lat, 
          const GString& lonVarName, const T& lon, 
          const GString& radVarName, const T& rad,
          GSIZET nodeID, GSIZET elemLayerID) 
       {
        _varMap[latVarName] = lat;
        _varMap[lonVarName] = lon;
        _varMap[radVarName] = rad;
        //_nodeID(nodeID), 
        _elemLayerID = elemLayerID;
    }
    ~GNode() {}

    // Access
    //GSIZET nodeID() const { return _nodeID; }
    GSIZET elemLayerID() const { return _elemLayerID; }
    void elemLayerID(GSIZET id) { _elemLayerID = id; }
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
        // Print ID and layer info
        //cout << "Node: [" << _nodeID << "] - ";
        cout << "elem ID: (" << _elemLayerID << ") | ";

        // Print all variables in the node
        typename map<GString, T>::const_iterator it; 
        for (it = _varMap.begin(); it != _varMap.end(); ++it)
        {
            cout << it->first << ": (" << it->second << ") | ";
        }
        cout << endl;
    }

private:
    //GSIZET _nodeID;          // original GF node ID
    GSIZET _elemLayerID;     // GeoFLOW element layer # node resides on
    map<GString, T> _varMap; // pairs of variable names and their values
};

#endif
