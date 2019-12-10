import matplotlib.pyplot as plt
import numpy as np
import pylab 
from matplotlib.patches import Ellipse



x = np.array ( [1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016,2017,2018,2019])

y = np.array (  [0.10000000000000001, 0.40000000000000002, 1.0999999999999999, 2.4000000000000008, 7.9999999999999876, 14.499999999999979, 30.700000000000152, 64.600000000000421, 109.19999999999807, 218.9999999999977, 371.79999999999768, 811.90000000000293, 1695.6000000000022, 2789.7999999999979, 4949.3999999999842, 12152.599999999993, 22639.799999999974, 32434.200000000077, 58928.000000000015, 123415.90000000002, 223653.8000000001, 273763.70000000042, 362651.79999999981, 567354.00000000012,748700, 1200000, 1600000])
y = y * 1E15


y_ln = np.log(y)


n = len(x[:-4])
A = np.array(([[x[j], 1] for j in range(n)]))
B = np.array(y_ln[0:n])
A = np.array(([[x[j], 1] for j in range(n)]))
B = np.array(y_ln[0:n])
X = np.linalg.lstsq(A,B)[0]
a=X[0]; b=X[1]

# print (A)
# print (len(A))
# print (B)
# print (len(B))


n1 = 4
nrange = range (len(x) - 4, len(x))
A1 = np.array(([[x[j], 1] for j in nrange]))
B1 = np.array(y_ln[nrange[0]:nrange[-1]+1])
A1 = np.array(([[x[j], 1] for j in nrange]))
B1 = np.array(y_ln[nrange[0]:nrange[-1]+1])

# print (A1)
# print (len(A1))
# print (B1)
# print (len(A1))

X1 = np.linalg.lstsq(A1,B1)[0]
a1=X1[0]; b1=float(X1[1])

# print (X, X1)
year  = 10
year1 = 23

old_settings = np.seterr(all='ignore')
np.geterr()


print (np.exp(1000+ b), np.exp(b1))
factor  = np.exp(a  * (year )+b + 1000  ) / np.exp(b + 1000 )
factor1 = np.exp(a1 * (year1)+b1 ) / np.exp(b1)


print ("0 - Factor " + str(factor)  + " in " + str(year ))
print ("1 - Factor " + str(factor1) + " in " + str(year1))

nrange2 = range (0, len(x))
x2 = np.array(range (x[0], x[-1] + 5))
print ("x2", x)


# plt.figure()
plt.plot(x2[nrange2], np.exp(a*x2[nrange2]+b),   '--', label='Avant 2012', color = 'green')
plt.plot(x2[nrange2], np.exp(a1*x2[nrange2]+b1), '--', label='Apres 2012', color = 'red')
plt.legend(loc='upper left')


texteX = 2005
texteY = 1E17
flecheX = 2005
flecheY = 1E18


plt.annotate('x1000 tout les 11 ans',
             xy=(flecheX, flecheY), xycoords='data',
             xytext=(texteX, texteY), textcoords='data',
              arrowprops=dict(arrowstyle="->",
                            linewidth = 3.,
                            color = 'green'))

texteX1 = 2012
texteY1 = 1E19
flecheX1 = 2015
flecheY1 = 1E20*3


plt.annotate('x1000 tout les 23 ans',
             xy=(flecheX1, flecheY1), xycoords='data',
             xytext=(texteX1, texteY1), textcoords='data',
             arrowprops=dict(arrowstyle="->",
                            linewidth = 3.,
                            color = 'red'))


plt.scatter(x, y, s=100)
plt.ylabel('Puissance en FLOP/S')
plt.xlabel('Annees')
plt.title('Evolution de la puissance des 500 supercalculateurs les plus puissants \n(puissance cumulee)')
pylab.legend(loc='upper left')
plt.grid(True)


tick_val = [1E12,1E15,1E18, 1E21]
tick_lab = ['GigaFlops','PetaFlops','ExaFlops', 'ZettaFlops']
plt.yticks(tick_val, tick_lab)

plt.yscale('log')
plt.show()















