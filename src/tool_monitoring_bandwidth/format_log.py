# ------------------------------------------------------------------
#                      YAMB Profiling Tool
#
#             Yet Another Memory Bandwidth Profiling Tool
# ------------------------------------------------------------------
#
#   [Jean Pourroy]
#   [Frederic Ciesielski]
#   [Patrick Demichel]
# ------------------------------------------------------------------
# Source: https://github.com/PourroyJean/performance_modelisation
# ------------------------------------------------------------------


import numpy as np
import sys
import re
import os.path
import argparse
import matplotlib,os



# Check if display is available
r = os.system('python -c "import matplotlib.pyplot as plt;plt.figure()" 2&> /dev/null')
if r != 0:
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
else:
    import matplotlib.pyplot as plt



def check_color(color):
    try:
        # Converting 'deep sky blue' to 'deepskyblue'
        color = color.replace(" ", "")
        Color(color)
        # if everything goes fine then return True
        return color
    except ValueError: # The color code was not found
        return color



# sys.exit(-1)
def parse_parameter ():
    global path_log_file
    global path_image_file
    global path_annotate_file

    parser = argparse.ArgumentParser(description='Generate a graph based on a log file gathered with YAMB profiling tool.')
    # parser.add_argument('path_log',
    #                     help='log file where data are stored')

    parser.add_argument('-d', '--data', dest='path_log', nargs='?', type=str, default="not_set",
                        help='log file where data are stored')

    parser.add_argument('-i', '--image', dest='path_image', nargs='?', type=str, default="not_set",
                        help='Print the graph to a .png file ')

    # parser.add_argument('-k', '--kimage', dest='kpath_image', nargs='?', type=str, default="not_set",
    #                     help='Print the grakk ')

    parser.add_argument('-a', '--annotate', dest='path_annotate', nargs=1, type=str, default="not_set",
                        help='This file will be used to annotate the graph')

    parser.add_argument('--version', action='version', version='%(prog)s 0.2')

    # read and set parameters
    args = parser.parse_args()

    #-- LOG --
    path_log_file = args.path_log
    if not os.path.exists(path_log_file):
        print "File does not exist: " + path_log_file
        sys.exit(-1)

    #-- IMAGE --
    if args.path_image == "not_set":
        # print "User wants a graphical output"
        path_image_file=""
    elif not args.path_image:
        # print "User wants an image with the same name as the log file"
            path_image_file=args.path_log+".png"
    else:
        # print "User wants an image with a specific name"
        path_image_file=args.path_image


    #--ANNOTATE
    if not args.path_annotate == "not_set":
        path_annotate_file= ''.join(str(e) for e in args.path_annotate)
        if not os.path.exists(path_annotate_file):
            print "Annotation file does not exist: " + path_annotate_file
            path_annotate_file= "ezajjkalz"
            sys.exit(-2)
        # print "Annotate file specified " + path_annotate_file







def parse_values (file_name):
    global start_time
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




#--------------------------------------------------------------------------------------------
#--------------------------------------------------------------------------------------------
#--------------------------------------------------------------------------------------------


#Some init
path_log_file   = ""
path_image_file = ""
path_annotate_file= ""
start_time = 0
np_array_read = np.empty((0,6), float)
timing=np.empty([1,0])


#-- PARSING COMMAND LINE --
parse_parameter()


##-- PARSE LOG FILES TO GET THE READ AND WRITE BANDWIDTH --
timing,y_Total_read, y_Total_write   = parse_values(path_log_file)
# print ("----- TIMING -------")
# print timing
# print ("----- READ -------")
# print y_Total_read
# print y_Total_read.shape
# print ("------ WRITE -----")
# print y_Total_write
# print y_Total_write.shape
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



#-- Annotation: If an annotation file was provided, use it
print path_annotate_file
if os.path.exists(path_annotate_file):
    print "GRAPH WILL BE ANNOTATED"

    #Parse the annotation file into content
    content=[]
    with open(path_annotate_file, 'r') as f:
        content = f.readlines ()
    print path_annotate_file + " " + str(content)
    time  = [int(x.split(' ')[0]) for x in content]
    flag  = [x.split(' ')[1] for x in content]
    color = [x.split(' ')[2][:-1] for x in content]

    print time
    #For each annotation: draw a vertical line with its label and color
    with open(path_log_file, 'r') as f:
        start_time = int(f.readline().split(' ')[-1])

    for i in range (len(time)):
        print str(time[i]) + " - " + flag[i] + " - " + color[i]
        X=(time[i]-start_time)/1000.0
        print "       " + str(time[i]) + " - " + str(start_time) + " = " + str( X)
        plt.axvline(x=X, linestyle='dashed', color='#425563')
        try:
            plt.text(X-0.1,max(y_Total)/2,flag[i],rotation=90, color=color[i], weight='bold')
        except:
            print "Something went wrong with annotation file: check the name of the color"




#Output: graphical or png file ?
if path_image_file == "":
    plt.show()
else:
    fig.savefig (str(path_image_file))

