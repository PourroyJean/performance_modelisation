#!/bin/bash
#export PATH=`dirname $0`:$PATH


MAX=50
op=m
for ((i=1; i<=$MAX; i++)); do
    operation="$operation${op}"
    printf "%50s " $operation

    ./bin/tool_kernel_generator/kg -L 10000  -W 128 -O $operation -D true | grep  "^IPC" | awk '{print $2}'
done








