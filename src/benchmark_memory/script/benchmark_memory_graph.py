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
from math import log, sqrt, pow, fabs

# --------------------------------------------------------------------------------------------
# ------------------------------------ GLOBAL VARIABLE INIT ----------------------------------
# --------------------------------------------------------------------------------------------
# plt = {}
_fileLog_mem = ""  # Argument parsing
_fileLog_cache = ""  # Argument parsing
_path_image_file = ""  # Argument parsing
_path_annotate_file = ""  # Argument parsing
_screen = True  # Argument parsing
_start_range = 0  # Argument parsing - default = Biggest range possible
_end_range = sys.maxsize  # Argument parsing - default = Biggest range possible
_start_time = 0  # Read from the first line of the perf log file
_next_y_location = 1  # Annotation label
_dict_y_location = {}  # Annotation label

logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)


# --------------------------------------------------------------------------------------------
# ------------------------------------ MAIN --------------------------------------------------
# --------------------------------------------------------------------------------------------

def main():
    is_screen_available()

    # -- PARSING COMMAND LINE --
    parse_parameter()



    # -- OPEN LOG FILE --
    # we check if the last line is full (could not be if the program was stopped)

    file_txt = open(_fileLog_mem, 'rt').readlines()
    len_first = len(file_txt[0])
    len_last = len(file_txt[-1])

    log_file_array = 1
    if (len_first > len_last):
        log_file_array = np.loadtxt(file_txt[:-1], delimiter=',', dtype='float')
    else:
        log_file_array = np.loadtxt(file_txt, delimiter=',', dtype='float')


    # -- PARSING LOG FILE --

    # 1st step: recover the stride --> the first line
    stride_array = log_file_array[0].astype(int)[1:]
    print (" --- Stride :" + str(stride_array))
    log_file_array = np.delete(log_file_array, 0, axis=0)

    # 2nd step: recover the data set size --> the first column
    x_value_dataset_size = log_file_array[0:log_file_array.shape[0], 0].astype(int)
    print (" ---  Data set (in bit) : " + str(x_value_dataset_size))

    # 3nd step: get the values and generate the plot
    fig, ax = plt.subplots()  # create figure and axes
    #for each stride we draw a line
    for i in range(0, len(stride_array)):
        y_value = log_file_array[:, i+1]  # get the column for the current stride
        y_value[y_value == 0] = np.nan  # replace 0 by nan to not plot not existing values
        ax.plot(x_value_dataset_size, y_value, label=str(stride_array[i]), linewidth=1)




    ## -- PLOT THE RESULTS --

    title_font = {'fontname': 'Arial', 'size': '20', 'color': 'black', 'weight': 'bold',
                  'verticalalignment': 'bottom'}
    axis_font = {'fontname': 'Arial', 'size': '14', 'weight': 'bold'}
    contour_color = 'white'


    plt.xlabel("Data set size", **axis_font)
    plt.ylabel("Bandwidth (GB/s)", **axis_font)
    plt.title("DML_MEM", **title_font)

    plt.xscale('log')
    ax.legend(title="Size of stride (MB)", loc=0,
              ncol=2, borderaxespad=0.)
    plt.gcf().set_facecolor(contour_color)

    ax = fig.add_subplot(1, 1, 1)
    for spine in ax.spines:
        ax.spines[spine].set_color('#C6C9CA')

    plt.grid(True)

    # https://en.wikichip.org/wiki/intel/xeon_gold/6148#Cache
    #Input data are in kilobyte
    cache_size = [32000,  1000000, 28000000]
    cache_hauteur = [50,  150, 150]
    cache_label = ["L1", "L2", "L3"]
    for i in range(len(cache_size)):
        print (i)
        plt.axvline(x=cache_size[i], linestyle='-', color='red', linewidth=1)
        plt.text(cache_size[i], cache_hauteur[i],
             cache_label[i], rotation=90, color='red', weight='bold', horizontalalignment='right')



    plt.show()



    # Vertical line




    exit(-1)

    # -- Annotation: If an annotation file was provided, use it
    if os.path.exists(_path_annotate_file):
        # For each annotation: draw a vertical line with its label and color
        with open(_fileLog_mem, 'r') as f:
            _start_time = int(f.readline().split(' ')[-1])

        # Parse the annotation file into content
        content = []
        with open(_path_annotate_file, 'r') as f:
            content = f.readlines()

        # Each line is formated like:     TIME |  FLAG  |  COLOR
        for x in content:
            line = x.split(' ')
            # X is the time (in second) when the event occurred after the begging of the bandwidth profiling
            # As is it useless to draw a line for events which did not occurred during the profiling we move on the next
            X = (int(line[0]) - _start_time) / 1000.0

            if X < _start_range:  # Not yet in the range  : move on
                continue
            elif X > _end_range:  # After the range       : stop
                break

            time = X
            flag = (line[1])
            color = (line[2][:-1])

            # Vertical line
            plt.axvline(x=time, linestyle='dotted', color=color)
            plt.text(time, get_y_location(color) * (max(y_Total) / 4),
                     flag, rotation=90, color=color, weight='bold', horizontalalignment='right')

    # ------------------------------------ OUTPUT  ----------------------------------------------

    # Output: graphical or png file ?
    if _screen:
        logging.info(" Output = Screen")
        plt.show()
        exit(-4)
    else:
        logging.info(" Output = " + _path_image_file)
        plt.savefig(str(_path_image_file))


# --------------------------------------------------------------------------------------------
# -------------------------------- END OF MAIN -----------------------------------------------
# --------------------------------------------------------------------------------------------







def parse_parameter():
    global _fileLog_mem

    parser = argparse.ArgumentParser(
        description='Generate a graph based on a log file gathered with YAMB profiling tool.\n')

    parser.add_argument('-d', '--data', dest='path_log', nargs='?', type=str, default="not_set", required=True,
                        help='log file where data are stored')

    # read and set parameters
    args = parser.parse_args()

    # -- LOG --
    _fileLog_mem = args.path_log
    if not os.path.exists(_fileLog_mem):
        logging.error(" Log file does not exist = " + _fileLog_mem)
        sys.exit(-1)
    logging.info(" Log file = " + _fileLog_mem)


def is_screen_available():
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


#
#
# -- Parse the perf log file
def parse_log_mem(filename):
    arr = np.loadtxt(filename, delimiter=',', dtype='float')
    # type = np.loadtxt(filename, dtype=str, delimiter=',', usecols=[3])
    res_values_read = np.empty((0, 6), float)
    res_values_write = np.empty((0, 6), float)
    res_ = np.empty([1, 0])

    # arr = arr.transpose()
    stride_vector = arr[0].astype(int)
    print (" --- Stride :" + str(stride_vector))
    arr = np.delete(arr, 0, axis=0)

    x_value = arr[0:arr.shape[0], 0].astype(int)
    print (" ---  Data set (in bit) : " + str(x_value))

    fig, ax = plt.subplots()  # create figure and axes
    for i in range(1, len(stride_vector)):
        y_value = arr[:, i]
        y_value[y_value == 0] = np.nan  # replace 0 by nan to not plot not existing values
        # print (" y value : " + str (y_value))

        ax.plot(x_value, y_value, label=str(stride_vector[i]))

    ax.set_xlabel("Data set size")
    plt.xscale('log')

    ax.set_ylabel("Gb per sec")
    # ax.get_xaxis().get_major_formatter().set_scientific(False)

    ax.legend(loc=1,
              ncol=1, borderaxespad=0.)

    # plt.show()


if __name__ == "__main__":
    main()
