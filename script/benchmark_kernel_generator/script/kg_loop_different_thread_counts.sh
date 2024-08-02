#!/bin/bash

if [ "$1" = "MAX" ] || [ "$1" = "max" ] ; then
    cpu_max=$(lscpu | grep "^CPU(s):" | cut -d ' ' -f2- |  awk '{$1=$1;print}')
else
    cpu_max=$1
fi

threads=$(lscpu | grep "^Thread(s) per core:" | cut -d ' ' -f4- |  awk '{$1=$1;print}')
cores=$(lscpu | grep "^Core(s) per socket:" | cut -d ' ' -f4- |  awk '{$1=$1;print}')
sockets=$(lscpu | grep "^Socket(s):" | cut -d ' ' -f2- |  awk '{$1=$1;print}')

if lscpu | grep avx512 > /dev/null; then
    size=512
    op="ffffffffffffffffffffffffffffff"
else
    size=256
fi
    op="ffffffffffffff"

# Retrieve CPU model from lscpu
cpu_model=$(lscpu | grep "Model name" | cut -d ' ' -f3- | awk '{$1=$1;print}')

# Echo headers
headers="$(hostname) $cpu_model $sockets:$cores:$threads | (GHz);"
headers+="$(hostname) $cpu_model $sockets:$cores:$threads | (FLOP/s/t);"
headers+="$(hostname) $cpu_model $sockets:$cores:$threads | (FLOP/s);"
headers+="$(hostname) $cpu_model $sockets:$cores:$threads | (W (Mean) - Full Server);"

# Add power header for every package individually
for j in $(seq 0 $((sockets - 1)))
do
    headers+="$(hostname) $cpu_model $sockets:$cores:$threads | (W (Mean) - CPU Package $j);"
done

headers+="$(hostname) $cpu_model $sockets:$cores:$threads | (FLOP/s/W - Full Server);"

# Same for temperature
for j in $(seq 0 $((sockets - 1)))
do
    headers+="$(hostname) $cpu_model $sockets:$cores:$threads | (°C (Mean) - CPU Package $j);"
done

headers=${headers::-1}
echo $headers

export OMP_PLACES="cores"
export OMP_PROC_BIND="TRUE"
for((i=1;i<=$cpu_max;i++)); do
    export OMP_NUM_THREADS=$i

    # Cleanup behind loops
    rm -rf apmdir
    
    export APM_T0=$(date --date="now +2 seconds" +%s.%N)

    # Run cpu power & temp monitor
    /opt/hpe/tools/bin/start_apm.local.sh apmdir cpupower > /dev/null

    # Sleep until monitor starts
    date_now=$(date +%s.%N)
    if (( $(bc -l <<< "$run_at_s - $date_now - 0.01 > 0") )); then
        sleep 0$(bc -l <<< "$run_at_s - $date_now - 0.01")
    fi
    ./kg -W $size -O $op -L 20000000 --openmp true > bench_result

    # Stop monitoring tools
    /opt/hpe/tools/bin/stop_apm.local.sh 2> /dev/null

    ghz=$(cat bench_result | grep "+ Bench frequency is" | cut -d " " -f5 | sed 's/GHz//')
    flops=$(cat bench_result |  grep "Double" | awk '{$1=$1;print}' | cut -d ' ' -f3)
    flops_total=$(cat bench_result |  grep "Double" | awk '{$1=$1;print}' | cut -d ' ' -f4)

    # Remove headers from file
    for j in $(seq 0 $((sockets - 1)))
    do
        cat apmdir/$(hostname)/cpupowerstats.$j | grep -ve [a-Z] > monitorcpupower$j.out.tmp
    done

    # Remove overmeasured points
    # -> Getting the time spent "on the benchmark"
    instructions_line=$(grep -n "NB INSTRUCTIONS" bench_result | awk '{print $1}' FS=":")
    instructions_line=$((instructions_line + 1))
    duration=$(awk "NR==$instructions_line{ print; exit }" ./bench_result | awk '{$1=$1;print}' | cut -d " " -f2)

    # Counting out of line points by verifying 
    cut -d " " -f1 monitorcpupower0.out.tmp | sort -n  > time
    out_of_time_points=$(awk "\$1 > $duration {print \$1}" time | wc -l)
    total_points=$(cat time | wc -l)
    kept_points=$((total_points - $out_of_time_points))
    
    # Sort power and temperature values independently and remove untreated points
    # Let's assume monitoring time points are the same (usually something like ~ 3ms difference)
    cat time | head -n +$kept_points | sort -rn > time.tmp
    mv time.tmp time

    for j in $(seq 0 $((sockets - 1)))
    do
        cut -d " " -f2 monitorcpupower$j.out.tmp | head -n +$kept_points > power.$j
        cut -d " " -f3 monitorcpupower$j.out.tmp | head -n +$kept_points > temperature.$j
    done

    # Full server CPU power
    paste power*  | awk {'print $1 + $2'} | sort -rn  > power

    # Individual CPU package power
    for j in $(seq 0 $((sockets - 1)))
    do
        sort -rn -o power.$j power.$j
        sort -rn -o temperature.$j temperature.$j
    done

    # Duration of the measure
    duration=$(head -1 time)

    # Read power key values
    max_power=$(head -1 power)
    min_power=$(tail -1 power)
    mean_power=$(awk -v kept_points=$kept_points '{ sum += $1 } END { printf sum / kept_points }' power)
    mean_power_full=$mean_power

    power_data="$max_power/$min_power/$mean_power;"
    temp_data=""

    # Store data for every CPU package individually
    for j in $(seq 0 $((sockets - 1)))
    do
        # Read individual CPU package temperature
        max_temp=$(head -1 temperature.$j)
        min_temp=$(tail -1 temperature.$j)
        mean_temp=$(awk -v kept_points=$kept_points '{ sum += $1 } END { printf sum / kept_points }' temperature.$j)

        temp_data+="$max_temp/$min_temp/$mean_temp;"

        # Read individual CPU package wattage
        max_power=$(head -1 power.$j)
        min_power=$(tail -1 power.$j)
        mean_power=$(awk -v kept_points=$kept_points '{ sum += $1 } END { printf sum / kept_points }' power.$j)

        power_data+="$max_power/$min_power/$mean_power;"
    done

    # Remove trailing |
    temp_data=${temp_data::-1}
    power_data=${power_data::-1}

    # FLOP/s/W
    # We have the total flops measured for this run and the a mean of the energy consumption
    flops_per_watt=$(awk '{printf $1 / $2}' <<< "$flops_total $mean_power_full")

    echo "$ghz;$flops;$flops_total;$power_data;$flops_per_watt;$temp_data"
done
