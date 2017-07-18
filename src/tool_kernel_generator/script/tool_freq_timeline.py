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
    for line in f:
        i =1
        for word in line.split():
            cycles.append(int(word))
            iteration.append(i)
            i = i + 1


x = iteration
y = cycles

plt.plot (x,y)


plt.xlabel('Iteration')
plt.ylabel('Number of cycles')
# plt.axis([x[0], x[-1], 0, 0.000002])
plt.autoscale(enable=True, axis='both', tight=None)
plt.grid(True)
plt.show()


