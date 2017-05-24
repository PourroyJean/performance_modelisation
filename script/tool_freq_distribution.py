import numpy as np
import scipy.stats as stats
import pylab as pl
import matplotlib.pyplot as plt

import sys

if len(sys.argv) < 3:
    print "You must set the input file data ans size of rectangles"
    sys.exit(1)


FILE_NAME=sys.argv[1]           #First argument is the input file
SIZE_RECT=int(sys.argv[2])           #Second the rectangle size

cycles = []

with open(FILE_NAME) as f:
    for line in f:
        for word in line.split():
            cycles.append(int(word))


x = sorted(cycles)

n, bins, patches = plt.hist(x, SIZE_RECT, normed=1, facecolor='b', alpha=0.5)


plt.xlabel('Mise')
plt.ylabel('Probabilite')
# plt.axis([x[0], x[-1], 0, 0.000002])
plt.autoscale(enable=True, axis='both', tight=None)
plt.grid(True)
plt.show()


