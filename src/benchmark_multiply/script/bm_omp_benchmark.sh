#!/bin/bash

LOG_FILE=bm_tmp_time.delete
rm -f ${LOG_FILE} 2>&1 1>/dev/null

#--------------------------------------------------
#               BENCHMARK CONFIGURATION           #
#--------------------------------------------------
BM=./bin/benchmark_multiply/benchmark_multiply
COL=4000
LINES=4000
VERSION=6
#BLOCK_SIZE_LIST="1 2 4 8 16 20 32 40 80 100"
BLOCK_SIZE_LIST="40"
#--------------------------------------------------

#--------------------------------------------------
#                OPEN MP CONFIGURATION            #
#--------------------------------------------------
OMP_PROC_BIND_LIST="false true master close spread"
#OMP_PROC_BIND_LIST="spread"
OMP_PLACES_LIST="threads cores sockets"
#OMP_PLACES_LIST="cores"
OMP_NUM_THREADS_LIST=`seq 0 32 128`
OMP_NUM_THREADS_LIST='1 16 32 48 64 80 96 112 128'
#--------------------------------------------------

function print_res() {
  printf "%-7s %-9s %-10s %-7s %-5s %-14s %-10s\n" $1 $2 $3 $4 $5 $6 $7
}

print_res "THREADS" "PROC_BIND" "OMP_PLACES" "VERSION" "BLOCK" "RES" "TIME"
for OMP_NUM_THREADS in $OMP_NUM_THREADS_LIST; do
  for OMP_PROC_BIND in $OMP_PROC_BIND_LIST; do
    for OMP_PLACES in $OMP_PLACES_LIST; do
      for BLOCK_SIZE in $BLOCK_SIZE_LIST; do
        export OMP_NUM_THREADS=$OMP_NUM_THREADS
        export OMP_PROC_BIND=$OMP_PROC_BIND
        export OMP_PLACES=$OMP_PLACES
        #Sanity check: for some configuration the number of thread, the runtime send a warning
        #   "WARNING: Requested total thread count and/or thread affinity may result in oversubscription "
        #So we launch the command for 3 seconds and check if any error were reported
        timeout 3 ./$BM -V $VERSION -L $LINES -C $COL &>bm_tmp_time.delete
        if [ $(wc -l bm_tmp_time.delete | awk '{print $1}') -gt 3 ]; then
          print_res $OMP_NUM_THREADS $OMP_PROC_BIND $OMP_PLACES $VERSION $BLOCK 0 "ERROR" 0:00.00
        else
          /usr/bin/time -f'%E' ./$BM -V $VERSION -L $LINES -C $COL -B $BLOCK_SIZE &>bm_tmp_time.delete
          TIME=$(cat bm_tmp_time.delete | tail -n 1)
          RES=$(cat bm_tmp_time.delete | grep SUM | awk '{print $3}')
          print_res $OMP_NUM_THREADS $OMP_PROC_BIND $OMP_PLACES $VERSION $BLOCK_SIZE $RES $TIME
        fi
      done
      #If 1 thread: don't try other PROC_BIN/PLACES
      if [ $OMP_NUM_THREADS -eq 1 ]; then break 2; fi
    done
  done
done
rm -f ${LOG_FILE} 2>&1 1>/dev/null