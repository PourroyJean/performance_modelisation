import numpy as np
import sys
import matplotlib.pyplot as plt




def parse_values (file_name):
    arr = np.loadtxt(file_name, delimiter=',', usecols=(0, 1) )
    res_values = np.empty((0,6), float)
    res_timing =np.empty([1,0])

    for seconde in range (int(arr.shape[0]/6)):
        #Read the time and append it to the final times array
        t=float(arr[seconde*6 ][0])
        res_timing=np.append(res_timing, t)

        #For each imc (6) we save its value in a single row
        values=[]
        for imc in range (6):
            values.append(float(arr[seconde*6 + imc][1]))

        #We add the raw at the bottom
        res_values=np.vstack((res_values, values))

    # print ("----------------")
    # print (res_timing)
    # print (res_values)
    # print ("----------------")

    #Reduction: add each imc to calcule the total bandwidth (read or write)
    return res_timing, np.add.reduce (res_values ,axis=1,keepdims=True)




#Init
np_array_read = np.empty((0,6), float)
timing=np.empty([1,0])

## PARSE BOTH FILES AND GET THE READ AND WRITE BANDWIDTH ##
timing,y_Total_read   = parse_values('log_64gb_read')
timing,y_Total_write  = parse_values('log_64gb_write')

## CALCULATING THE TOTAL BANDWIDTH
y_Total    = np.add (y_Total_read,y_Total_write)


## PLOT THE RESULTS
plt.plot (timing,y_Total_read,  label='Read trafic',  color='b')
plt.plot (timing,y_Total_write, label='Write trafic', color='r')
plt.plot (timing,y_Total,       label='Total trafic', color='g', linewidth=2,)

# option
plt.xlabel("Time (second)")
plt.ylabel("Bandwidth (Mib/second)")
plt.title ("Bandwidth monitoring")
plt.legend(loc=0,
           ncol=1, borderaxespad=0.)
plt.grid(True)



plt.show()



