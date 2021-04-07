# GeoFLOW Data Converter

## Overview

Convert [GeoFLOW](https://github.com/NOAA-GSL/GeoFLOW) (Geo FLuid Object Workbench) data to other data formats like [NetCDF-CF](http://cfconventions.org/) and [NetCDF UGRID](https://github.com/ugrid-conventions/ugrid-conventions).

Note: As of 4/21, UGRID has not been adopted into the official NetCDF specification.

## To Install

Tested on Ubuntu 18.04

1. C++ compiler, supporting the C++11 standard or newer

2. NetCDF ([NetCDF-C++ API](http://unidata.github.io/netcdf-cxx4/index.html))
```
sudo apt-get install libnetcdf-c++4-dev-1
```

## To Compile

1. Download the repository
```
git clone https://github.com/silverness2/geoflow-data-converter.git
```

2. Enter directory
```
cd geoflow-data-converter
```

3. Build source
```
make
```

## To Run

```
./bin/main
```