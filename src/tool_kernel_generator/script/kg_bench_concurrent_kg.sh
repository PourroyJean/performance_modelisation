#!/bin/bash
#export PATH=`dirname $0`:$PATH


#CPU 0, 1, 2 .... MAX
NB_BENCH=16
BENCH="assembly"
NEXT_SOCKET=18


printf "Launching on {"
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






