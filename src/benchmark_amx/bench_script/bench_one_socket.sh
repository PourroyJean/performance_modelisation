#!/bin/bash

# AMX Benchmark Script for Testing an Executable Across Different Cores on One Socket
# Author: Jean Pourroy, October 2023
# License: MIT
#
# Description:
# This script tests the behavior of an executable across different cores on a single socket.
# It iterates over various core combinations, measures the execution time,
# and logs the results to a file.
#
# Prerequisites:
# - Ensure the benchmark executable is present and has execution permissions.
# - This script assumes a system with up to 56 cores per socket.
#
# Usage: ./bench_one_socket.sh

# Define the executable to be tested
executable="./bench-amxtile-BF16.1S.exe"

# Check if the executable exists
if [ ! -x "$executable" ]; then
    echo "Error: Executable $executable not found or not executable."
    exit 1
fi

# Define the log file and ensure it's empty before starting
logfile="time_one_socket.log"
>"$logfile"

# Number of cores per socket
CORE_PER_SOCKET=56

# Define core list for the socket in a single line format
cores_socket_1=($(seq 0 $(($CORE_PER_SOCKET - 1))))

# Display core list on a single line
echo "Core list for Socket: ${cores_socket_1[*]}"

# Iterate over each core number up to the specified number of cores
for NB_CORE in $(seq 1 $CORE_PER_SOCKET); do
    start_time=$(date +%s.%N)

    # Launch the benchmark for the specified number of cores
    for ((i = 0; i < $NB_CORE; i++)); do
        $executable "${cores_socket_1[$i]}" &
    done

    # Wait for all commands to finish
    wait

    end_time=$(date +%s.%N)
    elapsed_time=$(echo "$end_time - $start_time" | bc)
    echo "Execution time for $NB_CORE cores: $elapsed_time seconds" >> "$logfile"
done

# Display the timings
cat "$logfile"
