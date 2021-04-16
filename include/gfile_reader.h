//==============================================================================
// Date         : 3/17/21 (SG)
// Description  : Reads header and data from a binary GeoFLOW data file.
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
//==============================================================================

#ifndef GFILEREADER_H
#define GFILEREADER_H

#include "gheader_info.h"

using namespace std;

template <class T>
class GFileReader
{
public:
    GFileReader(const GString& filename);
    ~GFileReader() {}

    // Read
    static GHeaderInfo readHeader(const GString& filename);
    void readData(const GString& filename);

    // Access
    const GHeaderInfo header() const { return _header; }
    const vector<T>& data() const { return _data; }
    const vector<GSIZET>& elementLayerIDs() const { return _elemLayerIDs; }

    // Compute
    void setElementLayerIDs();

    // Print
    static void printHeader(const GHeaderInfo& h);
    void printHeader();
    void printData();

private:
    GHeaderInfo _header;          // GeoFLOW file's header & other meta data
    vector<T> _data;              // GeoFLOW file's data values
    vector<GSIZET> _elemLayerIDs; // element layer ID for each node
};

#include "../src/gfile_reader.ipp"

#endif

