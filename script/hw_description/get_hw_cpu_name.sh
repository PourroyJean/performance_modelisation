#!/usr/bin/env bash
lscpu | grep 'name' |  awk '{ for (i=3; i<=NF; i++) { printf $i ;  printf " " } }'