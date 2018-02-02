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
import os.path
import argparse
import logging
import matplotlib, os



# --------------------------------------------------------------------------------------------
# ------------------------------------ GLOBAL VARIABLE INIT ----------------------------------
# --------------------------------------------------------------------------------------------
plt={}
_path_log_file      = ""            # Argument parsing
_path_image_file    = ""            # Argument parsing
_path_annotate_file = ""            # Argument parsing
_screen             = True          # Argument parsing
_start_range        = 0             # Argument parsing - default = Biggest range possible
_end_range          = sys.maxsize   # Argument parsing - default = Biggest range possible
_start_time         = 0             # Read from the first line of the perf log file
_next_y_location    = 1             # Annotation label
_dict_y_location    = {}            # Annotation label

logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)



# --------------------------------------------------------------------------------------------
# ------------------------------------ MAIN --------------------------------------------------
# --------------------------------------------------------------------------------------------

def main ():


    # -- PARSING COMMAND LINE --
    parse_parameter()

    # -- PARSE LOG FILES TO GET THE READ AND WRITE BANDWIDTH --
    timing, y_Total_read, y_Total_write = parse_values(_path_log_file)
    y_Total = np.add(y_Total_read, y_Total_write)



    # ------------------------------------ Bandwidth PLOT  ---------------------------------------


    # -- SETTINGS --
    bkgd_color      = 'black'
    text_color      = 'white'
    contour_color   = 'white'
    title_font      = {'fontname': 'Arial', 'size': '20', 'color': 'black', 'weight': 'bold',
                       'verticalalignment': 'bottom'}
    axis_font       = {'fontname': 'Arial', 'size': '14', 'weight': 'bold'}
    read_color      = '#FF8D6D'
    write_color     = '#2AD2C9'
    total_color     = '#614767'

    ## -- PLOT THE RESULTS --
    fig = plt.figure(facecolor=bkgd_color)


    logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)

    plt.plot(timing, y_Total_read,  label='Read trafic',    color=read_color,  linewidth=1.5)
    plt.plot(timing, y_Total_write, label='Write trafic',   color=write_color, linewidth=1.5)
    plt.plot(timing, y_Total,       label='Total trafic',   color=total_color, linewidth=2)


    #Border
    ax = fig.add_subplot(1, 1, 1)
    for spine in ax.spines:
        ax.spines[spine].set_color('#C6C9CA')


    plt.gcf().set_facecolor(contour_color)
    plt.xlabel("Time (second)",                 **axis_font)
    plt.ylabel("Bandwidth (Mib/second)",        **axis_font)
    plt.title ("MEMORY BANDWIDTH MONITORING",   **title_font)
    plt.legend(loc=0,
               ncol=1, borderaxespad=0.)
    plt.grid(True)




    # ------------------------------------ ANNOTATION PLOT  ----------------------------------------------


    # -- Annotation: If an annotation file was provided, use it
    if os.path.exists(_path_annotate_file):
        # For each annotation: draw a vertical line with its label and color
        with open(_path_log_file, 'r') as f:
            _start_time = int(f.readline().split(' ')[-1])


        # Parse the annotation file into content
        content = []
        with open(_path_annotate_file, 'r') as f:
            content = f.readlines()

        #Each line is formated like:     TIME |  FLAG  |  COLOR
        for x in content:
            line = x.split(' ')
            # X is the time (in second) when the event occurred after the begging of the bandwidth profiling
            # As is it useless to draw a line for events which did not occurred during the profiling we move on the next
            X = (int(line[0]) - _start_time) / 1000.0

            if X < _start_range:    # Not yet in the range  : move on
                continue
            elif X > _end_range:    # After the range       : stop
                break

            time    =  X
            flag    = (line[1])
            color   = (line[2][:-1])


            #Vertical line
            plt.axvline(x=time, linestyle='dotted', color=color)
            plt.text(time, get_y_location(flag) * (max(y_Total) / 4),
                     flag, rotation=90, color=color, weight='bold', horizontalalignment='right')




    # ------------------------------------ OUTPUT  ----------------------------------------------

    # Output: graphical or png file ?
    if _screen :
        logging.info (" Output = Screen")
        plt.show()
    else:
        logging.info (" Output = " + _path_image_file)
        fig.savefig(str(_path_image_file))


# --------------------------------------------------------------------------------------------
# -------------------------------- END OF MAIN -----------------------------------------------
# --------------------------------------------------------------------------------------------







def parse_parameter():
    global _path_log_file
    global _path_image_file
    global _path_annotate_file
    global _start_range
    global _end_range
    global _screen

    parser = argparse.ArgumentParser(description='Generate a graph based on a log file gathered with YAMB profiling tool.\n')

    parser.add_argument('-d', '--data', dest='path_log', nargs='?', type=str, default="not_set", required=True,
                        help='log file where data are stored')

    parser.add_argument('-a', '--annotate', dest='path_annotate', nargs=1, type=str, default="not_set",
                        help='This file will be used to annotate the graph')

    parser.add_argument('--range', type=parseNumList, dest='range', default=[],
                        help='Show only a specific time interval (in second). Format of RANGE should be "2.3-4.5" or "2.3-" or "-4.5" ')

    parser.add_argument('-i', '--image', dest='path_image', nargs='?', type=str, default="not_set",
                        help='Print the graph to a .png file ')

    parser.add_argument('--version', action='version', version='%(prog)s for YAMB version 1.0.0')


    # read and set parameters
    args = parser.parse_args()

    # -- LOG --
    _path_log_file = args.path_log
    if not os.path.exists(_path_log_file):
        logging.error(" Log file does not exist = " + _path_log_file)
        sys.exit(-1)
    logging.info (" Log file = " + _path_log_file)


    # -- IMAGE --
    _screen = is_screen_available ()

    if args.path_image == "not_set" and not _screen:
        # No -i option
        logging.warning (" No DISPLAY available is detected. The output will be redirected into a png file")
        _path_image_file = args.path_log + ".png"

    elif  not args.path_image:
        # Option -i
        # print "User wants an image with the same name as the log file"
        _screen = False
        _path_image_file = args.path_log + ".png"

    elif  args.path_image != "not_set" :
        # Option -i file_name
        # print "User wants an image with a specific name"
        _screen = False
        _path_image_file = str(args.path_image)



    # --ANNOTATE
    if not args.path_annotate == "not_set":
        _path_annotate_file = ''.join(str(e) for e in args.path_annotate)
        if not os.path.exists(_path_annotate_file):
            logging.warn (" Annotation file does not exists = " + _path_annotate_file)
            # _path_annotate_file = "radom3nam3suppos3dtonot3xists"
            _path_annotate_file = ""
        if os.path.exists(_path_annotate_file):
            logging.info (" Annotation file = " + _path_annotate_file)


    # --RANGE
    if args.range:
        _start_range    = args.range[0]
        _end_range      = args.range[1]
    logging.info (" Time range : " + str(_start_range) + " - " + str (_end_range == sys.maxsize and "Inf" or _end_range))


def is_screen_available ():
    global plt
    # Check if display is available
    r = os.system('python -c "import matplotlib.pyplot as plt;plt.figure()" 2&> /dev/null')
    if r != 0:
        matplotlib.use('Agg')
        import matplotlib.pyplot as plt
        return False
    else:
        import matplotlib.pyplot as plt
        return True


#Function used to parse the parameter --range
# Format of RANGE should be "2.3-4.5" or "2.3-" or "-4.5" '
def parseNumList(string):
    splt = string.split('-')

    if  not  1 <= len(splt) <= 2 :
        logging.error("'" + string + "' is not a range of number. Expected forms like '2.3-4.5' or '2.3-' or '-4.5' ")
        exit(-3)
    try:
        start = float(splt[0])
    except:
        start = 0

    try:
        end = float(splt[1])
    except:
        end=sys.maxsize

    return sorted([start, end])




def get_y_location (event):
    global _next_y_location
    global _dict_y_location

    if not event in _dict_y_location:
        _dict_y_location[event]=_next_y_location
        _next_y_location= (_next_y_location) % 3 + 1

    return _dict_y_location[event]




# -- Parse the perf log file
def parse_values(file_name):
    global _start_time
    arr = np.loadtxt(file_name, delimiter=',', usecols=(0, 1), skiprows=2)
    type = np.loadtxt(file_name, dtype=str, delimiter=',', usecols=[3])
    res_values_read = np.empty((0, 6), float)
    res_values_write = np.empty((0, 6), float)
    res_timing = np.empty([1, 0])

    for sample in range(int(arr.shape[0] / 12)):
        line_number = sample * 12

        # print "------- ", sample, " sur ", arr.shape[0]/12,  "------------ "
        # print "-------  ligne ", line_number, "------------ "

        # Read the time and append it to the final times array
        t = float(arr[line_number][0])

        # We only use data which are on the RANGE
        if t < _start_range:
            continue
        elif t > _end_range:
            break

        res_timing = np.append(res_timing, t)

        # For each imc (6) we save its value in a single row
        values = []
        for imc in range(6):
            values.append(float(arr[line_number + imc][1]))
        # We add the raw at the bottom
        if "read" in type[line_number]:
            # print "1 - read", values
            res_values_read = np.vstack((res_values_read, values))
        if "write" in type[line_number]:
            # print "1 - write", values
            res_values_write = np.vstack((res_values_write, values))

        values = []
        for imc in range(6):
            values.append(float(arr[line_number + 6 + imc][1]))
        # We add the raw at the bottom
        if "read" in type[line_number + 6]:
            # print "2 - read", values
            res_values_read = np.vstack((res_values_read, values))
        if "write" in type[line_number + 6]:
            # print "2 - write", values
            res_values_write = np.vstack((res_values_write, values))

    # print ("----- TIMING -------")
    # print (res_timing)
    # print ("----- READ -------")
    # print (res_values_read)
    # print ("------ WRITE -----")
    # print (res_values_write)


    # Reduction: add each imc to calcule the total bandwidth (read or write)
    return res_timing, \
           np.add.reduce(res_values_read,  axis=1, keepdims=True), \
           np.add.reduce(res_values_write, axis=1, keepdims=True)



if __name__=="__main__":
    main()