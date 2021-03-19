//==============================================================================
// Module       : gfile_reader.h
// Date         : 4/17/21 (SG)
// Description  : Read data from a binary GeoFLOW data file
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
// Derived From : none
//==============================================================================

#ifndef GFILEREADER_H
#define GFILEREADER_H

#include "gheader_info.h"

using namespace std;

/*!
 * Class GFileReader
 * 
 * Interface for reading a binary GeoFLOW data file. 
 */

class GFileReader
{
public:
    GFileReader(const GString& filename, GUINT dataSize=DEFAULT_DATA_SIZE);
    static void readHeader(
            const GString& filename, 
            GHeaderInfo& h, 
            GSIZET dataSize=DEFAULT_DATA_SIZE);
    void readData(const GString& filename);

    // Print helpers
    static void printHeader(const GHeaderInfo& h);
    void printHeader();
    void printData();

private:
    GHeaderInfo _header;
    vector<GDOUBLE> _data; // TODO: What should this type be?
};

#endif
