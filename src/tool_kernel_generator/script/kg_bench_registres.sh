#!/bin/bash
#export PATH=`dirname $0`:$PATH


MAX=50
op=a
for ((i=1; i<=$MAX; i++)); do
    operation="$operation${op}"
    printf "%50s " $operation

#    ./bin/tool_kernel_generator/kg -L 10000  -W 128 -O $operation -D true -U 10 | grep  "^IPC" | awk '{print $2}'
    ./bin/tool_kernel_generator/kg  -W 512  -P double   -S 10 -L 9000 -U 10  -O mmmmmmmm  | grep  "^IPC" | awk '{print $2}'
done








