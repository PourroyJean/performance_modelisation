#!/bin/bash -i

function main() {

SCRIPT=`realpath $0`
SCRIPTPATH=`dirname $SCRIPT`
gre0
#gre6
}

# Functions ready to be leveraged:
# setpart <partition> : adopt specified partition for subsequent invocations of built-in sbatch
# setconst <constraint_string> : adopt specified constraints for subsequent invocations of built-in sbatch
# setfeat <feature_string> : adopt specified comment string for subsequent invocations of built-in sbatch
# setname <name> : set name for subsequent invocations of built-in sbatch
# setres <reservation> : submit through specific reservation for subsequent invocations of built-in sbatch
# setcond   : set condition so that next job will depend on last submitted one (retrieved thanks to its name)
# resetcond : remove any condition for next job
# settopo nlots=<nslots> leafsize=<leaf_size> ppn=<ppn> [full|excl] : set parameters so that the appropriate number of leaf switches and hosts are allocated 
# sbatch [<extra_sbatch_flags>] <script> <args> : submit job according to what was set by previous functions

###########################################################################
function gre0() {
lfsz=18
ppn=28
resetcond
for n in 16
do
  if [ $(( $lfsz * $ppn )) -lt $n ];then mode=excl; else mode=full; fi
  settopo  nslots=$n leafsize=$lfsz ppn=$ppn $mode

    setpart  skl
#    setconst 4110
    setconst 6148
    setname "B_6148"

    ### FEATURE ###
    fTURBO="turbo off"
    fMONITOR="monitorfreq; monitorflops"
#    fHYPERT="disable cpus = smt"
    fFREQ="cap cpu freq=2.0Ghz"
    setfeat "$fFREQ ; $fTURBO ; $fMONITOR ; $fHYPERT"

#  sbatch -L  kg_run_slurm.sh -c sedan_4m -n $n -t 1 -f /nfs -l ${ppn}ppn.TEST
  sbatch  $SCRIPTPATH/kg_run_slurm.sh -n $n -t 1 -f /nfs -l ${ppn}ppn.TEST
  #setcond # Pour lancer 1 job par 1. Un wait.
done

}

###########################################################################
function settopo() {
  myns=0
  myleafsize=999999
  myppn=1
  myfull=""
  myexcl=""
  while [ -n "$1" ]; do
    case "$1" in
      *slot*) myns=`echo "$1" |sed 's:[A-Za-z=:]::g'`; shift ;;
      *leaf*size*) myleafsize=`echo "$1" |sed 's:[A-Za-z=:]::g'`; shift ;;
      *ppn*) myppn=`echo "$1" |sed 's:[A-Za-z=:]::g'`; shift ;;
      *full*) myfull="yes"; shift ;;
      *excl*) myexcl="yes"; shift ;;
      *) echo 'Unkwown request passed to settopo, aborting...'; exit 1 ;;
    esac
  done
  mynn=`awk 'END{mynn='${myns}/${myppn}';if(mynn!=(int(mynn)))mynn++;print int(mynn)}' /dev/null`
  mysw=`awk 'END{mysw='${mynn}/${myleafsize}';if(mysw!=(int(mysw)))mysw++;print int(mysw)}' /dev/null`
  if [ -n "$myexcl" ]; then
    mynn=$(($mysw * $myleafsize))
    myns=$(($mynn * $myppn))
  elif [ -n "$myfull" ]; then
    myns=$(($mynn * $myppn))
  fi
  mytopo="--switches=${mysw} --contiguous -N ${mynn} -n ${myns}"
}
function setpart() { mypartition="$1"; }
function setconst() { myconstraints="$1"; }
function setfeat() { myfeatures="$1"; }
function setname() { myjobname="$1"; }
function setres() { myreservation="--reservation=$1"; }
function getjobid() { myjobid=`scontrol show job "$1" | awk -F ' ' '/JobId/ {print $1}' | cut -f 2 -d '=' |tail -n 1`; }
function setcond() { getjobid "${myjobname}"; mycond="-d afterany:${myjobid}"; }
function resetcond() { mycond=""; }
#function sbatch() { echo `which sbatch` --mail-type=ALL -p $mypartition $myreservation -C "$myconstraints" $mytopo $mycond -J "$myjobname" --comment="$myfeatures" $*; }
function sbatch() { `which sbatch` --mail-type=ALL -p $mypartition $myreservation -C "$myconstraints" $mytopo $mycond -J "$myjobname" --comment="$myfeatures" $*; }

###########################################################################
main
