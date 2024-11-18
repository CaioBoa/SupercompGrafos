#!/bin/bash
#SBATCH --job-name=mpi
#SBATCH --output=output_mpi180.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --mem-per-cpu=1Gb
#SBATCH --time=00:15:00
#SBATCH --partition=espec

./MPI