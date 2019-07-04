import numpy as np
import scipy.stats as stats
import pylab as pl
import matplotlib.pyplot as plt

import sys

if len(sys.argv) < 2:
    print "You must set the input file data"
    sys.exit(1)


FILE_NAME=sys.argv[1]                #First argument is the input file
# SIZE_RECT=int(sys.argv[2])           #Second the rectangle size

cycles = []
iteration = []

with open(FILE_NAME) as f:
    i =1
    for line in f:
        ligne = line.split()
        # print ligne
        # print " ---- "
        # print i
        cycles.append(int(ligne[0]))
        iteration.append(i)
        i = i + 1


x = iteration
y = cycles

print x
plt.plot (x,y)
# plt.hist(y, normed=True, bins=1000)


plt.xlabel('Iteration')
plt.ylabel('Number of cycles')
# plt.axis([x[0], x[-1], 0, 0.000002])
plt.autoscale(enable=True, axis='both', tight=None)
plt.grid(True)
plt.show()


