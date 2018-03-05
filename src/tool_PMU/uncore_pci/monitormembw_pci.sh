#!/usr/bin/env bash



# ---- COMMAND ----
#SETUP_COUNTER="true"
#READ_COUNTER="true"
STOP_COUNTER="true"

#Get results with:
#while sleep .1 ; do nice -n -20 ./setpci_msr.sh  | grep TOTAL | awk '{ print $4 }' | tr '\n' ' '  ; echo "" ; done

# ---- VARIABLE ----
#all busses
mybusses=` lspci |grep -i 'System peripheral: Intel Corporation Device 2066'| cut -f 1 -d :                 | sort | uniq`
#Just my CPU
mybusses=$(res=`rdmsr 0x300` && printf "%x" $(( ( 0x$res  >> 16 & 0xff )  )))

myfunctions=`lspci |grep-i  "\(2042\|2046\|204A\)"| cut -f 2 -d : | cut -f 2 -d . |cut -f 1 -d ' ' | sort | uniq `
mychannels=`seq 0 5`

#Channel    0   1   2   3   4   5
devices=(   0a  0a  0b  0c  0c  0d)
function=(  2   6   2   2   6   2)

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



# Dump all performance counter event select registers on all cores
if [ "$SETUP_COUNTER" == "true" ]
then
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
fi


if [ "$READ_COUNTER" == "true" ]
then
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
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrupper1_0}=0x0  #ctr0
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrlower1_0}=0x0  #ctr0
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrupper1_1}=0x0  #ctr1
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmoncntrlower1_1}=0x0  #ctr1

        #       RELEASE COUNTERS
        setpci -s ${BUS}:${devices[CHAN]}.${function[CHAN]} ${pmonunitctrl_hi}=0x0

        done
    done

    echo " TOTAL READ  = $sum_r"
    echo " TOTAL WRITE = $sum_w"

fi

if [ "$STOP_COUNTER" == "true" ]
then
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


fi



