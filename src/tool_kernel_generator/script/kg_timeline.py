import numpy as np
import scipy.stats as stats
import pylab as pl
import matplotlib.pyplot as plt

import sys

if len(sys.argv) < 2:
    print "You must set the input file data"
    sys.exit(1)


FILE_NAME=sys.argv[1]                #First argument is the input file

cycles = []
iteration = []

with open(FILE_NAME) as f:
    i =1
    for line in f:
        ligne = line.split()
        cycles.append(int(ligne[0]))
        iteration.append(i)
        i = i + 1


x = iteration
y = cycles

print x
plt.plot (x,y)
plt.grid(True)



plt.xlabel('Mesures')
plt.ylabel('Nombre de cycles')
plt.autoscale(enable=True, axis='both', tight=None)
plt.grid(True)
plt.show()


