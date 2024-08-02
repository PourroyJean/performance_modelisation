#!/bin/bash

rm jobfile

BENCH_DIR=$(pwd)
MAX_THREADS=8

unit_measured="ghz flopst flops w w0 w1 flopsw c0 c1"

# Loop over each specified partition
for partition in "$@"
do
    # Generate proper SBATCH
    echo "#!/bin/bash" > SBATCH.$partition
    echo "#SBATCH --partition=$partition" >> SBATCH.$partition
    echo "#SBATCH --output=out.$partition" >> SBATCH.$partition
    echo "cd $BENCH_DIR/.." >> SBATCH.$partition
    echo "cp -r $BENCH_DIR $BENCH_DIR/../bench_$partition" >> SBATCH.$partition
    echo "cd $BENCH_DIR/../bench_$partition" >> SBATCH.$partition
    echo "./kg_loop_different_thread_counts.sh $MAX_THREADS" >> SBATCH.$partition

    # Run job and store the jobid to the jobfile
    sbatch SBATCH.$partition | sed 's/Submitted batch job //' >> jobfile
done

# While any job is still running, loop until it finishes
while [ -s jobfile ] ; do
    for job in $(cat jobfile)
    do
        if squeue | grep -q $job; then
            break
        else 
            grep -v "$job" jobfile > tmp_jobfile
            mv tmp_jobfile jobfile
        fi
    done
    sleep 1
done

max_cores=0
# Find longest run (partition with the most cores)
for partition in "$@"
do
    tail -n +3 out.$partition > $partition
    cores=$(cat $partition | wc -l)
    if [[ $cores -gt $max_cores ]]; then
        max_cores=$cores
    fi

    # Extract measured values from jobs output
    for j in $(seq 1 $(echo $unit_measured | wc -w))
    do
        cut -d';' -f$j $partition > $partition."$(echo $unit_measured | cut -d " " -f$j)"
    done
done

# End line of file is counted
max_cores=$(($max_cores-1));

# Compile outfiles in a single file
for unit in $unit_measured
do
    echo "Threads Count" > results.$unit
    seq 1 $max_cores >> results.$unit
done

# Compile results in a single file and cleanup
for partition in "$@"
do
    for unit in $unit_measured
    do
        # Add this partition's results to the compiled results
        paste -d ';' results.$unit $partition.$unit > tmp_results
        mv tmp_results results.$unit
    done   

    # Remove generated SBATCH
    rm SBATCH.$partition
    
    # Remove copied workdir
    rm -r $(dirname $BENCH_DIR)/bench_$partition

    # Remove output files
    rm $partition
    rm out.$partition

    for unit in $unit_measured
    do
        rm $partition.$unit
    done   
done

# Generate final plot
run="python3 ./plot_results.py"

# Add input files to python call
for unit in $unit_measured
do
    run+=" results.$unit"
done

# Add output file name to python call
run+=" cpu_compare_$(date +"%m-%d-%y_%H:%M").png"
$run

echo "Final plot available at \"cpu_compare_$(date +"%m-%d-%y_%H:%M").png\". Raw results available at results.*"