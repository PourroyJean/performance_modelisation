#!/usr/bin/env bash

sudo opcontrol --shutdown
sudo opcontrol --init
sudo opcontrol --reset
sudo opcontrol --setup --no-vmlinux #--event=CPU_CLK_UNHALTED:1000000 --event=INST_RETIRED:1000000
sudo opcontrol --save=junk
sudo opcontrol --start
