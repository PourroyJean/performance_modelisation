import numpy as np
import sys
import matplotlib.pyplot as plt
import re
import os.path
import argparse




def check_configuration ():

    if not len(sys.argv) == 2 :
            print "Usage: ./format_log.py <log_file>"
            sys.exit(-1)

    file_path=sys.argv[1]

    if not os.path.exists(file_path):
        print "File does not exist"
        sys.exit(-1)





def parse_values (file_name):
    arr = np.loadtxt(file_name, delimiter=',', usecols=(0, 1), skiprows=2 )
    type = np.loadtxt(file_name,dtype=str, delimiter=',', usecols=[3] )
    res_values_read = np.empty((0,6), float)
    res_values_write = np.empty((0,6), float)
    res_timing =np.empty([1,0])



    for sample in range (int(arr.shape[0]/12)):
        line_number = sample * 12

        # print "------- ", sample, " sur ", arr.shape[0]/12,  "------------ "
        # print "-------  ligne ", line_number, "------------ "

        #Read the time and append it to the final times array
        t=float(arr[line_number][0])
        res_timing=np.append(res_timing, t)

        #For each imc (6) we save its value in a single row
        values=[]
        for imc in range (6):
            values.append(float(arr[line_number + imc][1]))
        #We add the raw at the bottom
        if "read" in type[line_number]:
            # print "1 - read", values
            res_values_read=np.vstack((res_values_read, values))
        if "write" in type[line_number]:
            # print "1 - write", values
            res_values_write=np.vstack((res_values_write, values))


        values=[]
        for imc in range (6):
            values.append(float(arr[line_number +6  + imc][1]))
        #We add the raw at the bottom
        if "read" in type[line_number+6]:
            # print "2 - read", values
            res_values_read=np.vstack((res_values_read, values))
        if "write" in type[line_number+6]:
            # print "2 - write", values
            res_values_write=np.vstack((res_values_write, values))

    # print ("----- TIMING -------")
    # print (res_timing)
    # print ("----- READ -------")
    # print (res_values_read)
    # print ("------ WRITE -----")
    # print (res_values_write)


    # sys.exit()

    #Reduction: add each imc to calcule the total bandwidth (read or write)
    return res_timing, np.add.reduce (res_values_read ,axis=1,keepdims=True), np.add.reduce (res_values_write ,axis=1,keepdims=True)







#Check and some initialisation

check_configuration()
file_path=sys.argv[1]

np_array_read = np.empty((0,6), float)
timing=np.empty([1,0])


## PARSE LOG FILES TO GET THE READ AND WRITE BANDWIDTH ##
timing,y_Total_read, y_Total_write   = parse_values(file_path)
# print ("----- TIMING -------")
# print timing
# print ("----- READ -------")
# print y_Total_read
# print y_Total_read.shape
# print ("------ WRITE -----")
# print y_Total_write
# print y_Total_write.shape



## CALCULATING THE TOTAL BANDWIDTH
y_Total    = np.add (y_Total_read,y_Total_write)


## PLOT THE RESULTS

bkgd_color='black'
text_color='white'

fig = plt.figure(facecolor=bkgd_color)

ax = fig.add_subplot(1, 1, 1)
for spine in ax.spines:
    ax.spines[spine].set_color('#C6C9CA')

plt.plot (timing,y_Total_read,  label='Read trafic',  color='#FF8D6D', linewidth=1.5)
plt.plot (timing,y_Total_write, label='Write trafic', color='#2AD2C9', linewidth=1.5)
plt.plot (timing,y_Total,       label='Total trafic', color='#614767', linewidth=2)

# option
title_font = {'fontname':'Arial', 'size':'20', 'color':'black', 'weight':'bold',
              'verticalalignment':'bottom'} # Bottom vertical alignment for more space
axis_font = {'fontname':'Arial', 'size':'14', 'weight':'bold'}

plt.gcf().set_facecolor('white')
plt.xlabel("Time (second)", **axis_font)
plt.ylabel("Bandwidth (Mib/second)", **axis_font)
plt.title ("MEMORY BANDWIDTH MONITORING", **title_font)
plt.legend(loc=0,
           ncol=1, borderaxespad=0.)
plt.grid(True)



plt.show()



