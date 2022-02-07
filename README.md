# GeoFLOW Data Converter

### Overview

Convert [GeoFLOW](https://github.com/NOAA-GSL/GeoFLOW) (Geo FLuid Object Workbench) data to other data formats like [NetCDF-CF](http://cfconventions.org/) and [NetCDF UGRID](https://github.com/ugrid-conventions/ugrid-conventions).

Note: As of 2021, UGRID has not been adopted into the official NetCDF specification.

# Installation and Usage

The GeoFLOW Data Converter can be run on a Desktop or on NOAA's Hera supercomputer. The latter must be used when converting GeoFLOW datasets with a large memory footprint.

## Option A: Running on a Desktop
Tested on Ubuntu 18.04.

### Install

1. Install a C++ compiler, supporting the C++11 standard or newer.

2. Install NetCDF ([NetCDF-C++ API](http://unidata.github.io/netcdf-cxx4/index.html)).
```
sudo apt-get install libnetcdf-c++4-dev-1
```

### Get Code

Download repository.
```
git clone https://github.com/NOAA-GSL/geoflow-data-converter.git
```

### Compile and Run

1. Enter directory.
```
cd geoflow-data-converter
```

2. Build source.
```
make
```

3. Edit your UGRID JSON file as specified in the UGRID JSON File section.

4. Run program. Replace `JSON_FILENAME` with your UGRID JSON filename.
```
./bin/main JSON_FILENAME
```

### View ASCII Version of NetCDF File
To view a human-readable version of a NetCDF file, run the following command. Replace `NETCDF_FILENAME` with your `.nc` filename.
```
ncdump NETCDF_FILENAME
```

## Option B: Running on a NOAA RDHPCS (Research & Development HPC System) System
Tested on NOAA's Hera supercomputer. For instructions on logging onto Hera and other Hera commands, see the [RDHPCS docs](https://rdhpcs-common-docs.rdhpcs.noaa.gov/wiki/index.php/Start).
### Install

1. Once logged into your user account on Hera, set up a path to the custom module files created by the GeoFLOW team that they regularly use. There are two different ways to do this:
- Option A: From a terminal, each time you log in and get a new shell:
```
module use /scratch2/BMC/gsd-hpcs/modman/modulefiles/base
```
- Option B: Place the module path inside your `~/.bashrc` file so the path setup only needs to be done once:
```
export HERA_MODULEPATH_ROOT="/scratch2/BMC/gsd-hpcs/modman/modulefiles/base"
module use ${HERA_MODULEPATH_ROOT}
```

2. Load the modules manually. This step can be skipped as it is integrated into the job script described below. However, if you are running into any issues with compilation, this step is a good way to test if the required modules are installed.

List the available modules. After running the above command that sets up the path pointing to custom modules, the command:
```
module avail
```
This should output something similar to the following which confirms the custom module path has been loaded:
```
---------------- /scratch2/BMC/gsd-hpcs/modman/modulefiles/base ----------------
   cmake/3.20.2 (D)    gcc/11.1.0    llvm/12.0.0    nvhpc/21.5    pgi/21.5
```

Load the `gcc/11.1.0` module and list the available modules again:
```
module load gcc/11.1.0
module avail
```

This should output something similar to:
```
-------------------------------------------------- /scratch2/BMC/gsd-hpcs/modman/modulefiles/compiler/gcc/11.1.0 --------------------------------------------------
   blis/0.8.1    boost/1.76.0    gptl/8.0.3    hdf5/1.12.1    hwloc/2.4.1    libevent/2.1.12    netcdf/4.8.1    openblas/0.3.15    openmpi/4.1.1    ucx/1.10.1

--------------------------------------------------------- /scratch2/BMC/gsd-hpcs/modman/modulefiles/base ----------------------------------------------------------
   cmake/3.20.2 (D)    gcc/11.1.0 (L)    llvm/12.0.0
```

### Get Code

Download the repository
```
git clone https://github.com/NOAA-GSL/geoflow-data-converter.git
```

### Compile and Run

1. Enter directory.
```
cd geoflow-data-converter
```

2. Edit your UGRID JSON file as specified in the UGRID JSON File section.

3. Edit the batch job script `gf-data-converter.sh` to point to your UGRID JSON file by replacing the filename `ugrid-3D.json` with your UGRID JSON in the following line:
```
./bin/main ugrid-3D.json
```

4. Submit a batch job to Hera by running the job script which:
- Specifies required sbatch arguments
- Purges modules
- Loads necessary modules needed for compilation
- Compiles the application
- Runs the program
```
sbatch gf-data-converter-job.sh
```

### Useful Commands on Hera

- Submitting a job (your `~/.bashrc` environment will automatically be used):
```
sbatch NAME_OF_JOB_SCRIPT

Example: sbatch gf-data-converter.sh
```

- Showing jobs and their job IDs associated with your user name that are waiting or running:
```
squeue --users=YOUR_USER_NAME

Example: squeue --users=Shilpi.Gupta
```

- Canceling a job:
```
scancel JOB_ID
```

- Getting current memory usage of a job:
```
report-mem -j JOB_ID
```

- Showing all jobs associated with your user name since a certain date:
```
sacct -S YYYY-MM-DD -u USER_NAME

Example: sacct -S 2020-01-01 -u Shilpi.Gupta
```

# UGRID JSON FILE

# Using NCAR's VAPOR with GeoFLOW UGRID Files