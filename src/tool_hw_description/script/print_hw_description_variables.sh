#!/usr/bin/env bash


for var in  ${!PM_*} ; do
cmd="echo \$$var"
val=`eval $cmd`
echo $var $val

done

