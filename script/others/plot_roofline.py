#!/bin/python

#######################################################################################################################
#	This script is used to plot the roofline model for a given algorithm / node
#	./plot_roofline <bandwidth> <cpu peak> <operational intensity> <actual flop>
#		-bandwidth: The bandwidth of your node CPU <-> MEMORY (BYTE/CYCLE)
#		-cpu peak: peak flop of your CPU * number of core (SandyBridge 8DP 16SP, Haswell 16DP, 32SP) (FLOP)
#		-operational intesity of your kernel (FLOP/CYCLE)
#		-actual flops: number of flops currently reached by your program (FLOP/CYCLE)
#######################################################################################################################


## IMPORT ##
import matplotlib.pyplot as plt
from pylab import *
import matplotlib.ticker as ticker
import numpy as np
import sys

#---- Check
if len(sys.argv) != 5:
	print "#######################################################################################################################"
	print "#\tThis script is used to plot the roofline model for a given algorithm / node"
	print "#\t./plot_roofline <bandwidth> <cpu peak> <operational intensity> <actual flop>"
	print "#\t\t-bandwidth: The bandwidth of your node CPU <-> MEMORY (BYTE/CYCLE)"
	print "#\t\t-cpu peak: peak flop of your CPU * number of core (SandyBridge 8DP 16SP, Haswell 16DP, 32SP) (FLOP)"
	print "#\t\t-operational intesity of your kernel (FLOP/CYCLE)"
	print "#\t\t-actual flops: number of flops currently reached by your program (FLOP/CYCLE)"
	print "#\tExample: ./plot_roofline 24 192 4.4 5 "
	print "#######################################################################################################################"
	sys.exit()

#---- Parse parameters
BW 			= int(sys.argv[1]) 		#24
PEAK 		= int(sys.argv[2]) 		#16*12
ALGO_OI		= float(sys.argv[3]) 	#4.4
ACTU_FLOP	= float(sys.argv[4]) 		#5



#---- USEFULL VARIABLE ----
CROSSX= PEAK / BW 	#abscice croisement des deux courbes

#- BI: bandwidth curve
BIX =[0,CROSSX]
BIY= [x * BW for x in BIX[:CROSSX]]

#- PEAK: cpu peak curve
PEAKX = [CROSSX, 100]
PEAKY = [PEAK for x in PEAKX]






#---- PLOTING ----


#- Font and log scale
font = {'size'   : 16}
matplotlib.rc('font', **font)
plt.xscale('log', nonposx='clip')
plt.yscale('log', nonposy='clip')

#- Title and axes
plt.title("roof line")
plt.xlabel("Operational Intensity (FLOPS/byte)")
plt.ylabel("Performance (Giga FLOP/cycle)")

#- 2 curves BI and CPU Peak
plt.plot(BIX, 	BIY, 	linewidth=5, label="Bandwidth x Operational Intensity (BW = {} byte/cycle)".format(str(BW)))
plt.plot(PEAKX, PEAKY, 	linewidth=5, label="CPU FLOP peak ({} FLOP)".format(str(PEAK)))

#- Actual flop: red point
plt.plot(ALGO_OI, ACTU_FLOP, 'ro',markersize = 15, label="Curently: {} FLOP".format(ACTU_FLOP),  linewidth=7)


plt.axvline(x=ALGO_OI, linestyle='dashed')
plt.xticks([0.3, 10, 100, ALGO_OI, CROSSX], ["0", "10", "100", str(ALGO_OI), str(CROSSX)])

plt.axhline(y=ACTU_FLOP, linestyle='dashed')
plt.yticks([ 10, 100, 1000, ACTU_FLOP, PEAK], ["10", "100", "1000", str(ACTU_FLOP), str(PEAK)])

plt.axvline(x=CROSSX, linestyle='dashed', color='red')



plt.legend(loc="upper left", bbox_to_anchor=[.2, 0.98], ncol=1, shadow=True, title="", fancybox=True)



plt.annotate(
        "Actual performance",
        xy = (ALGO_OI, ACTU_FLOP),
        xytext = (100, 20),
        textcoords = 'offset points', ha = 'right', va = 'bottom',
        bbox = dict(boxstyle = 'round,pad=0.5', fc = 'yellow', alpha = 0.5),
        arrowprops = dict(arrowstyle = '->', connectionstyle = 'arc3,rad=0'))

plt.show()




