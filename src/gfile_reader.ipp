//==============================================================================
// Date      : 3/17/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include <fstream>

#include "logger.h"

template <class T>
GFileReader<T>::GFileReader(const GString& filename)
{
    // Read header
    _header = readHeader(filename);

    // Read data
    readData(filename);

    // Set layer ID for each data value
    setElementLayerIDs();
}

template <class T>
GHeaderInfo GFileReader<T>::readHeader(const GString& filename)
{
    cout << "Reading GeoFLOW header from file: " << filename << endl;

    // Open file
    ifstream ifs(filename, ios::in | ios::binary);
    if (!ifs)
    {
        string msg = "Cannot open file: " + filename;
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // Read header info
    GHeaderInfo h;
    ifs.read((char*)&h.version, sizeof(h.version));
    ifs.read((char*)&h.dim, sizeof(h.dim));
    ifs.read((char*)&h.nElems, sizeof(h.nElems));

    h.polyOrder.resize(h.dim); // each ref dir has its own poly order
    for (auto& p : h.polyOrder)
    {
        ifs.read((char*)&p, sizeof(h.polyOrder[0]));
    }

    // Verify data
    if (h.polyOrder.size() < 2)
    {
        string msg = "Found only (" + to_string(h.polyOrder.size()) + ") " + \
                     "polynomial orders in file: " + filename + ". Need " + \
                     "a minimum of 2 (for each x & y reference direction).";
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);       
    }
    
    ifs.read((char*)&h.gridType, sizeof(h.gridType));
    ifs.read((char*)&h.timeCycle, sizeof(h.timeCycle));
    ifs.read((char*)&h.timeStamp, sizeof(h.timeStamp));
    ifs.read((char*)&h.hasMultVars, sizeof(h.hasMultVars));

    // Get total byte size of header
    h.nHeaderBytes = ifs.tellg(); // curr pos in file stream
    
    // Get num nodes in volume (x,y,z ref dir)
    h.nNodesPerVolume = h.nElems;
    for (const auto& p : h.polyOrder)
    {
        h.nNodesPerVolume *= (p + 1);
    }

    // Get num nodes per 2D element (x,y ref dir). Num nodes in one reference 
    // direction of one element = (poly order + 1)
    h.nNodesPer2DElem = 1;
    for (auto i = 0u; i < 2; ++i)
    {
        h.nNodesPer2DElem *= (h.polyOrder[i] + 1);
    }

    // TODO: Get num GeoFLOW element layers
    h.nElemLayers = 1;

    // Get num GeoFLOW elments per GeoFLOW element layer
    h.nElemPerElemLayer = h.nElems / h.nElemLayers;

    // Get num nodes per 2D layer (x,y ref dir)
    h.nNodesPer2DLayer = h.nElemPerElemLayer * h.nNodesPer2DElem;

    // Get num faces (includes sub faces) per 2D layer (x,y ref dir)
    h.nFacesPer2DLayer = h.nElemPerElemLayer * (h.polyOrder[0] * h.polyOrder[1]);

    // Get num 2D layers in the entire volume
    h.n2DLayers = h.nNodesPerVolume / h.nNodesPer2DLayer;

    ifs.close();

    return h;
}

template <class T>
void GFileReader<T>::readData(const GString& filename)
{
    cout << "Reading GeoFLOW data from file: " << filename << endl;

    // Open file
    ifstream ifs(filename, ios::in | ios::binary);
    if (!ifs)
    {
        string msg = "Cannot open file: " + filename;
        Logger::error(__FILE__, __FUNCTION__, msg);
        exit(EXIT_FAILURE);
    }

    // Set file stream location to start of data and allocate memory
    ifs.seekg(_header.nHeaderBytes);
    _data.resize(_header.nNodesPerVolume);

    // Read data
    GSIZET nDataBytes = _header.nNodesPerVolume * sizeof(T);
    if (!ifs.read((char*)_data.data(), nDataBytes))
    {
        cerr << "Error: Cannot read the requested " << nDataBytes
             << " bytes of data from file: " << filename << endl;
        ifs.close();
        exit(EXIT_FAILURE);
    }

    ifs.close();
}

template <class T>
void GFileReader<T>::setElementLayerIDs()
{
    // Use header's element ID array to set an element layer ID for each data
    // value
    for (auto i = 0u; i < _data.size(); ++i)
    {
        _elemLayerIDs.push_back(0);
    }
}

template <class T>
void GFileReader<T>::printData()
{
    for (auto i = 0u; i < _data.size(); ++i)
    {
        cout << "_data[" << i << "] is: " << _data[i] << endl;
    }
}
