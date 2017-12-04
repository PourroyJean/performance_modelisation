


import numpy as np
import sys
import matplotlib.pyplot as plt


arr = np.loadtxt('log_64gb_read', delimiter=',', usecols=(0, 1) )


times=np.empty([1,0])
res_read = np.empty((0,6), float)
res_write=[]
# new_read_row = []
new_write_row= []

# sys.exit()
#




for seconde in range (int(arr.shape[0]/6)):
    #Read a new raw
    new_read_row= np.empty((0,1), float)
    #Read the time and append it to the final times array
    t=float(arr[seconde*6 ][0])
    times=np.append(times, t)

    #For each imc (6) we save its value in a single row
    values=[]
    for imc in range (6):
        values.append(float(arr[seconde*6 + imc][1]))

    #We add the raw at the bottom
    res_read=np.vstack((res_read, values))


print ("----------------")
print (times)
print (res_read)
print ("----------------")


print (res_read[:,0])

sys.exit()


x = times
y_imc0 = res_read[:,0]
y_imc1 = res_read[:,1]
y_imc2 = res_read[:,2]
y_imc3 = res_read[:,3]
y_imc4 = res_read[:,4]
y_imc5 = res_read[:,5]
plt.plot (x,y_imc0)
plt.plot (x,y_imc1)
plt.plot (x,y_imc2)
plt.plot (x,y_imc3)
plt.plot (x,y_imc4)
plt.plot (x,y_imc5)
plt.show()




sys.exit()
