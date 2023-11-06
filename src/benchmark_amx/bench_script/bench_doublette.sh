#!/bin/bash
#
# Benchmark Script for AMX Unit Behavior Testing with Scalability
# Author: Jean Pourroy, October 2023
# License: MIT
#
# Description:
# This script tests the behavior of an executable across different core pairs.
# It iterates over various core combinations, measures the execution time,
# and logs the results to a file.
#
# Prerequisites:
# - Ensure the benchmark executable is present and has execution permissions.
# - This script assumes a dual-socket system with up to 56 cores per socket.
#
# Usage: ./bench_doublette.sh

# Exit if any command fails
set -e

# Define the executable to be tested
executable="./test-amxtile-BF16.1S.exe"

# Check if the executable exists
if [ ! -x "$executable" ]; then
    echo "Error: Executable $executable not found or not executable."
    exit 1
fi

# Log file
logfile="time_doublette.log"
> "$logfile"

# Define core lists for each socket
cores_socket_1=($(seq 0 55))
cores_socket_2=($(seq 56 111))

# Display core lists on a single line
echo "Core list for Socket 1: ${cores_socket_1[*]}"
echo "Core list for Socket 2: ${cores_socket_2[*]}"

# Iterate over core pairs
for NB_PAIR in $(seq 1 56); do
    start_time=$(date +%s.%N)

    # Launch the benchmark for cores from the first socket
    for ((i = 0; i < $NB_PAIR; i++)); do
        $executable "${cores_socket_1[$i]}" &
    done

    # Launch the benchmark for cores from the second socket
    for ((i = 0; i < $NB_PAIR; i++)); do
        $executable "${cores_socket_2[$i]}" &
    done

    # Wait for all commands to finish
    wait

    end_time=$(date +%s.%N)
    elapsed_time=$(echo "$end_time - $start_time" | bc)
    total_cores=$((2 * $NB_PAIR))
    echo "Execution time for $total_cores cores [$NB_PAIR from each socket]: $elapsed_time seconds" >> "$logfile"
done

# Display the timings
cat "$logfile"