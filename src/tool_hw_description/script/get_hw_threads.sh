#!/usr/bin/env bash


lscpu | grep '^CPU(s)' |  awk '{print $NF}'