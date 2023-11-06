#!/bin/bash
#
# Benchmark Script for AMX Unit Behavior Testing with Scalability
# Author: Jean Pourroy, October 2023
# License: MIT
#
# Description:
# This script tests the scalability of the AMX unit across different core groups.
# It iterates over different core combinations and measures the execution time.
#
# Prerequisites:
# - Ensure the benchmark executable is present and executable.
# - The script assumes a dual-socket system with up to 56 cores per socket.
#
# Usage: ./bench_two_sockets.sh


# Define the executable
EXE="./bench-amxtile-BF16.4S.exe"

# Log file
logfile="time_all.log"

# Ensure the log file is empty before starting
> "$logfile"

# List of core groups to test scalability
NB_CORE_PER_SOCKET=56

# Retrieve cores from each socket for the current size in a single line format
cores_socket_1=$(seq -s " " 0 $NB_CORE_PER_SOCKET)
cores_socket_2=$(seq -s " " 56 $((55 + $NB_CORE_PER_SOCKET)))
echo "Core list: $cores_socket_1 -- $cores_socket_2"


start_time=$(date +%s.%N)

# Launch the benchmark for the main core
# perf stat $EXE 0 &

# Launch the benchmark for cores from the first socket
for core in $cores_socket_1; do
    $EXE "$core" &
done

# Launch the benchmark for cores from the second socket
for core in $cores_socket_2; do
    $EXE "$core" &
done

# Wait for all commands to finish
wait
end_time=$(date +%s.%N)
elapsed_time=$(echo "$end_time - $start_time" | bc)

# Log the elapsed time
total_cores=$((2 * $NB_CORE_PER_SOCKET))
echo "Execution time for $total_cores core(s) [${NB_CORE_PER_SOCKET} from each socket]: $elapsed_time seconds"
