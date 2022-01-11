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
     * @param numVars total number of variables (grid and field) to store in 
     *                the variable list 
     * @param latVarIndex index of lat variable in variable list
     * @param lat latitude coordinate value
     * @param lonVarIndex index of lon variable in variable list
     * @param lon longitude coordinate value
     * @param radVarIndex index of rad variable in variable list
     * @param rad radius coordinate value
     * @param elemLayerID GeoFLOW element layer index the node resides on
     */
    GNode(GUINT numVars,
          GUINT latVarIndex, const T& lat, 
          GUINT lonVarIndex, const T& lon, 
          GUINT radVarIndex, const T& rad,
          GSIZET elemLayerID) 
       {
           // Allocate storage for the variable list
           try
           {
               _varList.resize(numVars);
           }
           catch (const std::bad_alloc& e) 
           {
               std::string msg = "Error setting capacity (" + 
                                 to_string(numVars) + ") for list of node " \
                                 "variables.";
               Logger::error(__FILE__, __FUNCTION__, msg);
               cerr << e.what() << endl;
               exit(EXIT_FAILURE);
           }

           // Initalize the grid variables
           try
           {
               _varList.at(latVarIndex) = lat;
               _varList.at(lonVarIndex) = lon;
               _varList.at(radVarIndex) = rad;
           }
           catch(const std::out_of_range& e)
           {
               std::string msg = "Invalid index access into the node's " \
                                 "variable list.";
               Logger::error(__FILE__, __FUNCTION__, msg);
               cerr << e.what() << endl;
               exit(EXIT_FAILURE);
           }

           // Set the element ID
           _elemLayerID = elemLayerID;
    }

    ~GNode() {}

    // Access
    GUINT sortKey() const { return _sortKey; }
    void sortKey(GUINT key) { _sortKey = key; }
    GSIZET elemLayerID() const { return _elemLayerID; }
    void elemLayerID(GSIZET id) { _elemLayerID = id; }

    /*!
     * Get value of input variable from the variable list.
     * 
     * @param varIndex index in variable list the variable is stored at
     * @return value of variable
     */
    T var(GUINT varIndex) const
    {
        try
        {
            return _varList.at(varIndex);
        }
        catch(const std::out_of_range& e)
        {
            std::string msg = "Invalid index access into the node's " \
                              "variable list.";
            Logger::error(__FILE__, __FUNCTION__, msg);
            cout << e.what() << endl;
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Set value of input variable in the variable list
     * 
     * @param varIndex index in variable list the variable is stored at
     * @param value value of variable to set
     */
    void var(GUINT varIndex, const T& value)
    {
        try
        {
            _varList.at(varIndex) = value;
        }
        catch(const std::exception& e)
        {
            std::string msg = "Invalid index access into the node's " \
                              "variable list.";
            Logger::error(__FILE__, __FUNCTION__, msg);
            cout << e.what() << endl;
            exit(EXIT_FAILURE);
        }
    }

    /*
     * Comparison function used for sorting nodes by GeoFLOW element ID 
     * (bottom to top) based on the nodes' assigned element layer IDs.
     * 
     * @param node node to compare against
     * @return true if node's element layer id is less than input node's ID
     */
    bool operator < (const GNode<T>& node) const
    {
        return (_elemLayerID < node.elemLayerID());
    }

    /*
     * Comparison function used for sorting nodes by 2D mesh layers (bottom to 
     * top) based on the nodes' assigned sort keys.
     * 
     * @param a first node's sort key to compare
     * @param b second node's sort key to compare
     * @return true if node a's sort key is less than node b's key
     */
    static bool sort_key_comp(GNode a, GNode b)
    {
        return (a._sortKey < b._sortKey);
    }

    // Print node values
    void printNode(const vector<GString>& varNames)
    {
        // Print ID and layer info
        cout << "sortID: (" << _sortKey << ") | ";
        cout << "eID: (" << _elemLayerID << ") | ";

        // Print all variables in the node with variable name
        for (auto i = 0u; i < _varList.size(); ++i)
        {
            cout << varNames[i] << ": (" << _varList[i] << ") | ";
        }
        cout << endl;
    }

private:
    vector<T> _varList;  // list of grid and field variable values
    GSIZET _elemLayerID; // GeoFLOW element layer # the node resides on
    GUINT _sortKey;      // original 2D elem (x,y ref dir) position the node 
                         // belongs to in the GeoFLOW file
};

#endif
