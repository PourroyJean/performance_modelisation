#!/bin/bash
# ------------------------------------------------------------------
#                      YAMB Profiling Tool
#
#       Yet Another Memory Bandwidth Profiling Tool
# ------------------------------------------------------------------


 perf stat -o log  -a -e cpu-clock,cpu-migrations,page-faults -I 100 sleep 10


exit


#perf stat -a -e cpu-clock,cpu-migrations,page-faults -I 100 sleep 2


#sudo sh -c 'echo kernel.perf_event_paranoid=1 > /etc/sysctl.d/local.conf'

BENCH="/home/jean/programmation/performance_modelisation/build/bin/benchmark_multiply/benchmark_multiply -V 2 -L 3000"

echo "FEU"
#(./${BENCH}; echo finDodo &) &

rm log
#bash -c exec -a JANNOT perf stat -o log  -a -e cpu-clock,cpu-migrations,page-faults -I 100 ./$BENCH  ; echo "<finPerf>"  &
exec -a JANNOT bash perf stat -o log  -a -e cpu-clock,cpu-migrations,page-faults -I 100 $BENCH  &
PID=$!



echo "PID : $PID"

sleep 1;
top -n 1

killall JANNOT

sleep 1;
top -n 1

#sleep 2;
#kill -kill $PID

echo "END"