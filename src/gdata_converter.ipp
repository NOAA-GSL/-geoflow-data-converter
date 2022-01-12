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
#include "timer.h"

template <class T>
GDataConverter<T>::GDataConverter(const GString& ptFilename)
{
    Logger::info(__FILE__, __FUNCTION__, "");

    // Initialize
    _ptFilename = ptFilename;
    _nc = 0;

    // Load the property tree
    PTUtil::readJSONFile(_ptFilename, _ptRoot);

    // Get directory names and create output directory
    _inputDir = PTUtil::getValue<GString>(_ptRoot, "input_dir");
    _outputDir = PTUtil::getValue<GString>(_ptRoot, "output_dir");
    makeDirectory(_outputDir);
    cout << "Input directory is: " << _inputDir << endl;
    cout << "Output directory is: " << _outputDir << endl;    

    // Get number of timesteps
    _numTimesteps = PTUtil::getValue<GUINT>(_ptRoot, "num_timesteps");
    cout << "Num timestpes are: " << _numTimesteps << endl;

    // Get all variable names
    readVariableNames();
}

template <class T>
void GDataConverter<T>::readVariableNames()
{
    Logger::info(__FILE__, __FUNCTION__, "");

    // Get the names of the grid variables
    pt::ptree gridArr = PTUtil::getArray(_ptRoot, "grid_variable_names");
    vector<GString> gridVarNames = PTUtil::getValues<GString>(gridArr);

    // Get the names of the field variables
    pt::ptree varsArr = PTUtil::getArray(_ptRoot, "field_variable_root_names");
    vector<GString> fieldVarNames = PTUtil::getValues<GString>(varsArr);
    
    // Create field variable names with timestep appended 
    // (i.e., rootName.timestep)
    if (_numTimesteps == 0)
    {
        _fieldVarNames = fieldVarNames;
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
            for (auto rootVarName : fieldVarNames)
            {
               _fieldVarNames.push_back(rootVarName + "." + timestep);
            }
        }
    }

    // Create a single list of grid and (root) field variable names
    _allVarNames = gridVarNames;
    _allVarNames.insert(std::end(_allVarNames), 
                        std::begin(_fieldVarNames), 
                        std::end(_fieldVarNames));

    // For debugging
    cout << "All variable names (grid and field) are: ";
    for (auto n : _allVarNames) { cout << n << ", "; }
    cout << endl;

    cout << "Timestepped field variable names are: ";
    for (auto n : _fieldVarNames) { cout << n << ", "; }
    cout << endl;

    cout << "Verifying nc vars corresponding to grid and field variables "
         << "exist in the property tree." << endl;

    vector<GString> rootVarNames = gridVarNames;
    rootVarNames.insert(std::end(rootVarNames), 
                        std::begin(fieldVarNames), 
                        std::end(fieldVarNames));
    pt::ptree varArr = PTUtil::getArray(_ptRoot, "variables");

    // For each variable in the root var names list...
    for (auto var : rootVarNames)
    {
        GBOOL found = false;
        // For each variable in the property tree variables array...
        for (pt::ptree::iterator it = varArr.begin(); it != varArr.end(); ++it)
        {
            // Get the value of the specified key
            GString name = PTUtil::getValue<GString>(it->second, "name");

            // If the input variable is found...
            if (name == var)
            {
                cout << "Found variable: " << name << endl;
                found = true;
                continue;
            }
        }
        
        if (found == false)
        {
            std::string msg = "Could not find root variable (" + var + \
                              ") in property tree: " + _ptFilename;
            Logger::error(__FILE__, __FUNCTION__, msg);
            exit(EXIT_FAILURE); 
        }
    }
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
GUINT GDataConverter<T>::toVarIndex(const GString& varName)
{
    // Find the index of the input variable name
    auto it = std::find(_allVarNames.begin(), _allVarNames.end(), varName);
    if (it != _allVarNames.end())
    {
        return it - _allVarNames.begin();
    }
    else
    {
        string msg = "The variable name (" + varName + ") does not exist in " \
                     "the variable name list.";
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }
}

template <class T>
GHeaderInfo GDataConverter<T>::readGFGridToLatLonRadNodes(
                                                    const GString& latVarName, 
                                                    const GString& lonVarName, 
                                                    const GString& radVarName)
{
    Logger::info(__FILE__, __FUNCTION__, "");
    cout << "Reading GeoFLOW grid files" << endl;

    // Read the x,y,z GeoFLOW grid filenames from the property tree
    GString xFilename = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.x");
    GString yFilename = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.y");
    GString zFilename = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.z");

    xFilename = _inputDir + "/" + xFilename;
    yFilename = _inputDir + "/" + yFilename;
    zFilename = _inputDir + "/" + zFilename;

    // Read the GeoFLOW x,y,z grid files (reader stores header and data)
    GFileReader<T> x(xFilename);
    GFileReader<T> y(yFilename);
    GFileReader<T> z(zFilename);

    // Verify data size
    if (!(x.data().size() == y.data().size() && 
          y.data().size() == z.data().size()))
    {

    }

    // Read each x,y,z location value and element layer ID into a collection 
    // of nodes. The IDs/header are the same for each x,y,z triplet so just 
    // use the IDs/header from the x grid.

    cout << "Converting x,y,z to lat,lon,r and reading GeoFLOW grid to nodes" \
         << " (spherical coordinates)" << endl;

    _nodes.clear(); // remove all elements from vector (i.e., size = 0)
    _nodes.shrink_to_fit(); // reduce vector capacity to vector size

    // Set the vector capacity in advance
    GSIZET numNodes = (x.header()).nNodesPerVolume;
    try
    {
        _nodes.reserve(numNodes);
    }
    catch (const std::length_error& e) 
    {
        std::string msg = "Error setting capacity for list of nodes: " + \
                          (x.header()).nNodesPerVolume + GString(e.what());
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    cout << "_allVarNames.size() is: " << _allVarNames.size() << endl;

    // For each node in the volume...
    array<T, 3> llr;
    for (auto i = 0u; i < numNodes; ++i)
    {
        llr = MathUtil::xyzToLatLonRadius<T>({x.data()[i], y.data()[i], z.data()[i]});

        // Add new node to list
        _nodes.emplace_back(_allVarNames.size(),
                            toVarIndex(latVarName), llr[0],
                            toVarIndex(lonVarName), llr[1],
                            toVarIndex(radVarName), llr[2],
                            x.elementLayerIDs()[i]);
    }

    // Save header
    _header = x.header();

    return x.header();
}

template <class T>
GHeaderInfo GDataConverter<T>::readGFGridToBoxNodes(const GString& xVarName, 
                                                    const GString& yVarName, 
                                                    const GString& zVarName)
{
    Logger::info(__FILE__, __FUNCTION__, "");
    cout << "Reading GeoFLOW grid files" << endl;

    // Read the x,y,z GeoFLOW grid filenames from the property tree
    GString xFilename = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.x");
    GString yFilename = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.y");
    GString zFilename = PTUtil::getValue<GString>(_ptRoot, "grid_filenames.z");

    xFilename = _inputDir + "/" + xFilename;
    yFilename = _inputDir + "/" + yFilename;
    zFilename = _inputDir + "/" + zFilename;

    // Read the GeoFLOW x,y,z grid files (reader stores header and data)
    GFileReader<T> x(xFilename);
    GFileReader<T> y(yFilename);
    GFileReader<T> z(zFilename);

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

    cout << "Reading GeoFLOW grid to nodes (box grid)" << endl;

    _nodes.clear(); // remove all elements from vector (i.e., size = 0)
    _nodes.shrink_to_fit(); // reduce vectory capacity to vector size

    // Set the vector capacity in advance
    GSIZET numNodes = (x.header()).nNodesPerVolume;
    try
    {
        _nodes.reserve(numNodes);
    }
    catch (const std::length_error& e) 
    {
        std::string msg = "Error setting capacity for list of nodes: " + \
                          (x.header()).nNodesPerVolume + GString(e.what());
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // For each node in the volume...
    for (auto i = 0u; i < numNodes; ++i)
    {
        // Add new node to list
        _nodes.emplace_back(_allVarNames.size(),
                            toVarIndex(xVarName), x.data()[i],
                            toVarIndex(yVarName), y.data()[i],
                            toVarIndex(zVarName), z.data()[i],
                            x.elementLayerIDs()[i]);
    }

    // Save header
    _header = x.header();

    return x.header();
}

template <class T>
GHeaderInfo GDataConverter<T>::readGFVariableToNodes(const GString& gfFilename,
                                                     const GString& varName)
{
    Logger::info(__FILE__, __FUNCTION__, "");
    cout << "Reading GF variable to nodes: " << varName << endl;

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
    GSIZET numNodes = (var.header()).nNodesPerVolume;
    for (auto i = 0u; i < numNodes; ++i)
    {
        _nodes[i].var(toVarIndex(varName), var.data()[i]);
    } 

    return var.header();  
}

template <class T>
void GDataConverter<T>::sortNodesByElemID()
{
    Logger::info(__FILE__, __FUNCTION__, "");
    cout << "Sorting nodes by element ID" << endl;

    // Use stable sort to make sure the same order of objects is retained for 
    // two objects with equal keys (since the original order of nodes within 
    // a GF element must be retained).
    stable_sort(_nodes.begin(), _nodes.end());
}

template <class T>
void GDataConverter<T>::sortNodesBy2DMeshLayer()
{
    Logger::info(__FILE__, __FUNCTION__, "");
    cout << "Sorting nodes by 2D mesh layer" << endl;

    // Sort the nodes by 2D mesh layer. For 3D elements, there are multiple 
    // 2D layers (x,y ref dir) in the radial direction

    GUINT nX = _header.polyOrder[0] + 1; // num nodes in x ref dir
    GUINT nY = _header.polyOrder[1] + 1; // num nodes in y ref dir
    GUINT nZ = 1; // 1 = default num nodes in z ref dir for a 2D dataset
    if (_header.polyOrder.size() == 3) // 3D dataset
    {
        nZ = _header.polyOrder[2] + 1; // num nodes in z ref dir
    }
    GUINT nXY = nX * nY; // num nodes per element in x,y ref dir
    GUINT nXYZ = nX * nY * nZ; // num nodes per element in x,y,z dir
    GUINT nNodesPerElemLayer = _header.nElemPerElemLayer * nXYZ;
    GUINT start, end, count = 0;

    // For each GeoFLOW element layer...
    for (auto i = 0u; i < _header.nElemLayers; ++i)
    {
        // For each 2D layer (x,y ref dir) in the element...
        for (auto k = 0u; k < nZ; ++k)
        {
            // For each 2D/3D element in the GeoFLOW element layer...
            for (auto j = 0u; j < _header.nElemPerElemLayer; ++j)
            {
                start = (i * nNodesPerElemLayer) + (j * nXYZ) + (k * nXY);
                end = start + nXY;

                // For each node in the 2D element (x,y ref dir)...
                for (GUINT h = start; h < end; ++h)
                {
                    // Assign a sort key to the node
                    _nodes[h].sortKey(count);
                }

                ++count;
            }
        }
    }

    // Use stable sort to make sure the same order of objects is retained for 
    // two objects with equal keys (since the original order of nodes within 
    // a GF element must be retained).
    stable_sort(_nodes.begin(), _nodes.end(), GNode<T>::sort_key_comp);
}

template <class T>
void GDataConverter<T>::faceToNodes()
{
    Logger::info(__FILE__, __FUNCTION__, "");
    cout << "Mapping faces to nodes (i.e., creating a list of GFace objects)" \
         << endl;

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
                indices.push_back(i + (x * nY) + y);             // bottom left
                indices.push_back(i + (x * nY) + (y + 1));       // bottom right
                indices.push_back(i + ((x + 1) * nY) + (y + 1)); // top right
                indices.push_back(i + ((x + 1) * nY) + y);       // top left

                // Add the face to the list
                _faces.push_back(GFace(indices));
            }
        } 
    }
}

template <class T>
void GDataConverter<T>::setDimensions(const map<GString, GSIZET>& dims)
{
    Logger::info(__FILE__, __FUNCTION__, "");
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
    Logger::info(__FILE__, __FUNCTION__, "");

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
    Logger::info(__FILE__, __FUNCTION__, "");

    // Write the dataset dimensions to the NetCDF file
    _nc->writeDimensions();
}

template <class T>
void GDataConverter<T>::writeNCDummyVariable(const GString& varName)
{
    Logger::info(__FILE__, __FUNCTION__, "");

    // Write the contents of a dummy variable (has no data) to the NetCDF file
    _nc->writeVariableDefinition(varName);
    _nc->writeVariableAttributes(varName);
}

template <class T>
void GDataConverter<T>::writeNCNodeVariable(const GString& rootVarName, 
                                            const GString& fullVarName)
{
    Logger::info(__FILE__, __FUNCTION__, "");

    // Write the contents of a node variable to the NetCDF file
    _nc->writeVariableDefinition(rootVarName);
    _nc->writeVariableAttributes(rootVarName);
    _nc->writeVariableData<T>(rootVarName, toVarIndex(fullVarName), _nodes);
}

template <class T>
template <typename U>
void GDataConverter<T>::writeNCVariable(const GString& varName, 
                                        const U& varValue)
{
    Logger::info(__FILE__, __FUNCTION__, "");

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
    Logger::info(__FILE__, __FUNCTION__, "");

    // Write the contents of a vector to the NetCDF file
    _nc->writeVariableDefinition(varName);
    _nc->writeVariableAttributes(varName);
    _nc->writeVariableData<U>(varName, values);
}