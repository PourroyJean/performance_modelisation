#!/usr/bin/env bash

#Get the number of physical cores

#FROM https://stackoverflow.com/questions/394230/detect-the-os-from-a-bash-script
if [[ "$OSTYPE" == "darwin"* ]]; then
    # Mac OSX
    sysctl -n hw.physicalcpu
else
    CORE_SOC=$(lscpu | grep '^Core' |  awk '{print $4}')
    CPUS=$(lscpu | grep '^Socket(s)' |  awk '{print $2}')
    echo $(($CORE_SOC * CPUS))
fi