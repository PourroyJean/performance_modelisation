##!/bin/bash

#--------------------------------------------------
#                OPEN MP CONFIGURATION            #
#--------------------------------------------------
export OMP_NUM_THREADS=32

#export OMP_PROC_BIND=false
#export OMP_PROC_BIND=true
#export OMP_PROC_BIND=master
export OMP_PROC_BIND=close
#export OMP_PROC_BIND=spread

#export OMP_PLACES=threads
export OMP_PLACES=cores
#export OMP_PLACES=sockets
#export OMP_PLACES={0:32},{64:32}
#--------------------------------------------------


#--------------------------------------------------
#               BENCHMARK CONFIGURATION           #
#--------------------------------------------------
BM=./bin/benchmark_multiply/benchmark_multiply
VERSION=6
COL=3000
LINES=3000
BLOCK_SIZE=20
#--------------------------------------------------


#--------------------------------------------------
#                 BENCHMARK EXECUTION             #
#--------------------------------------------------
VERSION=6
COL=3000
LINES=3000
BLOCK_SIZE=20

time ./$BM -V 4 -L $LINES -C $COL -V $VERSION -B $BLOCK_SIZE
#/home/pourroyj/local/bin/likwid-pin -c 1,3,6,63
#--------------------------------------------------
