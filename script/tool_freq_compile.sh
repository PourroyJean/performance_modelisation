#!/bin/bash
#export PATH=`dirname $0`:$PATH

D_BUILD=$(pwd)


echo "# COMPILNG "
make  ||  exit 1

echo "# RUNNING #"
./bin/tool_freq $*





