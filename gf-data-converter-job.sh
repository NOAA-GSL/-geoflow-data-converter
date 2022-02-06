#!/bin/bash -l

# Job script used for running on Hera. Replace the program's command line 
# argument with your JSON file.

# Set the SLURM batch options
#SBATCH --account=gsd-hpcs   # account job should run on
#SBATCH -D .                 # working dir name
#SBATCH -q batch             # type of queue
#SBATCH --nodes=1            # min num of nodes to request
#SBATCH --time=08:00:00      # max time to run
#SBATCH --exclusive          # job cannot share nodes with other jobs
#SBATCH -p bigmem            # memory needed by job

# Purge modules
module purge

# Load modules
module load gcc/11.1.0
module load gcc hdf5 netcdf-c netcdf-cxx

# Compile
make -f Makefile-hera

# Run the program (replace ugrid-3D.json with your json file)
./bin/main ugrid-3D.json

# Get the max amount of memory used up to this point in the job
report-mem
