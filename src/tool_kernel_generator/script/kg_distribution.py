import numpy as np
import pylab as pl
import matplotlib.pyplot as plt

import sys

if len(sys.argv) < 3:
    print "You must set the input file data ans size of rectangles"
    sys.exit(1)


FILE_NAME=sys.argv[1]           #First argument is the input file
SIZE_RECT=int(sys.argv[2])       #Second the rectangle size

cycles = []

with open(FILE_NAME) as f:
    for line in f:
        ligne = line.split()
        cycles.append(int(ligne[0]))

x = sorted(cycles)

n, bins, patches = plt.hist(x, SIZE_RECT, normed=False, facecolor='b', alpha=0.5)


plt.xlabel('Number of cycle')
plt.ylabel('Distribution')
plt.autoscale(enable=True, axis='both', tight=None)
plt.grid(True)
plt.show()


