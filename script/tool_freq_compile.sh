#!/bin/bash
#export PATH=`dirname $0`:$PATH

D_BUILD=$(pwd)


make  ||  exit 1
echo "##"
echo "# RUNNING #"
echo "##"
echo ""
./bin/tool_freq




