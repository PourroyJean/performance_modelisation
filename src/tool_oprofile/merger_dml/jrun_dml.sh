#!/usr/bin/env bash

CODE="/nfs/pourroy/code/THESE/performance_modelisation/build/bin/benchmark_multiply/benchmark_multiply -V 4 -L 1000 -C 1000"
CODE=/nfs/pourroy/code/THESE/performance_modelisation/build/assembly
CODE=/nfs/pourroy/code/THESE/performance_modelisation/test/horner/horner1_long
#. /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh intel64
#g++ --std=c++11 test2.cpp -o $CODE


/nfs/pourroy/code/THESE/performance_modelisation/src/tool_oprofile/merger_dml/jini.core2

PARSER=~dml/TOOLS/objdump_parsing
PARSER=/nfs/pourroy/code/THESE/performance_modelisation/build/bin/tool_oprofile/merger
PARSER=~dml/TOOLS/objdump_parsing



#for i in {0..39}; do
#   numactl --physcpubind=$i ./test2&
#done
#wait
time $CODE


sudo opcontrol --dump
T=$(date +%H%M%S)
T=test
opreport     >op1.$T
opreport -d  >op2.$T
objdump -d $CODE  > OBJ
$PARSER OBJ op2.$T | tee op3.$T
