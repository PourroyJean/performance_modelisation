#!/usr/bin/env bash

# ------------------------------------------------------------------
#   Author  [Jean Pourroy - jean.pourroy at g_:) _mail.com]
#
#   Description: Bash file using to start/stop uncore counters for Skylake Architecture with the lspci command
#
# ------------------------------------------------------------------
# Source: https://github.com/PourroyJean/performance_modelisation
# ------------------------------------------------------------------
# ref: bash template from
#           https://github.com/RenatGilmanov/shell-script-template/edit/master/getopts/template-commands.sh
# ref: code inspired from John Mc Calpin answers on intel forum
#           https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/595214


VERSION=1.0
USAGE="Usage: $0 {start, stop, read, reset}
             - start    setup and start the uncore counters
             - stop     stop the uncore counters
             - read     dump the  uncore counters
             - reset    set the value 0 in the uncore counters\n"


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


#Get results with:
#while sleep .1 ; do nice -n -20 ./setpci_msr.sh  | grep TOTAL | awk '{ print $4 }' | tr '\n' ' '  ; echo "" ; done

# ---- VARIABLE ----

#all busses
export mybusses=` lspci |grep -i 'System peripheral: Intel Corporation Device 2066'| cut -f 1 -d :                 | sort | uniq`
#Just my CPU
export mybusses=$(res=`rdmsr 0x300` && printf "%x" $(( ( 0x$res  >> 16 & 0xff )  )))

export myfunctions=`lspci |grep-i  "\(2042\|2046\|204A\)"| cut -f 2 -d : | cut -f 2 -d . |cut -f 1 -d ' ' | sort | uniq `
export mychannels=`seq 0 5`

#Channel             0   1   2   3   4   5
export  devices=(   0a  0a  0b  0c  0c  0d)
export  function=(  2   6   2   2   6   2)

# --- NDA INFORMATION ---
source skylake_nda_information
#This will set the following variable
#pmonunitctrl_hi
#pmonunitctrl_lo
#Counter (config)
#pmoncntrcfg_0
#pmoncntrcfg_1
#Counter (data)
#pmoncntrupper1_0#counter 0
#pmoncntrlower1_0#counter 0
#pmoncntrupper1_1#counter 1
#pmoncntrlower1_1#counter 1
#Event
#ev_read  #CAS_COUNT.RD = ENABLE | RESET | umask:b00000011 | event:0x04
#ev_write #CAS_COUNT.WR = ENABLE | RESET | umask:b00001100 | event:0x04



echo "-----------------------------------------------------------"
echo " - command:     " $command
echo " - mybusses:    " $mybusses
echo " - mydevices:   " ${devices[@]}
echo " - myfunctions: " ${function[@]}
echo " - mychannels:  " $mychannels
echo " - myeventts:   " $ev_read " " $ev_write
echo "-----------------------------------------------------------"

if [ -z $ev_read ] ; then
    echo "Error: source the Skylake information file (currently NDA), if you have it :)"
    exit
fi


# ---- MAIN ----



function command_start (){
    echo "Setting up IMC Performance Counters"
    for BUS in $mybusses   #ONE PER CPU
    do
        for CHAN in $mychannels
        do
    #       FREEZE and RESET
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_hi}=0x1
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_lo}=0x2

    #       ZERO
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrupper1_0}=0x0  #ctr0
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrlower1_0}=0x0  #ctr0
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrupper1_1}=0x0  #ctr1
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrlower1_1}=0x0  #ctr1

    #       RESET MONITORING CTRL REGISTERS
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_lo}=0x1

    #       CONFIGURE COUNTER #0 FOR CAS_COUNT.RD and #1 for CAS_COUNT_WR
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrcfg_0}=${ev_read}
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrcfg_1}=${ev_write}

    #       RELEASE COUNTERS
            setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_hi}=0x0
        done
    done
}

function command_read(){
    echo "Reading IMC CycleCounter and Programmable counters "
    sum_r=0
    sum_w=0
    for BUS in $mybusses   #ONE PER CPU
    do
        for CHAN in $mychannels
        do
        #       FREEZE
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_hi}=0x1

        #       READ RESULTS
        hi=`setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrupper1_0}`
        lo=`setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrlower1_0}`
        r=`echo -n "$((0x$hi$lo))   "`
        hi=`setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrupper1_1}`
        lo=`setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrlower1_1}`
        w=`echo -n "$((0x$hi$lo))   "`
        sum_r=$(( $sum_r + $r ))
        sum_w=$(( $sum_w + $w ))


        #       ZERO
        #command_reset


        #       RELEASE COUNTERS
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_hi}=0x0

        done
    done

    echo " TOTAL READ  = $sum_r"
    echo " TOTAL WRITE = $sum_w"
}


function command_reset(){
    echo "Reset IMC CycleCounter and Programmable counters "
    for BUS in $mybusses   #ONE PER CPU
    do
        for CHAN in $mychannels
        do

        #       ZERO
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrupper1_0}=0x0  #ctr0
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrlower1_0}=0x0  #ctr0
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrupper1_1}=0x0  #ctr1
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrlower1_1}=0x0  #ctr1

        done
    done
}

function command_stop (){
    echo "STOP IMC CycleCounter and Programmable counters "
    for BUS in $mybusses   #ONE PER CPU
    do
        for CHAN in $mychannels
        do
        #       FREEZE
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_hi}=0x1  #FREEZE
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_lo}=0x2  #RESET
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_lo}=0x1  #RESET MONITORING CTRL REGISTERS

        done
    done
}

# -----------------------------------------------------------------
# -----------------------------------------------------------------
if [ -n "$(type -t ${command})" ] && [ "$(type -t ${command})" = function ]; then
   ${command}
else
   echo "'${cmd}' is NOT a command";
fi


