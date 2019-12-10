import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

#plt.style.use('ggplot')

# Initial function given
def f(x):
    return 4.0 / (1 + x**2)


# Definite integral of the function from a to b
def F(a,b):
    if a > b:
        raise ValueError('b must be greater than a')
    elif a == b:
        return 0
    else:
        return (b**3-a**3)/3


# Approximating function using numerical methods:
    # rectangular
    # trapezoidal
def approximateNumerical(a,b,points=10,error=False,mod='rectangular',plt_data=False):
    if points < 2:
        raise ValueError('Number of points must be greater than 2')
    if a == b:
        return 0
    n = np.linspace(a,b,points)
    
    plot_dat(a,b,points,mod=mod)
        
    return partialSum,e

# Plotting function for a visual representation
def plot_dat(a,b,points,mod='rectangular'):

    n = np.linspace(a,b,points)
    plt.plot(n,f(n),color='red', linewidth=3.0)
    plt.xlabel('x')
    plt.ylabel('y = 4.0 / (1 + x*x) ')
    plt.title('Approximation de pi')
    plt.axis((0,1.1,0,5))

    # plt.text(0.07, 2,   'hauteur',color = "green", weight = 'bold', rotation='vertical')
    # plt.text(0.12, 4,  'largeur',color = "green", weight = 'bold', )

    plt.text(0.28, 2,   'hauteur',color = "green", weight = 'bold', rotation='vertical')
    plt.text(0.45, 3.3,  'largeur',color = "green", weight = 'bold', )

    #coordonee bas gauche du rectangle
    # someX, someY = 0.105, 0
    someX, someY = 0.33, 0

    currentAxis = plt.gca()
    currentAxis.add_patch(Rectangle((someX, someY), 0.34, 3.20, facecolor="grey"))

    if mod == 'rectangular':    
        for i in range(1,len(n)):
            c = n[i-1]
            d = n[i]
            plt.plot([c,c],[0,f((c+d)/2)],color='blue', linewidth=2.0)
            plt.plot([d,d],[0,f((c+d)/2)],color='blue', linewidth=2.0)
            plt.plot([c,d],[f((c+d)/2),f((c+d)/2)],color='blue', linewidth=2.0)
        plt.show()
    if mod == 'trapezoidal':
        for i in range(1,len(n)):
            c = n[i-1]
            d = n[i]
            plt.plot([d,d],[0,f(d)],color='blue')
            plt.plot([c,c],[0,f(c)],color='blue')
            plt.plot([c,d],[f(c),f(d)],color='blue')
        plt.show()
    return 0
    
    
# Approximate area with a given precision
def approxGivenPrecision(a,b,error=0.5,mod='rectangular',printf=False):
    e = 100
    p = 5
    itMax = 500
    it = 0
    while abs(e) > error:
        area,e = approximateNumerical(a,b,mod=mod,points=p)
        p += 10
        it += 1
        if printf:
            print('Approximating using '+mod+' rule with:',p,'points, percentage error:',e,'%')
        if it > itMax:
            print('Number of iterations exceeded: '+str(itMax))
            break
    return area

#-----------------------------Run the program----------------------------------

# Initial parameters:
    # xmin = m
    # xmax = M
    # points used = p

m = 0
M = 1
# p = 20
p = 4


print('Actual area:',F(m,M))
print('Approximation:',approximateNumerical(m,M,p,error=True,plt_data=True))
print('Approximation:',approximateNumerical(m,M,p,mod='trapezoidal',error=True,plt_data=True))
print('#############################################################')
print(approxGivenPrecision(m,M,error=0.05,printf=True))
print(approxGivenPrecision(m,M,error=0.05,mod='trapezoidal',printf=True))




















