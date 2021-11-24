##!/bin/bash

#--------------------------------------------------
#                OPEN MP CONFIGURATION            #
#--------------------------------------------------
#OMP_NUM_THREADS_LIST="8 16 32 64 128"
OMP_NUM_THREADS_LIST=`seq 10 1 128`
#OMP_NUM_THREADS_LIST="64"
export OMP_PROC_BIND=spread
export OMP_PLACES=cores;
#--------------------------------------------------

#--------------------------------------------------
#               BENCHMARK CONFIGURATION           #
#--------------------------------------------------
BM=./bin/benchmark_multiply/benchmark_multiply
COL=4000
LINES=4000
VERSION=6
#BLOCK_SIZE_LIST="1 2 4 8 16 20 32 40 80 100"
#BLOCK_SIZE_LIST="10"
#BLOCK_SIZE_LIST="20 40"
BLOCK_SIZE_LIST="40"
#BLOCK_SIZE_LIST=`seq 28 1 50`
#--------------------------------------------------


function print_res() {
  printf "%-8s %-9s %-20s %-10s\n" $1 $2 $3 $4
}
print_res "THREADS" "BLOCK" "RES" "TIME"


for OMP_NUM_THREADS in $OMP_NUM_THREADS_LIST; do
for BLOCK_SIZE in $BLOCK_SIZE_LIST; do
        export OMP_NUM_THREADS=$OMP_NUM_THREADS
        /usr/bin/time -f'%E' ./$BM -V $VERSION -L $LINES -C $COL -B $BLOCK_SIZE &>bm_tmp_time.delete
#        ./$BM -V $VERSION -L $LINES -C $COL -B $BLOCK_SIZE
        TIME=$(cat bm_tmp_time.delete | tail -n 1)
        RES=$(cat bm_tmp_time.delete |  grep SUM | awk '{print $3}')
        print_res $OMP_NUM_THREADS $BLOCK_SIZE $RES $TIME
done
done
rm bm_tmp_time.delete