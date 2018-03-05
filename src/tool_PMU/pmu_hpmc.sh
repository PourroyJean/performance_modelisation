#!/usr/bin/env bash

# ------------------------------------------------------------------
#   Author  [Jean Pourroy - jean.pourroy at g_:) _mail.com]
#           [Frederic Ciesielski]
#           [Patrick Demichel]
#
#   Description: Bash file using to start/stop HPMC counters for Skylake Architecture through /dev/msu/*msr interface
#
# ------------------------------------------------------------------
# Source: https://github.com/PourroyJean/performance_modelisation
# ------------------------------------------------------------------
# ref: bash template from
#           https://github.com/RenatGilmanov/shell-script-template/edit/master/getopts/template-commands.sh
# ref: code inspired from John Mc Calpin answers on intel forum
#           https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/595214

VERSION=1.0
USAGE="Usage: $0 {enable, disable, start, stop, check, read, reset}
             - enable   enabling   HPMC counters from the global registers      IA32_PERF_GLOBAL_CTRL
             - disable  disabling  HPMC counters from the global registers      IA32_PERF_GLOBAL_CTRL
             - start    setup and start the HPMC counters                       IA32_PERFEVTSEL
             - stop     stop the HPMC counters                                  IA32_PERFEVTSEL
             - check    dump the HPMC configuration                             IA32_PERFEVTSEL
             - read     dump the  PMC counters                                  IA32_PMC
             - reset    set the value 0 in the PMC counters                     IA32_PMC\n"


# --- Option processing --------------------------------------------
while getopts ":vh" optname
  do
    case "$optname" in
      "v")
        echo "Version $VERSION"
        exit 0;
        ;;
      "h")
        printf "$USAGE"
        exit 0;
        ;;
      "?")
        echo "Unknown option $OPTARG"
        exit 0;
        ;;
      ":")
        echo "No argument value for option $OPTARG"
        exit 0;
        ;;
      *)
        echo "Unknown error while processing options"
        exit 0;
        ;;
    esac
  done

shift $(($OPTIND - 1))

cmd=$1
param=$2
command="command_$1"

# -----------------------------------------------------------------

# ---- SETTINGS ----


# For SKYLAKE: Logical Core Performance Counter Event Select MSRs
#   HPMC   Config   Counter
#	   0    0x186     0x4C1
#	   1    0x187     0x4C2
#	   2    0x188     0x4C3
#	   3    0x189     0x4C4


MSR_TOOS_DIR="/nfs/pourroy/code/THESE/msr-tools"
export NCORES=`cat /proc/cpuinfo | grep -c processor`
export MAXCORE=`expr $NCORES - 1`

echo "# -- Number of cores is      $NCORES"
echo "# -- Process from       0 to $MAXCORE"
echo "# -- Command is: $command "


# ---- MAIN ----

function command_enable () {
    # Enable all counters in IA32_PERF_GLOBAL_CTRL (0x38f)
    #   bits 34:32 enabled the three fixed function counters
    #	bits 3:0 enable the 4 programmable counters for the logical processor
    echo "Enabling and Checking IA32_PERF_GLOBAL_CTRL on all cores"
    echo "  000000070000000f  <--  should be equal to "
    for core in `seq 0 $MAXCORE`
    do
        echo -n "$core "

        #   Fixed Function 1, 2,3                                       4 IA_32_PMCx enable
        #	         0111                0000 0000 0000 0000 0000 0000 0000        1111
        # 0x0000000   0                   0    0    0    0    0    0    0           f
        $MSR_TOOS_DIR/wrmsr -p $core 0x38f 0x000000000000000f
        $MSR_TOOS_DIR/rdmsr -p $core -x -0 0x38f

    done
}

function command_disable () {
    # Enable all counters in IA32_PERF_GLOBAL_CTRL (0x38f)
    echo "Disabling and Checking IA32_PERF_GLOBAL_CTRL on all cores"
    echo "  0  <--  should be equal to "
    for core in `seq 0 $MAXCORE`
    do
        echo -n "$core "
        $MSR_TOOS_DIR/wrmsr -p $core 0x38f 0x0
        $MSR_TOOS_DIR/rdmsr -p $core -x 0x38f

    done
}


function command_start (){

    echo "ACTIVATING PMC  $core"
    for core in `seq 0 $MAXCORE`
    do
        $MSR_TOOS_DIR/wrmsr -p $core 0x186 0x004301a1 # Counter 0 Uops Dispatched on Port 0
        $MSR_TOOS_DIR/wrmsr -p $core 0x187 0x004302a1 # Counter 1 Uops Dispatched on Port 1
        $MSR_TOOS_DIR/wrmsr -p $core 0x188 0x004304a1 # Counter 2 Uops Dispatched on Port 2
        $MSR_TOOS_DIR/wrmsr -p $core 0x189 0x004308a1 # Counter 3 Uops Dispatched on Port 3
    done
}


function command_stop (){
    echo "DESACTIVATING every HPMC"

    #If ($core) and ($core + 1) represents the two logical threads of the same physical core
    for core in `seq 0 $MAXCORE`
    do
#        if [ $(($core % 2 )) -eq 0 ] ; then
            $MSR_TOOS_DIR/wrmsr -p $core 0x186 0
            $MSR_TOOS_DIR/wrmsr -p $core 0x187 0
            $MSR_TOOS_DIR/wrmsr -p $core 0x188 0
            $MSR_TOOS_DIR/wrmsr -p $core 0x189 0
#        else
#            $MSR_TOOS_DIR/wrmsr -p $core 0
#            $MSR_TOOS_DIR/wrmsr -p $core 0
#            $MSR_TOOS_DIR/wrmsr -p $core 0
#            $MSR_TOOS_DIR/wrmsr -p $core 0
#        fi
    done

}

function command_check () {

	echo "Printing out all HPMC configuration"
    echo "Config        HPMC_1     HPMC_2     HPMC_3     HPMC_4"
    for core in `seq 0 $MAXCORE`
    do
        v1=`$MSR_TOOS_DIR/rdmsr -p $core 0x186`
        v2=`$MSR_TOOS_DIR/rdmsr -p $core 0x187`
        v3=`$MSR_TOOS_DIR/rdmsr -p $core 0x188`
        v4=`$MSR_TOOS_DIR/rdmsr -p $core 0x189`
        printf "Core %02d : %10s %10s %10s %10s \n" $core  $v1  $v2  $v3  $v4
    done
}

function command_read (){
    echo "Reading    COUNTER_1  COUNTER_2  COUNTER_3  COUNTER_4"
    for core in `seq 0 $MAXCORE`
    do
        v1=`$MSR_TOOS_DIR/rdmsr -p $core 0x4C1`
        v2=`$MSR_TOOS_DIR/rdmsr -p $core 0x4C2`
        v3=`$MSR_TOOS_DIR/rdmsr -p $core 0x4C3`
        v4=`$MSR_TOOS_DIR/rdmsr -p $core 0x4C4`
#        echo "Core $core :  $v1  $v2  $v3  $v4"
        printf "Core %02d : %10s %10s %10s %10s \n" $core  $v1  $v2  $v3  $v4
    done
}

function command_reset (){
    echo "Reseting all counters to 0"
    echo "Should be          0          0          0          0"

    for core in `seq 0 $MAXCORE`
    do
        $MSR_TOOS_DIR/wrmsr -p $core 0x4C1 0
        $MSR_TOOS_DIR/wrmsr -p $core 0x4C2 0
        $MSR_TOOS_DIR/wrmsr -p $core 0x4C3 0
        $MSR_TOOS_DIR/wrmsr -p $core 0x4C4 0

        v1=`$MSR_TOOS_DIR/rdmsr -p $core 0x4C1`
        v2=`$MSR_TOOS_DIR/rdmsr -p $core 0x4C2`
        v3=`$MSR_TOOS_DIR/rdmsr -p $core 0x4C3`
        v4=`$MSR_TOOS_DIR/rdmsr -p $core 0x4C4`
        printf "Core %02d : %10s %10s %10s %10s \n" $core  $v1  $v2  $v3  $v4

    done
}


# -----------------------------------------------------------------
# -----------------------------------------------------------------
if [ ! -n "$(type -t "rdmsr")" ] || [ ! -n "$(type -t "$MSR_TOOS_DIR/rdmsr")" ] ; then
    echo "# ! Install msr-tool from https://github.com/01org/msr-tools"
    echo "# ! Or set the variable MSR_TOOS_DIR"
elif [ -n "$(type -t ${command})" ] && [ "$(type -t ${command})" = function ]; then
   ${command}
else
   echo "'${cmd}' is NOT a command";
fi