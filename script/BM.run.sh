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

# PARSE PARAMETERS
while getopts hn:t:f:l:c:p: opt; do
  case $opt in
    h) usage ;;
    n) np=$OPTARG ;;
    t) nt=$OPTARG ;;
    f) fs=$OPTARG/$LOGNAME ;;
    l) l=$OPTARG ;;
    c) c=$OPTARG ;;
    p) p=$OPTARG ;;
    *) exit 2 ;;
  esac
done

JCOMP=$c
JPROC=$p

echo "Mon job: $JCOMP $JPROC $np "

shift $(($OPTIND - 1))  ### extra parameters are not meant to be leveraged, but they remain available as $*
if [ -z "${np}" ]; then usage; fi

set -v
set -x

cd `echo $PWD |sed "s:^/nfs:/ibnfs:g"`
export o=$PWD

# CHECK WHETHER PROFILING MODE IS EXPECTED
PROF=0
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
PROJECT_NAME="WORK_DIR"
APP_NAME=""
export d=$fs/$PROJECT_NAME/$APP_NAME/work.${np}Px${nt}T.jobid-$jobid.`hostname`.`date +"%Y%m%d.%H%M%S"`.${JCOMP}.${JPROC}.${l}

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
# Portability stuff, if necessary
#setintel
. /opt/intel/compilers_and_libraries/linux/bin/compilervars.sh intel64

# EXTRA STEPS IF PROFILING IS REQUESTED
if [ $PROF -ne 0 ]; then
  export MPIRUN_OPTIONS='-i myinstr'
  export I_MPI_STATS=all
  export I_MPI_STATS_FILE=$d/stats.all
  ln -s stats.all $d/stats.ipm
  ln -s stats.all $d/stats.txt
fi

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
EXE_NAME=bm
cp $o/bin/benchmark_memory/$EXE_NAME $d
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
#mympirun -a -np $np $myEXE -i $o/sets/compute-$np.bsmbench -o $d/BENCH-$np.out    &>> $d/myoutput
$myEXE --steplog 0.05 --unroll 2 --type read --cacheline 64 --prefix out --output out_TEST --matrixsize 2000 --stride 64 &>> $d/myoutput

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
mv $d/ $o/finalworkdirectory/

exit 0

