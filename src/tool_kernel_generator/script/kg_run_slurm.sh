#!/bin/bash -i
exec 2>&1

function usage() {
  echo 'Usage: '$0' -c <case> -n <nranks> [ -t <nthreads> ] [ -f <workfs> ] [ -l <label> ]'
  exit 1
}

/bin/echo '================================================================================'
/bin/echo -e $0' - VERSION 1.5.6 - 31-Jan-2016'
/bin/echo '================================================================================'

# DEFINE DEFAULT VALUES
c=""
np=""
nt=1
fs=$PWD
l=""

# PARSE PARAMETERS
while getopts hc:n:t:f:l: opt; do
  case $opt in
    h) usage ;;
    c) c=$OPTARG ;;
    n) np=$OPTARG ;;
    t) nt=$OPTARG ;;
    f) fs=$OPTARG/$LOGNAME ;;
    l) l=.$OPTARG ;;
    *) exit 2 ;;
  esac
done
shift $(($OPTIND - 1))  ### extra parameters are not meant to be leveraged, but they remain available as $*
if [ -c "${c}" -o -z "${np}" ]; then usage; fi

set -v
set -x

cd `echo $PWD |sed "s:^/nfs:/ibnfs:g"`
export o=$PWD




# CHECK WHETHER PROFILING MODE IS EXPECTED
PROF=0
#if [ -n "$SLURM_JOB_ID" ]; then
#  if [ `scontrol show job $SLURM_JOB_ID |grep -E 'Comment|Command' |grep -c 'prof'` -gt 0 ]; then PROF=1; fi
#else
#  if [ `echo $0 |grep -c prof` -gt 0 ]; then PROF=1; fi
#fi

# RETRIEVE JOBID
if [ -n "$SLURM_JOB_ID" ]; then
  jobid=$SLURM_JOB_ID
  scontrol show job $SLURM_JOB_ID
else
  jobid=${Lrealpat_JOBID:-0}
fi

# DEFINE WORK DIRECTORY
PROJECT_NAME=""
APP_NAME=""
#export d=$fs/$PROJECT_NAME/$APP_NAME/work.${c}.${np}Px${nt}T.jobid-$jobid.`hostname`.`date +"%Y%m%d.%H%M%S"`$l
export d=/nfs/pourroy/code/THESE/performance_modelisation/build/WORKDIR/work-${c}-${np}Px${nt}T--id_$jobid--.`hostname`.`date +"%Y%m%d.%H%M%S"`$l

# CREATE WORK DIRECTORY AND COPY RELEVANT DATA
mkdir -p $d


fsroot=`echo $fs |cut -f 2 -d /`
ln -s /nfs/pourroy/code/THESE/performance_modelisation/build/bin/tool_kernel_generator/* $d    ### custom cases (case, data and journal files available in <case>)


# cd TO WORK DIRECTORY
cd $d


# SET RUNTIME ENVIRONMENT
export PATH=.:$HOME/runtime/tools/bin:/opt/hpe/tools/bin:$PATH
export MYMPIRUN_ATTACH=`which attach`' -c a -i a'
export OMP_NUM_THREADS=$nt

# Portability stuff, if necessary
#setintel

#PPN
#ppn=`echo $SLURM_TASKS_PER_NODE |grep -o -E '^[0-9]+'`
rm -f hosts hosts.tmp
ppn=`echo $SLURM_TASKS_PER_NODE |grep -o -E '^[0-9]+'`
## Packed mapping
for h in `hostlist -e "$SLURM_NODELIST"`; do
  for i in `seq 1 $ppn`; do
    echo $h >> hosts.tmp
  done
done
## RR mapping
#for i in `seq 1 $ppn`; do
#  for h in `hostlist -e "$SLURM_NODELIST"`; do
#    echo $h >> hosts.tmp
#  done
#done
##
head -n $np hosts.tmp > hosts

# EXTRA STEPS IF PROFILING IS REQUESTED
if [ $PROF -ne 0 ]; then
  export MPIRUN_OPTIONS="$MPIRUN_OPTIONS -i myinstr"
  export I_MPI_STATS=all
  export I_MPI_STATS_FILE=$d/stats.all
  ln -s stats.all $d/stats.ipm
  ln -s stats.all $d/stats.txt
fi

# PRELOAD APPLICATION FILES AND TUNE MPI
### IBMPI/PCMPI +
mytype=`dmidecode |grep 'Product Name:'`
if [[ "$mytype" =~ "m710" ]]; then
  myX="-mpi=pcmpi -pib.ofed"
  myMPIOPT="$MPIRUN_OPTIONS -IBV -e MPI_IB_CARDS=0:1 -e MPI_RDMA_MSGSIZE=32768,32768,32768 -prot -ndd"
elif [ `ibstat |grep -c hfi` -gt 0 ]; then
  myX="-mpi=pcmpi -pib.infinipath"
  export LD_LIBRARY_PATH=/usr/lib64/psm2-compat:$LD_LIBRARY_PATH
  myMPIOPT="$MPIRUN_OPTIONS -PSM"
else
  myX="-mpi=pcmpi -pib.ofed"
  myMPIOPT="$MPIRUN_OPTIONS -IBV -rdma"
fi
### IBMPI/PCMPI -

# LOG RUNTIME ENVIRONMENT AND INITIAL DIRECTORY CONTENT
printenv |&sort >& env_variables.out
ulimit -a >& shell_limits.out
pwd > where.out
ls -ltr >& ls_initial.out
lfs getstripe * >& lfs_getstripe_initial.out
grep MHz /proc/cpuinfo >& cpufreq.out
grep 'model name' /proc/cpuinfo >& cpumodel.out
sysctl -a >& sysctl.out

# LOG START TIME
now=`date +"date=%Y%m%d.%H%M%S -- seconds=%s"`
echo "BENCHMARK STARTED ON $now" |tee timestamps.out

echo "RUN THE BENCH NOW" >>myoutput

# RUN BENCHMARK
# RUN BENCHMARK
# RUN BENCHMARK
# RUN BENCHMARK
# RUN BENCHMARK

/usr/bin/time -p ./kg -P double -W 128 -U 4 -L 100000000 -O mmmmmm  >& myoutput


# RUN BENCHMARK
# RUN BENCHMARK
# RUN BENCHMARK
# RUN BENCHMARK
# RUN BENCHMARK

# LOG COMPLETION TIME
now=`date +"date=%Y%m%d.%H%M%S -- seconds=%s"`
echo "BENCHMARK COMPLETED ON $now" |tee -a timestamps.out
wallclock=`awk -F '=' '{if(index($1,"STARTED")!=0){s=$3}else if(index($1,"COMPLETED")!=0)print $3-s}' timestamps.out`
echo "WALLCLOCK TIME WAS $wallclock s" |tee -a timestamps.out

# LOG FINAL DIRECTORY CONTENT
ls -ltr >& ls_final.out
lfs getstripe * >& lfs_getstripe_final.out

# REMOVE USELESS FILES IF NECESSARY
rm -f *.dat* *.cas* *.pdat*

# COPY RELEVANT FILES TO ORIGINAL DIRECTORY
cd $o
#mv $d $o

mv slurm-$SLURM_JOB_ID* $d
mv *obid-$SLURM_JOB_ID* $d
mv *.log log/

exit 0
