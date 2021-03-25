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
    const vector<T>& data() const { return _data; }
    const GHeaderInfo header() { return _header; }

    // Print
    static void printHeader(const GHeaderInfo& h);
    void printHeader();
    void printData();

private:
    GHeaderInfo _header;
    vector<T> _data;
};

#include "../src/gfile_reader.ipp"

#endif

