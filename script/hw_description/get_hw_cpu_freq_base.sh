#!/usr/bin/env bash

lscpu | grep '^CPU MHz' |  awk '{print $3}'