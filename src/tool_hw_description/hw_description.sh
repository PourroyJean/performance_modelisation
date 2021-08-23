#!/usr/bin/bash


PATH_TOOL="${BASH_SOURCE[0]}";
if ([ -h "${PATH_TOOL}" ]) then
  while([ -h "${PATH_TOOL}" ]) do PATH_TOOL=`readlink "${PATH_TOOL}"`; done
fi
pushd . > /dev/null
cd `dirname ${PATH_TOOL}` > /dev/null
PATH_TOOL=`pwd`;
popd  > /dev/null

PATH_SCRIPT=$PATH_TOOL/script
KG=$PATH_TOOL/../tool_kernel_generator/kg


echo "Current execution: $PATH_TOOL"
echo "  - Your path:             $(pwd)"
echo "  - hw_description path:   $PATH_TOOL"
echo "  - hw_description script: $PATH_SCRIPT"


echo ""
echo "############################### "
echo "#########  GENERAL ############ "
echo "############################### "
echo ""

PM_HW_CPU_GENERATION=$($PATH_SCRIPT/get_hw_cpu_generation)
PM_HW_CPU_NAME=$($PATH_SCRIPT/get_hw_cpu_name)
PM_HW_SOCKET=$($PATH_SCRIPT/get_hw_socket)
PM_HW_CPU_CORE_PHY=$($PATH_SCRIPT/get_hw_core_phy)
PM_HW_CPU_THREADS=$($PATH_SCRIPT/get_hw_threads)
PM_HW_CPU_FREQ_BASE=$($PATH_SCRIPT/get_hw_cpu_freq_base)
PM_HW_CPU_HAS_TURBO=$($PATH_SCRIPT/get_hw_cpu_has_turbo)

echo " - PM_HW_CPU_GENERATION   $PM_HW_CPU_GENERATION"
echo " - PM_HW_CPU_NAME         $PM_HW_CPU_NAME"
echo " - PM_HW_SOCKET           $PM_HW_SOCKET"
echo " - PM_HW_CPU_CORE_PHY     $PM_HW_CPU_CORE_PHY"
echo " - PM_HW_CPU_THREADS      $PM_HW_CPU_THREADS"
echo " - PM_HW_CPU_FREQ_BASE    $PM_HW_CPU_FREQ_BASE"
echo " - PM_HW_CPU_HAS_TURBO    $PM_HW_CPU_HAS_TURBO"
echo ""
echo ""



echo "############################### "
echo "#########  COMPUTE  ########### "
echo "############################### "


if ! command -v $KG &> /dev/null
then
  echo "Kernel Generator could not be found"
else
  echo -n "Launching Kernel generator : "

  OPERATION='mmmmmmm'
  echo -n "MUL SINGLE PRECISION "
  PM_HW_ALU_32_MUL_SP=NO #NOT YET AVAILABLE
  PM_HW_ALU_64_MUL_SP=` $KG -O $OPERATION -W 64  -P single | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_128_MUL_SP=`$KG -O $OPERATION -W 128 -P single | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_256_MUL_SP=`$KG -O $OPERATION -W 256 -P single | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_512_MUL_SP=`$KG -O $OPERATION -W 512 -P single | grep "^IPC" |  awk '{print $2}'`
  echo -n " ... MUL DOUBLE PRECISION "
  PM_HW_ALU_32_MUL_DP=NO #NOT YET AVAILABLE
  PM_HW_ALU_64_MUL_DP=` $KG -O $OPERATION -W 64  -P double | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_128_MUL_DP=`$KG -O $OPERATION -W 128 -P double | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_256_MUL_DP=`$KG -O $OPERATION -W 256 -P double | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_512_MUL_DP=`$KG -O $OPERATION -W 512 -P double | grep "^IPC" |  awk '{print $2}'`

  OPERATION='fffffff'
  echo -n " ... FMA SINGLE PRECISION "
  PM_HW_ALU_32_FMA_SP=NO #NOT YET AVAILABLE
  PM_HW_ALU_64_FMA_SP=` $KG -O $OPERATION -W 64  -P single | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_128_FMA_SP=`$KG -O $OPERATION -W 128 -P single | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_256_FMA_SP=`$KG -O $OPERATION -W 256 -P single | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_512_FMA_SP=`$KG -O $OPERATION -W 512 -P single | grep "^IPC" |  awk '{print $2}'`
  echo -n " ... FMA DOUBLE PRECISION "
  PM_HW_ALU_32_FMA_DP=NO #NOT YET AVAILABLE
  PM_HW_ALU_64_FMA_DP=` $KG -O $OPERATION -W 64  -P double | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_128_FMA_DP=`$KG -O $OPERATION -W 128 -P double | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_256_FMA_DP=`$KG -O $OPERATION -W 256 -P double | grep "^IPC" |  awk '{print $2}'`
  PM_HW_ALU_512_FMA_DP=`$KG -O $OPERATION -W 512 -P double | grep "^IPC" |  awk '{print $2}'`

  echo ""
  echo ""
  echo " -- Simple Precision"
  echo "    - PM_HW_ALU_32_MUL_SP    $PM_HW_ALU_32_MUL_SP"
  echo "    - PM_HW_ALU_64_MUL_SP    $PM_HW_ALU_64_MUL_SP"
  echo "    - PM_HW_ALU_128_MUL_SP   $PM_HW_ALU_128_MUL_SP"
  echo "    - PM_HW_ALU_256_MUL_SP   $PM_HW_ALU_256_MUL_SP"
  echo "    - PM_HW_ALU_512_MUL_SP   $PM_HW_ALU_512_MUL_SP"
  echo ""
  echo "    - PM_HW_ALU_32_FMA_SP    $PM_HW_ALU_32_FMA_SP"
  echo "    - PM_HW_ALU_64_FMA_SP    $PM_HW_ALU_64_FMA_SP"
  echo "    - PM_HW_ALU_128_FMA_SP   $PM_HW_ALU_128_FMA_SP"
  echo "    - PM_HW_ALU_256_FMA_SP   $PM_HW_ALU_256_FMA_SP"
  echo "    - PM_HW_ALU_512_FMA_SP   $PM_HW_ALU_512_FMA_SP"
  echo ""
  echo " -- Double Precision"
  echo "    - PM_HW_ALU_32_MUL_DP    $PM_HW_ALU_32_MUL_DP"
  echo "    - PM_HW_ALU_64_MUL_DP    $PM_HW_ALU_64_MUL_DP"
  echo "    - PM_HW_ALU_128_MUL_DP   $PM_HW_ALU_128_MUL_DP"
  echo "    - PM_HW_ALU_256_MUL_DP   $PM_HW_ALU_256_MUL_DP"
  echo "    - PM_HW_ALU_512_MUL_DP   $PM_HW_ALU_512_MUL_DP"
  echo ""
  echo "    - PM_HW_ALU_32_FMA_DP    $PM_HW_ALU_32_FMA_DP"
  echo "    - PM_HW_ALU_64_FMA_DP    $PM_HW_ALU_64_FMA_DP"
  echo "    - PM_HW_ALU_128_FMA_DP   $PM_HW_ALU_128_FMA_DP"
  echo "    - PM_HW_ALU_256_FMA_DP   $PM_HW_ALU_256_FMA_DP"
  echo "    - PM_HW_ALU_512_FMA_DP   $PM_HW_ALU_512_FMA_DP"
  echo ""
fi

#TODO
#echo "#########  MEMORY  ############ "
#echo "PM_HW_MEMORY_CACHE_LINE_SIZE="
#echo "PM_HW_MEMORY_L1_D_SIZE="
#echo "PM_HW_MEMORY_L1_I_SIZE="
#echo "PM_HW_MEMORY_L1_LATENCY="
#echo "PM_HW_MEMORY_L1_BW="
#echo "PM_HW_MEMORY_L2_SIZE="
#echo "PM_HW_MEMORY_L2_LATENCY="
#echo "PM_HW_MEMORY_L2_BW="
#echo "PM_HW_MEMORY_L3_SIZE="
#echo "PM_HW_MEMORY_L3_LATENCY="
#echo "PM_HW_MEMORY_L3_BW="
#echo "PM_HW_MEMORY_LLC_SIZE="
#echo "PM_HW_MEMORY_LLC_LATENCY="
#echo "PM_HW_MEMORY_LLC_BW="