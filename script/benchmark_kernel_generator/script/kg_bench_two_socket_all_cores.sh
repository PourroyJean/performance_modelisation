#!/bin/bash
#export PATH=`dirname $0`:$PATH

BENCH="assembly"

# -- WILL LAUNCH THE BENCH ON CPUs : --
#CPU 0, 1, 2 .... NB_CORES_TO_BENCH
NB_CORES_TO_BENCH=18 #TOTAL


#  -- FIND THE FIRST CORE OF THE SECOND SOCKET --
#  numactl --hardware
#  node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53
#  node 1 cpus: 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71
NEXT_SOCKET=`numactl --hardware | grep "node 1" | head -n 1 | awk '{print $4}'`


printf "Launching $BENCH on {"
for ((i=0; i<($NB_CORES_TO_BENCH / 2); i++)); do
    printf "$i,  "
    ./$BENCH $i  | grep "Running" &
done

echo
printf "            "

# We don't execute the bench on the last core, see below
for ((i=$NEXT_SOCKET; i<($NB_CORES_TO_BENCH/2 + $NEXT_SOCKET - 1); i++)); do
    printf " $i, "
    ./$BENCH $i  | grep "Running" &
done



# We manually execute the last one to have at least one output to display
echo $(($NB_CORES_TO_BENCH/2 + $NEXT_SOCKET - 1))"}"
perf stat ./$BENCH $(($NB_CORES_TO_BENCH/2 + $NEXT_SOCKET - 1))



