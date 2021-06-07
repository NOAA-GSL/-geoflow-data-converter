//==============================================================================
// Date      : 4/1/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>

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

    // Get number of timesteps
    _numTimesteps = PTUtil::getValue<GUINT>(_ptRoot, "num_timesteps");

    // Get the root names of the variables
    pt::ptree varsArr = PTUtil::getArray(_ptRoot, "root_variable_names");
    _rootVarNames = PTUtil::getValues<GString>(varsArr);
    
    // Create full variable names with timestep (i.e., rootVarName.timestep)
    if (_numTimesteps == 0)
    {
        _fullVarNames = _rootVarNames;
    }
    else
    {
        // For each timestep...
        for (auto i = 0u; i < _numTimesteps; ++i)
        {
            // Get timestep as a string
            stringstream ss;
            ss << std::setfill('0') << std::setw(6) << i;
            GString timestep = ss.str();

            // For each variable at this timestep...
            for (auto rootVarName : _rootVarNames)
            {
               _fullVarNames.push_back(rootVarName + "." + timestep);
            }
        }
    }

    // For debugging
    cout << "Input directory is: " << _inputDir << endl;
    cout << "Output directory is: " << _outputDir << endl;
    cout << "Num timestpes are: " << _numTimesteps << endl;
    cout << "Root variable names are: ";
    for (auto f : _rootVarNames)
    {
        cout << f << ", "; 
    }
    cout << endl;
    cout << "Full variable names are: ";
    for (auto f : _fullVarNames)
    {
        cout << f << ", "; 
    }
    cout << endl;
}

template <class T>
GString GDataConverter<T>::extractTimestep(GString varName)
{
    auto npos = varName.find('.');
    if (npos != string::npos)
    {
        return varName.substr(npos + 1);
    }
    else
    {
        std::string msg = "Could not extract timestep from input name: " + \
                          varName;
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);       
    }
}

template <class T>
GString GDataConverter<T>::extractRootVarName(GString varName)
{
    auto npos = varName.find('.');
    if (npos != string::npos)
    {
        return varName.substr(0, npos);
    }
    else
    {
        std::string msg = "Could not extract root variable name from " \
                          "input name: " + varName;
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);       
    }
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
GHeaderInfo GDataConverter<T>::readGFGridToNodes()
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
    GHeaderInfo header = readGFGridToNodes(x, y, z);

    return header;
}

template <class T>
GHeaderInfo GDataConverter<T>::readGFGridToNodes(const GString& gfXFilename,
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
    _nodes.clear();
    _nodes.shrink_to_fit();
    for (auto i = 0u; i < (x.header()).nNodesPerVolume; ++i)
    {
        GNode<T> node(x.data()[i],
                      y.data()[i], 
                      z.data()[i],
                      x.elementLayerIDs()[i]);

        // Save node
        _nodes.push_back(node);
    }

    // Save header
    _header = x.header();

    return x.header();
}

template <class T>
GHeaderInfo GDataConverter<T>::readGFVariableToNodes(const GString& gfFilename,
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
    for (auto i = 0u; i < (var.header()).nNodesPerVolume; ++i)
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
void GDataConverter<T>::sortNodesByElemID()
{
    // Use stable sort to rmake sure the same order of objects is retained for 
    // for two objects with equal keys
    stable_sort(_nodes.begin(), _nodes.end());
}

template <class T>
void GDataConverter<T>::sortNodesBy2DMeshLayer()
{
    // Sort the nodes by 2D mesh layer. For 3D elements, there are multiple 
    // 2D layers (x,y ref dir) in the radial direction

    vector<GNode<T>> temp;
    GUINT nX = _header.polyOrder[0] + 1; // num nodes in x ref dir
    GUINT nY = _header.polyOrder[1] + 1; // num nodes in y ref dir
    GUINT nZ = 1; // default num nodes in z ref dir for a 2D dataset
    if (_header.polyOrder.size() == 3) // 3D dataset
    {
        nZ = _header.polyOrder[2] + 1; // num nodes in z ref dir
    }
    GUINT nXY = nX * nY; // num nodes per element in x,y ref dir
    GUINT nXYZ = nX * nY * nZ; // num nodes per element in x,y,z dir
    GUINT nNodesPerElemLayer = _header.nElemPerElemLayer * nXYZ;

    // For each GeoFLOW element layer...
    for (auto i = 0u; i < _header.nElemLayers; ++i)
    {
        // For each 2D layer (x,y ref dir) in the element
        for (auto k = 0u; k < nZ; ++k)
        {
            // For each element in the GeoFLOW element layer...
            for (auto j = 0u; j < _header.nElemPerElemLayer; ++j)
            {
                typename vector<GNode<T>>::iterator start = 
                    _nodes.begin() + (i * nNodesPerElemLayer) + (j * nXYZ) + (k * nXY);
                typename vector<GNode<T>>::iterator end = start + nXY;
                temp.insert(temp.end(), start, end);
            }
        }
    }

    // Swap the sorted temp nodes with the class member nodes
    _nodes.swap(temp);
}

template <class T>
void GDataConverter<T>::faceToNodes()
{
    // Create a mapping of face to nodes for the first 2D mesh layer. This 
    // mapping is the same for each layer. The assumption here is the nodes 
    // are already sorted in ascending order by 2D mesh layer, each 2D element 
    // (i.e., all the nodes for one element in the x,y ref dir) is grouped 
    // together, and the faces for one 2D element are listed left to right, 
    // top to bottom

    GUINT nX = _header.polyOrder[0] + 1; // num nodes in x ref dir
    GUINT nY = _header.polyOrder[1] + 1; // num nodes in y ref dir
    GUINT nXY = nX * nY; // num nodes per element in x,y ref dir

    _faces.clear();
    _faces.shrink_to_fit();

    // For each 2D element in the first layer...
    for (auto i = 0u; i < _header.nNodesPer2DLayer; i += nXY)
    {
        // Get all the faces in the 2D element. Nodes for a face must be 
        // specified in counter-clockwise direction. Here we use the top-left 
        // node as the starting point for each face

        // For each row of faces in the element...
        for (auto x = 0u; x < nX - 1; ++x)
        {
            // For each column of faces in the element...
            for (auto y = 0u; y < nY - 1; ++y)
            {
                // Get the indices of the face
                vector<GSIZET> indices;
                indices.push_back(i + (x * nY) + y);             // top left
                indices.push_back(i + ((x + 1) * nY) + y);       // bot left
                indices.push_back(i + ((x + 1) * nY) + (y + 1)); // bot right
                indices.push_back(i + (x * nY) + (y + 1));       // top right

                // Add the face to the list
                _faces.push_back(GFace(indices));
            }
        } 
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
void GDataConverter<T>::writeNCNodeVariable(const GString& rootVarName, 
                                            const GString& fullVarName)
{
    // Write the contents of a node variable to the NetCDF file
    _nc->writeVariableDefinition(rootVarName);
    _nc->writeVariableAttributes(rootVarName);
    _nc->writeVariableData<T>(rootVarName, fullVarName, _nodes);
}

template <class T>
template <typename U>
void GDataConverter<T>::writeNCVariable(const GString& varName, 
                                        const U& varValue)
{
    // Write a single valued variable to the NetCDF file
    _nc->writeVariableDefinition(varName);
    _nc->writeVariableAttributes(varName);
    _nc->writeVariableData<U>(varName, varValue);
}

template <class T>
template <typename U>
void GDataConverter<T>::writeNCVariable(const GString& varName, 
                                        const vector<U>& values)
{
    // Write the contents of a vector to the NetCDF file
    _nc->writeVariableDefinition(varName);
    _nc->writeVariableAttributes(varName);
    _nc->writeVariableData<U>(varName, values);
}