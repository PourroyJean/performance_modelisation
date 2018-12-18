#!/usr/bin/env bash

CODE=/nfs/pourroy/code/THESE/performance_modelisation/build/bin/benchmark_multiply/benchmark_multiply
CODE=/nfs/pourroy/code/THESE/performance_modelisation/build/assembly
#. /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh intel64
#g++ --std=c++11 test2.cpp -o $CODE


/nfs/pourroy/code/THESE/performance_modelisation/src/tool_oprofile/merger_dml/jini.core2

#for i in {0..39}; do
#   numactl --physcpubind=$i ./test2&
#done
#wait
$CODE -V 4 -L 1000 -C 1000


sudo opcontrol --dump
T=$(date +%H%M%S)
T=test
opreport     >op1.$T
opreport -d  >op2.$T
objdump -d $CODE  > OBJ
~dml/TOOLS/objdump_parsing OBJ op2.$T | tee op3.$T
