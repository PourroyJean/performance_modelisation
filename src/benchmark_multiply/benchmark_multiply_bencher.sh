#!/bin/bash
#export PATH=`dirname $0`:$PATH

LINES=10000
COL=100
MAX=2  #version max


#for ((VERSION=1; VERSION<=$MAX; VERSION++)); do
#    perf stat   ./bin/benchmark_multiply/benchmark_multiply -V 2 -C $LINES -L $COL 2>&1 | grep instruction | awk '{print $4}'
#                ./bin/benchmark_multiply/benchmark_multiply -V 2 -C $LINES -L $COL
#    perf stat   ./bin/benchmark_multiply/benchmark_multiply -V $VERSION -C $LINES -L $COL
#done

#srun -N 1 -p gre5_all perf stat ./bin/benchmark_multiply/benchmark_multiply -V 1 -L $LINES -C $COL > perf_stat
srun -N 1 -p gre5_all --comment=" disable prefetch"  perf stat ./bin/benchmark_multiply/benchmark_multiply -V 1 -L $LINES -C $COL > perf_stat







