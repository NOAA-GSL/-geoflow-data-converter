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

    /*!
     * Read the header from the GeoFLOW file.
     * 
     * @param filename input GeoFLOW file name
     * 
     * return the GeoFLOW header
     */
    static GHeaderInfo readHeader(const GString& filename);

    /*!
     * Read the data values from the GeoFLOW file.
     *
     * @param filename input GeoFLOW filename
     */
    void readData(const GString& filename);

    // Access
    const GHeaderInfo header() const { return _header; }
    const vector<T>& data() const { return _data; }
    const vector<GSIZET>& elementLayerIDs() const { return _elemLayerIDs; }

    /*!
     * Use the header's element ID array to set an element layer ID for each data 
     * value.
     */
    void setElementLayerIDs();

    /*!
     * Print each data value on a new line.
     */
    void printData();

private:
    GHeaderInfo _header;          // GeoFLOW file header & other meta data
    vector<T> _data;              // GeoFLOW file data values
    vector<GSIZET> _elemLayerIDs; // element layer ID for each data value
};

#include "../src/gfile_reader.ipp"

#endif

