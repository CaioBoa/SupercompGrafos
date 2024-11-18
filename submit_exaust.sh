#!/bin/bash
#SBATCH --job-name=exaust
#SBATCH --output=output_exaust100.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --mem-per-cpu=1Gb
#SBATCH --time=00:15:00
#SBATCH --partition=espec

./Exaustiva