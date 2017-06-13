#!/usr/bin/env bash

lscpu | grep '^Core' |  awk '{print $4}'
