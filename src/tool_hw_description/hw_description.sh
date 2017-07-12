#!/usr/bin/bash



#Get Script source: https://stackoverflow.com/questions/59895/getting-the-source-directory-of-a-bash-script-from-within
#


PATH_TOOL="${BASH_SOURCE[0]}";
if ([ -h "${PATH_TOOL}" ]) then
  while([ -h "${PATH_TOOL}" ]) do PATH_TOOL=`readlink "${PATH_TOOL}"`; done
fi
pushd . > /dev/null
cd `dirname ${PATH_TOOL}` > /dev/null
PATH_TOOL=`pwd`;
popd  > /dev/null


PATH_SCRIPT=$PATH_TOOL/script
TOOL_FREQ=$PATH_TOOL/tool_freq/tool_freq


echo "Current execution: $PATH_TOOL"
echo "  - Your path:             $(pwd)"
echo "  - hw_description path:   $PATH_TOOL"
echo "  - hw_description script: $PATH_SCRIPT"


echo "#########  GENERAL ############ "


export PM_HW_CPU_GENERATION=$($PATH_SCRIPT/get_hw_cpu_generation)

export PM_HW_CPU_NAME=$($PATH_SCRIPT/get_hw_cpu_name)

export PM_HW_SOCKET=$($PATH_SCRIPT/get_hw_socket)

export PM_HW_CPU_CORE_PHY=$($PATH_SCRIPT/get_hw_core_phy)

export PM_HW_CPU_THREADS=$($PATH_SCRIPT/get_hw_threads)

export PM_HW_CPU_FREQ_BASE=$($PATH_SCRIPT/get_hw_cpu_freq_base)

export PM_HW_CPU_HAS_TURBO=$($PATH_SCRIPT/get_hw_cpu_has_turbo)






echo "####  FLOATING POINT UNIT #####"

echo "--------- operation per cycle ----------"

$TOOL_FREQ | grep IPC |  awk '{print $2}'

OPERATION='mmmmmmm'

echo "# MUL SINGLE PRECISION "
export PM_HW_ALU_32_MUL_SP= #NOT YET AVAILABLE
export PM_HW_ALU_64_MUL_SP=` $TOOL_FREQ -O $OPERATION -W 64  -P single | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_128_MUL_SP=`$TOOL_FREQ -O $OPERATION -W 128 -P single | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_256_MUL_SP=`$TOOL_FREQ -O $OPERATION -W 256 -P single | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_512_MUL_SP=`$TOOL_FREQ -O $OPERATION -W 512 -P single | grep IPC |  awk '{print $2}'`

echo "# MUL DOUBLE PRECISION "
export PM_HW_ALU_32_MUL_DP= #NOT YET AVAILABLE
export PM_HW_ALU_64_MUL_DP=` $TOOL_FREQ -O $OPERATION -W 64  -P double | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_128_MUL_DP=`$TOOL_FREQ -O $OPERATION -W 128 -P double | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_256_MUL_DP=`$TOOL_FREQ -O $OPERATION -W 256 -P double | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_512_MUL_DP=`$TOOL_FREQ -O $OPERATION -W 512 -P double | grep IPC |  awk '{print $2}'`



OPERATION='fffffff'

echo "# FMA SINGLE PRECISION "
export PM_HW_ALU_32_FMA_SP= #NOT YET AVAILABLE
export PM_HW_ALU_64_FMA_SP=` $TOOL_FREQ -O $OPERATION -W 64  -P single | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_128_FMA_SP=`$TOOL_FREQ -O $OPERATION -W 128 -P single | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_256_FMA_SP=`$TOOL_FREQ -O $OPERATION -W 256 -P single | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_512_FMA_SP=`$TOOL_FREQ -O $OPERATION -W 512 -P single | grep IPC |  awk '{print $2}'`

echo "# FMA DOUBLE PRECISION "
export PM_HW_ALU_32_FMA_DP= #NOT YET AVAILABLE
export PM_HW_ALU_64_FMA_DP=` $TOOL_FREQ -O $OPERATION -W 64  -P double | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_128_FMA_DP=`$TOOL_FREQ -O $OPERATION -W 128 -P double | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_256_FMA_DP=`$TOOL_FREQ -O $OPERATION -W 256 -P double | grep IPC |  awk '{print $2}'`
export PM_HW_ALU_512_FMA_DP=`$TOOL_FREQ -O $OPERATION -W 512 -P double | grep IPC |  awk '{print $2}'`


echo "#########  MEMORY  ############ "

export PM_HW_MEMORY_L1_D_SIZE=
export PM_HW_MEMORY_L1_I_SIZE=
export PM_HW_MEMORY_L1_LATENCY=
export PM_HW_MEMORY_L1_BW=

export PM_HW_MEMORY_L2_SIZE=
export PM_HW_MEMORY_L2_LATENCY=
export PM_HW_MEMORY_L2_BW=

export PM_HW_MEMORY_L3_SIZE=
export PM_HW_MEMORY_L3_LATENCY=
export PM_HW_MEMORY_L3_BW=

export PM_HW_MEMORY_LLC_SIZE=
export PM_HW_MEMORY_LLC_LATENCY=
export PM_HW_MEMORY_LLC_BW=



#Updata the data file: hw_data.sh
source $PATH_TOOL/hw_update.sh





