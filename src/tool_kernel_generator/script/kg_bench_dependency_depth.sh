#!/bin/bash
#export PATH=`dirname $0`:$PATH


MAX=30
for ((i=0; i<=$MAX; i++)); do
    printf "./kg -D $i "
    ./bin/tool_kernel_generator/kg -W 512  -P double   -S 1 -L 9000000 -U 10  -O ffffffffffffffffff  -D $i | grep "_value_" | awk '{print $6}'
done








