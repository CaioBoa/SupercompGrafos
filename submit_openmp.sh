#!/bin/bash
#SBATCH --job-name=openmp
#SBATCH --output=output_openmp180.txt
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=2
#SBATCH --mem-per-cpu=1Gb
#SBATCH --time=00:15:00
#SBATCH --partition=espec

./OpenMP