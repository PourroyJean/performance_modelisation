#!/bin/bash -i
exec 2>&1

function usage() {
  echo 'Usage: '$0' -n <nranks> [ -t <nthreads> ] [ -f <workfs> ] [ -l <label> ]'
  exit 1
}

/bin/echo '================================================================================'
/bin/echo -e $0' - VERSION 1.5.7 - 02-Oct-2016'
/bin/echo '================================================================================'

# DEFINE DEFAULT VALUES
np=""
nt=1
fs=$PWD
l=""
k=""
j=""

# PARSE PARAMETERS
while getopts hn:t:f:l:c:p:j: opt; do
  case $opt in
    h) usage ;;
    n) np=$OPTARG ;;
    t) nt=$OPTARG ;;
    f) fs=$OPTARG/$LOGNAME ;;
    l) l=$OPTARG ;;
    c) c=$OPTARG ;;
    p) p=$OPTARG ;;
    j) j=$OPTARG ;;
    *) exit 2 ;;
  esac
done



shift $(($OPTIND - 1))  ### extra parameters are not meant to be leveraged, but they remain available as $*
if [ -z "${np}" ]; then usage; fi

set -v
set -x

cd `echo $PWD |sed "s:^/nfs:/ibnfs:g"`
export o=$PWD

# CHECK WHETHER PROFILING MODE IS EXPECTED
PROF=1
if [ -n "$SLURM_JOB_ID" ]; then
  if [ `scontrol show job $SLURM_JOB_ID |grep -E 'Comment|Command' |grep -c 'prof'` -gt 0 ]; then PROF=1; fi
else
  if [ `echo $0 |grep -c prof` -gt 0 ]; then PROF=1; fi
fi

# RETRIEVE JOBID
if [ -n "$SLURM_JOB_ID" ]; then
  jobid=$SLURM_JOB_ID
  scontrol show job $SLURM_JOB_ID
else
  jobid=${LSB_JOBID:-0}
fi

# DEFINE WORK DIRECTORY
export d=/nfs/pourroy/code/BSM_Bench/BSM_BenchJ/WORK_DIR/${l}_${j}/work.${np}Px${nt}T.jobid-$jobid.`hostname`.`date +"%Y%m%d.%H%M%S"`

# CREATE WORK DIRECTORY AND COPY RELEVANT DATA
mkdir -p $d



#cp -Rp $o/data $d
#ln -s $o/data/* $d

# cd TO WORK DIRECTORY
cd $d

# SET RUNTIME ENVIRONMENT
export PATH=.:$HOME/runtime/tools/bin:/opt/hpe/tools/bin:$PATH
export MYMPIRUN_ATTACH=`which attach`' -c a -i a'
export OMP_NUM_THREADS=$nt

#SORIN
#source /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh  intel64
#source /opt/intel/impi/current/bin64/mpivars.sh

# --- Jean
#OPENMPI GNU
#export PATH=/opt/openmpi/gnu/bin/:$PATH

#OPENMPI intel
#source /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh  intel64
#export PATH=/opt/openmpi/intel/bin/:$PATH


#IntelMPI GNU et Intel
source /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh  intel64
source /opt/intel/impi/current/bin64/mpivars.sh

#Itac for Hana
#source /opt/intel/parallel_studio_xe_2017.1.043/psxevars.sh



# Portability stuff, if necessary
#setintel

# EXTRA STEPS IF PROFILING IS REQUESTED
if [ $PROF -ne 0 ]; then
  export MPIRUN_OPTIONS='-i myinstr'
  export I_MPI_STATS=all
  export I_MPI_STATS_FILE=$d/stats.all
  ln -s stats.all $d/stats.ipm
  ln -s stats.all $d/stats.txt
fi

#  export MPIRUN_OPTIONS='-i JEAN_MPI_instr_output'
#  export I_MPI_STATS=all
#  export I_MPI_STATS_FILE=$d/stats.all
#  ln -s stats.all $d/stats.ipm
#  ln -s stats.all $d/stats.txt

# LOG RUNTIME ENVIRONMENT AND INITIAL DIRECTORY CONTENT
printenv |&sort >& env_variables.out
ulimit -a >& shell_limits.out
pwd > where.out
ls -ltr >& ls_initial.out
lfs getstripe * >& lfs_getstripe_initial.out
grep MHz /proc/cpuinfo >& cpufreq.out
grep 'model name' /proc/cpuinfo >& cpumodel.out
sysctl -a >& sysctl.out

# READY BENCHMARK
#EXE_NAME=bsmbench_compute
#EXE_NAME=bsmbench_compute_OPENMPI_GNU
#EXE_NAME=bsmbench_compute_OPENMPI_INTEL
#EXE_NAME=bsmbench_compute_INTELMPI_GNU

EXE_NAME=${l}

cp $o/$EXE_NAME $d
export myEXE=$d/$EXE_NAME
ldd $myEXE >& ldd.out
readsox.sh $myEXE >& compiler_flags.out
#prechargement
doall myvmtouch $d $myEXE &> vmtouch.out ### should "load" input data, the exe and its shared libs in page cache...

# LOG START TIME
now=`date +"date=%Y%m%d.%H%M%S -- seconds=%s"`
echo "BENCHMARK STARTED ON $now" |tee timestamps.out

# RUN BENCHMARK
#/usr/bin/time -p $myEXE ...    &>> $d/myoutput
#mympirun -m flagpourMpi -a -np $np $myEXE ...    &>> $d/myoutput
#mympirun -a -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/myoutput   &>> $d/myoutput

#Si OpenMPI: GNU et Intel
#mpirun          -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput

#***** Si Intel: GNU et Intel
#compute
/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput
#comms
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -np $np $myEXE -i $o/sets/comms-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput
#balance
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -np $np $myEXE -i $o/sets/balance-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput

#unbalanced
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -ppn  8 -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -ppn 16 -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -ppn 28 -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput

#check binding 16 = 8 - 8
#export I_MPI_PIN_PROCESSOR_LIST=0,1,2,3,4,5,6,7,16,17,18,19,20,21,22,23
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun -genv I_MPI_PIN_PROCESSOR_LIST "0,1,2,3,4,5,6,7,16,17,18,19,20,21,22,23" -ppn 16 -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput

#check binding 28 = 14 - 14
#export I_MPI_PIN_PROCESSOR_LIST=0,1,2,3,4,5,6,7,8,9,10,11,12,13,16,17,18,19,20,21,22,23,24,25,26,27,28,29
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun -genv I_MPI_PIN_PROCESSOR_LIST "0,1,2,3,4,5,6,7,8,9,10,11,12,13,16,17,18,19,20,21,22,23,24,25,26,27,28,29" -ppn 28 -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput


#+++++++++++++++++++ ++++++++++++ + + + + PROFILING
#COPY PROFILING TOOLS
#cp  $o/JEAN.jini.sh $o/JEAN.jop.sh $d

#Moonshot
#export I_MPI_FABRICS=shm:tcp
#export I_MPI_TCP_NETMASK=eth1
#./JEAN.jini.sh
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput
#./JEAN.jop.sh $jobid

#+++++++++++++++++++ ++++++++++++ + + + + Moonshot
#export I_MPI_FABRICS=shm:tcp
#export I_MPI_TCP_NETMASK=eth1
#Compute
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput
#Comms
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -np $np $myEXE -i $o/sets/comms-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput
#Balance
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -np $np $myEXE -i $o/sets/balance-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput



#+++++++++++++++++++ ++++++++++++ + + + + Moonshot threads
#export I_MPI_FABRICS=shm:tcp
#export I_MPI_TCP_NETMASK=eth1
#set OMP_NUM_THREADS=4
#export OMP_NUM_THREADS=4
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun  -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput


#Moonshot avec mympirun marche pas
#mympirun -m openmpi                                                         -a -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput

#Itac for Hana
#/opt/intel/compilers_and_libraries_2017.1.132/linux/mpi/intel64/bin/mpirun -trace  -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np   &>> $d/myoutput




# LOG COMPLETION TIME
now=`date +"date=%Y%m%d.%H%M%S -- seconds=%s"`
echo "BENCHMARK COMPLETED ON $now" |tee -a timestamps.out
wallclock=`awk -F '=' '{if(index($1,"STARTED")!=0){s=$3}else if(index($1,"COMPLETED")!=0)print $3-s}' timestamps.out`
echo "WALLCLOCK TIME WAS $wallclock s" |tee -a timestamps.out

# LOG FINAL DIRECTORY CONTENT
ls -ltr >& ls_final.out
lfs getstripe * >& lfs_getstripe_final.out

# REMOVE USELESS FILES IF NECESSARY
#rm -f *.bin

# COPY RELEVANT FILES TO ORIGINAL DIRECTORY
cd $o

exit 0

