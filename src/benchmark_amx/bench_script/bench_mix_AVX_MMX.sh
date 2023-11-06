#!/bin/bash
#
# Benchmark Script for AMX Unit Behavior Testing with Scalability
# Author: Jean Pourroy, October 2023
# License: MIT
#
# Description:
# This script tests the performance of one core when MMX and AVX_512 instructions are mixed
# It captures the execution time for each mode and displays the results.
#
# Prerequisites:
# - Ensure the executables are present and have execution permissions.
# - This script assumes the presence of `/usr/bin/time` for time measurement.
#
# Usage: ./bench_mix_AVX_MMX.sh

# Exit if any command fails
set -e

# Define the executables
EXE1="./kg_FFFF_512_5S"            # AVX-512
EXE2="./bench-amxtile-BF16.4S.exe"  # MMX
# Check if executables exist
if [[ ! -x "$EXE1" ]] || [[ ! -x "$EXE2" ]]; then
    echo "Error: Executables not found (or not executable)"
    exit 1
fi

# Temporary files to store results
temp_seq=$(mktemp)
temp_par=$(mktemp)

# Sequential execution
{
    /usr/bin/time -f "Sequential time for $EXE1: %e seconds" $EXE1 &>>$temp_seq
    /usr/bin/time -f "Sequential time for $EXE2: %e seconds" $EXE2 0 &>>$temp_seq
}

# Parallel execution
{
    /usr/bin/time -f "Parallel time for $EXE1 and $EXE2: %e seconds" bash -c "$EXE1 & $EXE2 0 & wait" &>>$temp_par
}

# Display the results
echo -e "\nResults:"
cat $temp_seq $temp_par | grep "time for"

# Cleanup temporary files
rm $temp_seq $temp_par
