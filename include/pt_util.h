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
     * Read a JSON file into a property tree.
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
     * Get an array from the property tree.
     * 
     * @param root root of the property tree
     * @param key name of the array key to find
     * @return the array
     */
    static pt::ptree getArray(const pt::ptree& root, const GString& key)
    {
        try
        {
            pt::ptree arr = root.get_child(key);
            return arr;
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error reading array: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Get the value of a key from the property tree.
     * 
     * @param root root of the property tree
     * @param key name of the key to find
     * @return value of key
     */
    template <typename T>
    static T getValue(const pt::ptree& root, const GString& key)
    {
        try
        {
            T value = root.get<T>(key);
            return value;
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error reading key: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Get the value of a key from an object in an array of the property tree.
     * 
     * @param obj object in array of a property tree
     * @param key name of the key to find
     * @return value of key
     */
    template <typename T>
    static T getValue(const pt::ptree::value_type& obj, const GString& key)
    {
        try
        {
            T value = (obj.second).get<T>(key);
            return value;
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error reading key: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Get the values in an array in the property tree.
     * 
     * @param arr array in the property tree
     * @return vector of values in the array
     */
    template <typename T>
    static vector<T> getValues(const pt::ptree& arr)
    {
        try
        {
            vector<T> values;

            // For each value in the array...
            BOOST_FOREACH (pt::ptree::value_type a , arr)
            {
                // Add the value to the vector
                values.push_back(a.second.data());
            }
            return values;
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error reading values: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }
};
                      
#endif