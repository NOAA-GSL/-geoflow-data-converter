//==============================================================================
// Date        : 4/8/21 (SG)
// Description : Property tree helper class.
// Copyright   : Copyright 2021. Regents of the University of Colorado.
//               All rights reserved.
//==============================================================================

#ifndef PTUTIL_H
#define PTUTIL_H

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include "gtypes.h"
#include "logger.h"

using namespace std;
namespace pt = boost::property_tree;

class PTUtil
{
public:
    PTUtil() {}
    ~PTUtil() {}

     /*!
     * Read a JSON file into a propery tree.
     * 
     * @param filename name of JSON file
     * @param root the property tree to populate
     */
    static void readJSONFile(const GString& filename, pt::ptree& root)
    {
        try
        {
            cout << "Reading JSON file: " << filename << endl;
            pt::read_json(filename, root);
        }
        catch (const boost::property_tree::json_parser_error& e)
        {
            std::string msg = "Error reading JSON file: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }
    /*!
     * Read a subtree from the property tree.
     * 
     * @param root root of the property tree
     * @param subName name of the subtree to get
     * @return the root of the subtree
     */
    static pt::ptree readSubtree(const pt::ptree& root, const GString& subName)
    {
        try
        {
            pt::ptree subtree = root.get_child(subName);
            return subtree;
        }
        catch(const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error reading property: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Read a property from the property tree.
     * 
     * @param root root of the property tree
     * @param property the property to get
     * @return value of property
     */
    template <typename T>
    static T readProperty(const pt::ptree& root, const GString& property)
    {
        try
        {
            T value = root.get<T>(property);
            return value;
        }
        catch(const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error reading property: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Read a property from the property tree.
     * 
     * @param obj object in a property tree
     * @param property the property to get
     * @return value of the property
     */
    template <typename T>
    static T readProperty(pt::ptree::value_type obj, const GString& property)
    {
        try
        {
            T value = (obj.second).get<T>(property);
            return value;
        }
        catch(const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error reading property: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }
};
                      
#endif