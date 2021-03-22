//==============================================================================
// Module       : gfile_reader.cpp
// Date         : 4/17/21 (SG)
// Description  : Read data from a binary GeoFLOW data file
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
// Derived From : none
//==============================================================================

#include <fstream>

template <class T>
GFileReader<T>::GFileReader(const GString& filename, GUINT dataSize)
{
    readHeader(filename, _header, dataSize);
}

template <class T>
void GFileReader<T>::readHeader(
        const GString& filename,
        GHeaderInfo& h,
        GSIZET dataSize)
{
    // Open file
    ifstream ifs(filename, ios::in | ios::binary);
    if (!ifs)
    {
        cerr << "Error: Cannot open file: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    // Read header info
    ifs.read((char*)&h.version, sizeof(h.version));
    ifs.read((char*)&h.dim, sizeof(h.dim));
    ifs.read((char*)&h.nElems, sizeof(h.nElems));

    // TODO: remove hard-coded GUINT and use decltype
    /*typedef decltype(h.polyOrder)::value_type PORDERTYPE;
    for (auto i = 0u; i < h.dim; ++i)
    {
        PORDERTYPE pOrder;
        ifs.read((char*)&pOrder, sizeof(PORDERTYPE));
        h.polyOrder.push_back(pOrder);
    }*/
    h.polyOrder.resize(h.dim); // each ref dir has its own poly order
    for (auto& p : h.polyOrder)
    {
        ifs.read((char*)&p, sizeof(GUINT));
    }
    
    ifs.read((char*)&h.gridType, sizeof(h.gridType));
    ifs.read((char*)&h.timeCycle, sizeof(h.timeCycle));
    ifs.read((char*)&h.timeStamp, sizeof(h.timeStamp));
    ifs.read((char*)&h.hasMultVars, sizeof(h.hasMultVars));

    // Derive other properties from header info
    h.nHeaderBytes = ifs.tellg(); // curr pos in file stream
    h.dataSize = dataSize;
    h.nNodesPerElem = 1;
    for (const auto& po : h.polyOrder)
    {
        h.nNodesPerElem *= (po + 1); // num nodes in a dim = (poly order + 1)
    }
    h.nNodes = h.nElems * h.nNodesPerElem;
    h.nDataBytes = h.nNodes * sizeof(T);

    ifs.close();
}

template <class T>
void GFileReader<T>::readData(const GString& filename)
{
    // Open file
    ifstream ifs(filename, ios::in | ios::binary);
    if (!ifs)
    {
        cout << "Error: Cannot open file: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    // Set file stream loc to start of data and allocate mem
    ifs.seekg(_header.nHeaderBytes);
    _data.resize(_header.nNodes);

    // Read variable data
    if (!ifs.read((char*)_data.data(), _header.nDataBytes))
    {
        cerr << "Error: Cannot read the requested " << _header.nDataBytes
             << " bytes of data from file: " << filename << endl;
        ifs.close();
        exit(EXIT_FAILURE);
    }

    ifs.close();
}

template <class T>
void GFileReader<T>::printHeader(const GHeaderInfo& h)
{
    // Print header info
    cout << endl;
    cout << "-----------" << endl;
    cout << "Header Info" << endl;
    cout << "-----------" << endl;
    cout << "IO version: " << h.version << endl;
    cout << "Dimension: " << h.dim << endl;
    cout << "Num elements: " << h.nElems << endl;
    cout << "Poly orders: "; 
    for (const auto& po : h.polyOrder)
    {
        cout << po << " ";
    }
    cout << endl;
    cout << "Grid type: " << h.gridType << endl;
    cout << "Time cycle: " << h.timeCycle << endl;
    cout << "Time stamp: " << h.timeStamp << endl;
    cout << "Has mult fields?: " << h.hasMultVars << endl;

    // Print derived header inof
    cout << endl;
    cout << "-------------------" << endl;
    cout << "Derived Header Info" << endl;
    cout << "-------------------" << endl;
    cout << "Num nodes per element: " << h.nNodesPerElem << endl;
    cout << "Num nodes in file: " << h.nNodes << endl;
    cout << "Data size bytes: " << h.dataSize << endl;
    cout << "Num header bytes: " << h.nHeaderBytes << endl;
    cout << "Num data bytes: " << h.nDataBytes << endl;
    cout << endl;
}

template <class T>
void GFileReader<T>::printHeader()
{
    printHeader(_header);
}

template <class T>
void GFileReader<T>::printData()
{
    for (auto i = 0u; i < _data.size(); ++i)
    {
        cout << "_data[" << i << "] is: " << _data[i] << endl;
    }
}
