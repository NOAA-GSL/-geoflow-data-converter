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
     * Get an array from a tree in the property tree.
     * 
     * @param tree tree in the property tree
     * @param key name of the array to find
     * @return the array
     */
    static pt::ptree getArray(const pt::ptree& tree, const GString& key)
    {
        try
        {
            return tree.get_child(key);
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error getting array: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Get a reference to an array from a tree in the property tree.
     * 
     * @param tree tree in the property tree
     * @param key name of the array to find
     * @return the array
     */
    static pt::ptree& getArrayRef(pt::ptree& tree, const GString& key)
    {
        try
        {
            return tree.get_child(key);
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error getting reference to array: " + \
                              GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Get the value of a key from the property tree.
     * 
     * @param tree a tree in the property tree
     * @param key name of the key to find
     * @return value of key
     */
    template <typename T>
    static T getValue(const pt::ptree& tree, const GString& key)
    {
        try
        {
            return tree.get<T>(key);
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error getting value: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Get the values in an array in the property tree.
     * 
     * @param arr array in the property tree
     * @return vector of the array values
     */
    template <typename T>
    static vector<T> getValues(const pt::ptree& arr)
    {
        try
        {
            vector<T> values;

            // For each key-value element in the array...
            BOOST_FOREACH (pt::ptree::value_type a , arr)
            {
                // Add the value to the vector
                values.push_back(a.second.data());
            }
            return values;
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error getting values: " + GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }

    /*!
     * Check if a key exists in a tree of the property tree.
     * 
     * @param tree a tree in the property tree
     * @param key name of the key to find
     * @return true if key is found, false otherwise
     */
    static GBOOL findKey(const pt::ptree& tree, const GString& key)
    {
        try
        {
            // For each key-value element in the array...
            BOOST_FOREACH (pt::ptree::value_type t , tree)
            {
                // Check if the key exists
                if (t.first.data() == key)
                {
                    return true;
                }
            }

            return false;
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error looking for key (" + key + "): " + \
                              GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);        
        }
    }

    /*!
     * Set a value for a key in a tree of the property tree. The value gets 
     * overwritten if it exisits.
     * 
     * @param @param tree a sub tree/object in the property tree
     * @param key name of the key
     * @param value value of the key
     */
    template <typename T>
    static void putValue(pt::ptree& tree,
                         const GString& key,
                         const T& value)
    {
        try
        {
            tree.put<T>(key, value);
        }
        catch (const boost::property_tree::ptree_error& e)
        {
            std::string msg = "Error setting value for key (" + key + "): " + \
                              GString(e.what());
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE);
        }
    }
};
                      
#endif