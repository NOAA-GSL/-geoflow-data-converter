//==============================================================================
// Module       : gfile_reader.cpp
// Date         : 4/17/21 (SG)
// Description  : Read data from a binary GeoFLOW data file
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
// Derived From : none
//==============================================================================

#include "gfile_reader.h"

#include <fstream>
#include <iomanip>
#include <assert.h>

//using namespace std;

GSIZET GFileReader::readHeader(
        GString file,
        GHeaderInfo& header,
        GSIZET dataSize)
{
    ifstream ifs(file, ios::in | ios::binary);
    if (!ifs)
    {
        cout << "Error: Cannot open file: " << file << endl;
        exit(EXIT_FAILURE);
    }

    ifs.read((char*)&header.version, sizeof(header.version));
    ifs.read((char*)&header.dim, sizeof(header.dim));
    ifs.read((char*)&header.numElems, sizeof(header.numElems));
    typedef decltype(header.polyOrders)::value_type PORDERTYPE;
    for (auto i = 0u; i<header.dim; ++i)
    {
        PORDERTYPE pOrder;
        ifs.read((char*)&pOrder, sizeof(PORDERTYPE));
        header.polyOrders.push_back(pOrder);
    }
    ifs.read((char*)&header.gridType, sizeof(header.gridType));
    ifs.read((char*)&header.timeCycle, sizeof(header.timeCycle));
    ifs.read((char*)&header.timeStamp, sizeof(header.timeStamp));
    ifs.read((char*)&header.hasMultVars, sizeof(header.hasMultVars));

    header.numHeaderBytes = ifs.tellg();
    header.dataSize = dataSize;

    ifs.close();

    return header.numHeaderBytes;
}

void GFileReader::printHeader(GHeaderInfo header)
{
    cout << endl;
    cout << "Header Info" << endl;
    cout << "-----------" << endl;
    cout << "IO version: " << header.version << endl;
    cout << "Dimension: " << header.dim << endl;
    cout << "Num elements: " << header.numElems << endl;
    cout << "Poly orders: "; 
    for (auto po : header.polyOrders)
    {
        cout << po << " ";
    }
    cout << endl;
    cout << "Grid type: " << header.gridType << endl;
    cout << "Time cycle: " << header.timeCycle << endl;
    cout << "Time stamp: " << header.timeStamp << endl;
    cout << "Has mult fields?: " << header.hasMultVars << endl;
    cout << "Num header bytes: " << header.numHeaderBytes << endl;
    cout << "Data size (bytes): " << header.dataSize << endl;
    cout << endl;
}
