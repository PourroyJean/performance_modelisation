#!/bin/bash
#
# The purpose of this benchmark is to test the behavior of one AMX unit (here core 0)
# when running two concurrent AMX programs using 2 different tile sets.
# It is necessary to have compiled two versions of the benchmark first,
# changing the tile numbers used: 
#
# Benchmark Script for AMX Unit Behavior Testing
# Author: Jean Pourroy, October 2023
# License: MIT
#
# Description:
# This script tests the behavior of one AMX unit (e.g., core 0) when running two concurrent AMX programs using 2 different tile sets.
# Prerequisites:
# - Ensure you've compiled two versions of the benchmark first, changing the tile numbers used (tile 1 2 3 and 4 5 6 for the second)
# - Adapt their name in both EXECUTABLE1 and EXECUTABLE1 variable.
#
# Usage: ./bench_2_tiles.sh


# Define the two executables
EXECUTABLE1="./bench-amxtile-BF16.exe_123"
EXECUTABLE2="./bench-amxtile-BF16.exe_456"
# Check if executables exist
if [[ ! -x "$EXECUTABLE1" ]] || [[ ! -x "$EXECUTABLE2" ]]; then
    echo "Error: Executables not found (or not executable)"
    exit 1
fi

# Run a reference benchmark
(time $EXECUTABLE1 0) &>time_ref.log

# Run the first executable in the background and measure its time
(time $EXECUTABLE1 0) &>time_123.log &
pid1=$!

# Run the second executable in the background and measure its time
(time $EXECUTABLE2 0) &>time_456.log &
pid2=$!

# Wait for both executables to finish
wait $pid1
wait $pid2

# Extract the real time from each log and convert it to seconds
timeRef=$(grep real time_ref.log | awk '{split($2,a,"m"); print a[1]*60 + substr(a[2], 1, length(a[2])-1)}')
time1=$(grep real time_123.log | awk '{split($2,a,"m"); print a[1]*60 + substr(a[2], 1, length(a[2])-1)}')
time2=$(grep real time_456.log | awk '{split($2,a,"m"); print a[1]*60 + substr(a[2], 1, length(a[2])-1)}')

# Display the times using the variable names
echo " -- Running one benchmark alone for reference -- "
echo " * Ref  for $EXECUTABLE1: $timeRef seconds"
echo " -- Running two benchmarks on  the same core  -- "
echo " * Time for $EXECUTABLE1: $time1 seconds"
echo " * Time for $EXECUTABLE2: $time2 seconds"

# The total time is the maximum of the two times since they run in parallel
total=$(echo "if ($time1 > $time2) $time1 else $time2" | bc)
echo "Total time to run both commands: $total seconds"

# Remove the log files
rm -f time_ref.log time_123.log time_456.log 2>/dev/null