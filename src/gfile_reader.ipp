//==============================================================================
// Date      : 3/17/21 (SG)
// Copyright : Copyright 2021. Regents of the University of Colorado.
//             All rights reserved.
//==============================================================================

#include <fstream>

template <class T>
GFileReader<T>::GFileReader(const GString& filename)
{
    // Read header
    _header = readHeader(filename);

    // Read data
    readData(filename);
}

template <class T>
GHeaderInfo GFileReader<T>::readHeader(const GString& filename)
{
    // Open file
    ifstream ifs(filename, ios::in | ios::binary);
    if (!ifs)
    {
        cerr << "Error: Cannot open file: " << filename << endl;
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
    
    ifs.read((char*)&h.gridType, sizeof(h.gridType));
    ifs.read((char*)&h.timeCycle, sizeof(h.timeCycle));
    ifs.read((char*)&h.timeStamp, sizeof(h.timeStamp));
    ifs.read((char*)&h.hasMultVars, sizeof(h.hasMultVars));

    // Get total byte size of header
    h.nHeaderBytes = ifs.tellg(); // curr pos in file stream
    
    // Get num nodes (incl. sub nodes) per element
    // Num nodes in one reference direction of one element = (poly order + 1)
    h.nNodesPerElem = 1;
    for (const auto& p : h.polyOrder)
    {
        h.nNodesPerElem *= (p + 1);
    }

    // Get num nodes (incl. sub nodes) in entire dataset
    h.nNodes = h.nElems * h.nNodesPerElem;

    ifs.close();

    return h;
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

    // Read data
    GSIZET nDataBytes = _header.nNodes * sizeof(T);
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
    for (const auto& p : h.polyOrder)
    {
        cout << p << " ";
    }
    cout << endl;
    cout << "Grid type: " << h.gridType << endl;
    cout << "Time cycle: " << h.timeCycle << endl;
    cout << "Time stamp: " << h.timeStamp << endl;
    cout << "Has mult fields?: " << h.hasMultVars << endl;

    // Print derived header info
    cout << endl;
    cout << "------------------------" << endl;
    cout << "Derived Info from Header" << endl;
    cout << "------------------------" << endl;
    cout << "Num nodes per element: " << h.nNodesPerElem << endl;
    cout << "Num nodes in file: " << h.nNodes << endl;
    cout << "Num header bytes: " << h.nHeaderBytes << endl;
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
