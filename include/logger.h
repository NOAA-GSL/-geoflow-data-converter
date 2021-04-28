//==============================================================================
// Date        : 3/25/21 (SG)
// Description : Simple logger.
// Copyright   : Copyright 2021. Regents of the University of Colorado.
//               All rights reserved.
//==============================================================================

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

using namespace std;

class Logger
{
public:
    Logger() {}
    ~Logger() {}

     /*!
     * Print errror message.
     * 
     * @param file name of source file
     * @param func name of function
     * @param msg error message description
     */
    static void error(const char* file, const char* func, string msg)
    {
        cerr << "ERROR [" << file << "::" << func << "()] " << msg << endl;
    }

    /*!
     * Print warning message.
     * 
     * @param file name of source file
     * @param func name of function
     * @param msg warning message description
     */
    static void warning(const char* file, const char* func, string msg)
    {
        cerr << "WARNING [" << file << "::" << func << "()] " << msg << endl;
    }
};

#endif