##!/bin/bash

#--------------------------------------------------
#                OPEN MP CONFIGURATION            #
#--------------------------------------------------
OMP_PROC_BIND_LIST="false true master close spread"
OMP_PLACES_LIST="threads cores sockets"
OMP_NUM_THREADS_LIST="32 64 128"
#--------------------------------------------------

#--------------------------------------------------
#               BENCHMARK CONFIGURATION           #
#--------------------------------------------------
BM=./bin/benchmark_multiply/benchmark_multiply
COL=4000
LINES=4000
VERSION=6
BLOCK_SIZE=40
#--------------------------------------------------

function print_res() {
  printf "%-8s %-9s %-10s %-7s %-10s \n" $1 $2 $3 $4 $5
}

print_res "THREADS" "PROC_BIND" "OMP_PLACES" "VERSION" "TIME"
for OMP_NUM_THREADS in $OMP_NUM_THREADS_LIST; do
  for OMP_PROC_BIND in $OMP_PROC_BIND_LIST; do
    for OMP_PLACES in $OMP_PLACES_LIST; do
      #Sanity check: for some configuration the number of thread, the runtime send a warning
      #   "WARNING: Requested total thread count and/or thread affinity may result in oversubscription "
      #So we launch the command for 3 seconds and check if any error were reported
      timeout 3 ./$BM -V $VERSION -L $LINES -C $COL &>bm_tmp_time.delete
      if [ $(wc -l bm_tmp_time.delete | awk '{print $1}') -gt 3 ]; then
        print_res $OMP_NUM_THREADS $OMP_PROC_BIND $OMP_PLACES $VERSION "ERROR"
      else
        /usr/bin/time -f'%E' ./$BM -V $VERSION -L $LINES -C $COL -B $BLOCK_SIZE &>bm_tmp_time.delete
        TIME=$(cat bm_tmp_time.delete | tail -n 1)
        print_res $OMP_NUM_THREADS $OMP_PROC_BIND $OMP_PLACES $VERSION $TIME
      fi
    done
  done
done
rm bm_tmp_time.delete