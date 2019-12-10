import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# Abhishek Bhatia's data & scatter plot.
ox = np.array([  29.,   36.,    8.,   32.,   11.,   60.,   16.,  242.,   36.,
               115.,    5.,  102.,    3.,   16.,   71.,    0.,    0.,   21.,
               347.,   19.,   12.,  162.,   11.,  224.,   20.,    1.,   14.,
                 6.,    3.,  346.,   73.,   51.,   42.,   37.,  251.,   21.,
               100.,   11.,   53.,  118.,   82.,  113.,   21.,    0.,   42.,
                42.,  105.,    9.,   96.,   93.,   39.,   66.,   66.,   33.,
               354.,   16.,  602.])
oy = np.array([ 30,  47, 115,  50,  40, 200, 120, 168,  39, 100,   2, 100,  14,
               50, 200,  63,  15, 510, 755, 135,  13,  47,  36, 425,  50,   4,
               41,  34,  30, 289, 392, 200,  37,  15, 200,  50, 200, 247, 150,
              180, 147, 500,  48,  73,  50,  55, 108,  28,  55, 100, 500,  61,
              145, 400, 500,  40, 250])

x = [2016, 2015, 2014, 2013]
y = [125435.9, 54902.4, 543902.4, 542902.4]


# x = np.array(lx)
# y = np.array(ly)


fig = plt.figure()
ax=plt.gca() 
ax.scatter(x,y,c="blue",alpha=0.95,edgecolors='none', label='data')
ax.set_yscale('log')
ax.set_xscale('log')


newX = np.logspace(0, 3, base=10)  # Makes a nice domain for the fitted curves.
                                   # Goes from 10^0 to 10^3
                                   # This avoids the sorting and the swarm of lines.

# Let's fit an exponential function.  
# This looks like a line on a lof-log plot.

# Let's fit a more complicated function.
# This won't look like a line.
def myComplexFunc(x, a, b, c):
    return a * np.power(x, b) + c
popt, pcov = curve_fit(myComplexFunc, x, y)
plt.plot(newX, myComplexFunc(newX, *popt), 'g-', 
         label="({0:.3f}*x**{1:.3f}) + {2:.3f}".format(*popt))
print "Modified Exponential Fit: y = (a*(x**b)) + c"
print "\ta = popt[0] = {0}\n\tb = popt[1] = {1}\n\tc = popt[2] = {2}".format(*popt)

ax.grid(b='on')
plt.legend(loc='lower right')
plt.show()