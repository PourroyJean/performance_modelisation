#!/bin/bash


srun    -N 1  \
        -n 10 \
        --sockets-per-node=1 \
        --cpu_bind=verbose \
        numactl   --physcpubind=0,1,2,3,4,5,6,7,8,9      ./bin/benchmark_dml_mem/dml_mpi --log 8 --matrixsize 1000 --stride 8 --measure 100
#        numactl   --physcpubind=0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143      ./bin/benchmark_dml_mem/dml_mpi --log 8 --matrixsize 1000 --stride 8 --measure 100


