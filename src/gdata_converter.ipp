//==============================================================================
// Date      : 4/1/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

#include "gfile_reader.h"
#include "math_util.h"
#include "logger.h"

template <class T>
GDataConverter<T>::GDataConverter(const GString& ptFilename)
{
    // Initialize
    _ptFilename = ptFilename;
    _nc = 0;

    // Load the property tree
    PTUtil::readJSONFile(_ptFilename, _ptRoot);

    // Get input and output directories
    _inputDir = PTUtil::getValue<GString>(_ptRoot, "input_dir");
    _outputDir = PTUtil::getValue<GString>(_ptRoot, "output_dir");
    makeDirectory(_outputDir);

    // Get variable names
    pt::ptree varsArr = PTUtil::getArray(_ptRoot, "variable_names");
    _varNames = PTUtil::getValues<GString>(varsArr);

    // Get number of timesteps
    _numTimesteps = PTUtil::getValue<GUINT>(_ptRoot, "num_timesteps");

    // For debugging
    cout << "Input directory is: " << _inputDir << endl;
    cout << "Output directory is: " << _outputDir << endl;
    cout << "Num timestpes are: " << _numTimesteps << endl;
    cout << "Variable names are: ";
    for (auto f : _varNames)
    {
        cout << f << ", "; 
    }
    cout << endl;
}

template <class T>
void GDataConverter<T>::makeDirectory(const GString& dirName)
{
    if (mkdir(dirName.c_str(), 0777) != 0 && errno != EEXIST)
    {
        std::string msg = "Cannot create directory (" + \
                          dirName + "): " + strerror(errno);
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }
}

template <class T>
bool GDataConverter<T>::fileExists(const GString& filename)
{
    ifstream ifs(filename);
    if (!ifs.good())
    {
        return false;
    }
    return true;
}

template <class T>
GDataConverter<T>::~GDataConverter()
{
    // Clean memory
    closeNC();
}

template <class T>
GHeaderInfo GDataConverter<T>::readGFGrid()
{
    cout << "Reading GeoFLOW grid files" << endl;

    // Read the x,y,z GeoFLOW grid filenames from the property tree
    GString x = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.x");
    GString y = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.y");
    GString z = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.z");

    x = _inputDir + "/" + x;
    y = _inputDir + "/" + y;
    z = _inputDir + "/" + z;

    // Read the GeoFLOW x,y,z grid values into a collection of nodes
    GHeaderInfo header = readGFGrid(x, y, z);

    return header;
}

template <class T>
GHeaderInfo GDataConverter<T>::readGFGrid(const GString& gfXFilename,
                                          const GString& gfYFilename,
                                          const GString& gfZFilename)
{
    // Read the GeoFLOW x,y,z grid files (header and data)
    GFileReader<T> x(gfXFilename);
    GFileReader<T> y(gfYFilename);
    GFileReader<T> z(gfZFilename);

    // Verify data size
    if (!(x.data().size() == y.data().size() && 
          y.data().size() == z.data().size()))
    {
        string msg = "The number of values in the x grid (" + \
                     to_string(x.data().size()) + "), y grid (" + \
                     to_string(y.data().size()) + ") and z grid (" + \
                     to_string(z.data().size()) + ") differ.";

        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // Read each x,y,z location value and element layer ID into a collection 
    // of nodes. The IDs/header are the same for each x,y,z triplet so just 
    // use the IDs/header from the x grid.
    vector<GNode<T>> nodes;
    for (auto i = 0u; i < (x.header()).nNodesPer2DLayer; ++i)
    {
        GNode<T> node(x.data()[i],
                      y.data()[i], 
                      z.data()[i],
                      x.elementLayerIDs()[i]);

        // Save node
        _nodes.push_back(node);
    }

    return x.header();
}

template <class T>
GHeaderInfo GDataConverter<T>::readGFNodeVariable(const GString& gfFilename,
                                                  const GString& varName)
{
    // Get full output path
    GString filename = _inputDir + "/" + gfFilename;

    // Read a GeoFLOW file
    GFileReader<T> var(filename);

    // Verify data size
    if (var.data().size() != _nodes.size())
    {
        string msg = "The size of " + filename + " data (" + \
                     to_string(var.data().size()) + ") is different than " \
                     "the size of nodes (" + to_string(_nodes.size()) + ")";
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // Store variable data into nodes
    for (auto i = 0u; i < (var.header()).nNodesPer2DLayer; ++i)
    {
        _nodes[i].var(varName, var.data()[i]);
    } 

    return var.header();  
}

template <class T>
void GDataConverter<T>::xyzToLatLonRadius(const GString& latVarName,
                                          const GString& lonVarName,
                                          const GString& radVarName)
{
    cout << "For each node, computing lat,lon,radius from x,y,z" << endl;

    // For each node, convert x,y,z to lat,lon,radius...
    for (auto& n : _nodes)
    {
        array<T, 3> p = MathUtil::xyzToLatLonRadius<T>(n.pos());
        n.var(latVarName, p[0]);
        n.var(lonVarName, p[1]);
        n.var(radVarName, p[2]);
    } 
}

template <class T>
void GDataConverter<T>::setDimensions(const map<GString, GSIZET>& dims)
{
    cout << "Setting mesh dimensions in the property tree from GeoFLOW data" 
         << endl;

    pt::ptree& dimArr = PTUtil::getArrayRef(_ptRoot, "dimensions");

    // For each dimension in the dimensions array...
    for (pt::ptree::iterator it = dimArr.begin(); it != dimArr.end(); ++it)
    {
        // Get the values of the specified keys
        GString dimName = PTUtil::getValue<GString>(it->second, "name");
        GSIZET dimValue = PTUtil::getValue<GSIZET>(it->second, "value");

        // If the value of the dimension in the property tree is 0, the value 
        // needs to be set using the value in the input dimensions map
        if (dimValue == 0)
        {
            // Look for the dimension name in the input dimensions map
            map<GString, GSIZET>::const_iterator itMap;
            itMap = dims.find(dimName);
            if (itMap != dims.end())
            {
               // Write the value found in the map to the dimension value in 
               // the property tree
               PTUtil::putValue<GSIZET>(it->second, "value", dims.at(dimName));
            }
            else {
                std::string msg = "Could not find dimension (" + dimName + \
                                  ") in the property tree.";
                Logger::error(__FILE__, __FUNCTION__, msg);
                exit(EXIT_FAILURE);
            }
        }
    }
} 

template <class T>
void GDataConverter<T>::initNC(const GString& ncFilename,
                               NcFile::FileMode mode)
{
    // Clean memory
    closeNC();

    // Get full output path
    GString filename = _outputDir + "/" + ncFilename;

    // Initialize a GToNetCDF object
    _nc = new GToNetCDF(_ptRoot, filename, mode);
}

template <class T>
void GDataConverter<T>::closeNC()
{
    // Clean memory
    if (_nc != 0)
    {
        delete _nc;
        _nc = 0;
    }
}

template <class T>
void GDataConverter<T>::writeNCDimensions()
{
    // Write the dataset dimensions to the NetCDF file
    _nc->writeDimensions();
}

template <class T>
void GDataConverter<T>::writeNCNodeVariable(const GString& varName)
{
    // Write variable definition
    _nc->writeVariableDefinition(varName);

    // Write variable attributes
    _nc->writeVariableAttribute(varName);

    // Write variable data
    _nc->writeVariableData<T>(varName, _nodes);
}

template <class T>
template <typename U>
void GDataConverter<T>::writeNCVariable(const GString& varName, 
                                        const U& varValue)
{
    // Write variable definition
    _nc->writeVariableDefinition(varName);

    // Write variable attributes
    _nc->writeVariableAttribute(varName);

    // Write variable data
    _nc->writeVariableData<U>(varName, varValue);
}