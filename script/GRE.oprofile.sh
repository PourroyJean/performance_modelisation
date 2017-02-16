#!/usr/bin/env bash


TOP=$(pwd)
BIN=$TOP/$1

SCRIPT_DIR="`dirname \"$0\"`"
SCRIPT_DIR="`( cd \"$SCRIPT_DIR\" && pwd )`"

echo "$SCRIPT_DIR"


# COMMAND
#./GRE.oprofile.sh
#grep -v "^ " op2 | more
#objdump -d  bsmbench_compute_1cpu  > OBJ.18
#./dmlv3 OBJ.18 op2 > op4.suivant




$SCRIPT_DIR/GRE.jini.sh

echo "Lancement en background" ; $BIN &

#./GRE.obj.sh $BIN
