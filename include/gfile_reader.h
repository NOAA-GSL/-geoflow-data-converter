//==============================================================================
// Module       : gfile_reader.h
// Date         : 4/17/21 (SG)
// Description  : Read data from a binary GeoFLOW data file
// Copyright    : Copyright 2021. Regents of the University of Colorado.
//                All rights reserved.
// Derived From : none
//==============================================================================

#ifndef GDATAREADER_H
#define GDATAREADER_H

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
    GFileReader() {}
    static GSIZET readHeader(
            GString file, 
            GHeaderInfo& header,
            GSIZET dataSize=DEFAULT_DATA_SIZE);
    static void printHeader(GHeaderInfo header);

private:
};

#endif
