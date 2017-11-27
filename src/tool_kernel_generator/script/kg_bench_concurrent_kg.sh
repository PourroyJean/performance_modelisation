#!/bin/bash
#export PATH=`dirname $0`:$PATH


#CPU 0, 1, 2 .... MAX
NB_BENCH=16 #TOTAL

BENCH="a.out"
BENCH="assembly_fluent_like_sedan"

NEXT_SOCKET=18
NEXT_SOCKET=`numactl --hardware | grep "node 1" | head -n 1 | awk '{print $4}'`


printf "Launching $BENCH on {"
for ((i=0; i<($NB_BENCH / 2); i++)); do
    printf "$i,  "
    ./$BENCH $i  | grep "Running" &
done

echo
printf "            "

for ((i=$NEXT_SOCKET; i<($NB_BENCH/2 + $NEXT_SOCKET - 1); i++)); do
    printf " $i, "
    ./$BENCH $i  | grep "Running" &
done



echo $(($NB_BENCH/2 + $NEXT_SOCKET - 1))"}"
./$BENCH $(($NB_BENCH/2 + $NEXT_SOCKET - 1))






