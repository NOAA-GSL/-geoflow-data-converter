//==============================================================================
// Date        : 9/1/21 (SG)
// Description : Simple timer.
// Copyright   : Copyright 2021. Regents of the University of Colorado.
//               All rights reserved.
//==============================================================================

#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <time.h>

using namespace std;

class Timer
{
public:
    Timer() {}
    ~Timer() {}

     /*!
     * Get current CPU time. Time uses microsecond precision.
     * 
     * @return current time in seconds
     */
    static double getTime()
    {
        return (double)clock() / CLOCKS_PER_SEC;
    }

    /*!
     * Print elapsed time in seconds.
     * 
     * @param start starting value of the timer
     * @param end ending value of the timer
    */
    static void printElapsedTime(double start, double end, string msg="")
    {
        cout << "*** Elapsed time (" << msg << "): " << (end - start) << " s" 
             << endl << endl;
    }
};

#endif
