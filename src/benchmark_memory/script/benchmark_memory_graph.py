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

from __future__ import \
    print_function  # https://stackoverflow.com/questions/493386/how-to-print-without-newline-or-space

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
_screen = True  # Argument parsing
_is_cache = False

logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)

# source https://en.wikichip.org/wiki/intel/xeon_gold/6148#Cache
_cache_size = [32000, 1000000, 28000000]
_cache_label = ["L1", "L2", "L3"]
_cache_hauteur = [50, 150, 150]  # hauteur d'affichage du label en BG/s


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
    print(" --- Stride :" + str(stride_array))
    log_file_array = np.delete(log_file_array, 0, axis=0)

    # 2nd step: recover the data set size --> the first column
    x_value_dataset_size = log_file_array[0:log_file_array.shape[0], 0].astype(int)
    print(" --- Data set (in bit) : " + str(x_value_dataset_size[:3]) + " ... " + str(x_value_dataset_size[-3:]))

    ## -- PLOT THE RESULTS --
    fig, ax = plt.subplots()  # create figure and axes

    title_font = {'fontname': 'Arial', 'size': '20', 'color': 'black', 'weight': 'bold',
                  'verticalalignment': 'bottom'}
    axis_font = {'fontname': 'Arial', 'size': '14', 'weight': 'bold'}
    contour_color = 'white'

    # Get the values and generate the plot

    # for each stride we draw a line
    for i in range(0, len(stride_array)):
        y_value = log_file_array[:, i + 1]  # get the column for the current stride
        y_value[y_value == 0] = np.nan  # replace 0 by nan to not plot not existing values
        y_value[y_value > 10000] = np.nan  # replace 0 by nan to not plot not existing values
        # if (i % 2 == 0):
        ax.plot(x_value_dataset_size, y_value, label=str(stride_array[i]), linewidth=1)
        # else:
        # ax.plot(x_value_dataset_size, y_value, label=str(str(stride_array[i]) + "bw"), linewidth=1, linestyle='-')

    ## -- DRAWING VERTICAL LINES FOR CACHE L1 L2 L3 --
    if _is_cache:
        print(' --- Cache size : ', end='')
        for i in range(len(_cache_size)):
            print(_cache_label[i] + "(" + str(_cache_size[i]) + ") ", end='')
            plt.axvline(x=_cache_size[i], linestyle='-', color='red', linewidth=1)
            plt.text(_cache_size[i], _cache_hauteur[i],
                     _cache_label[i], rotation=90, color='red', weight='bold', horizontalalignment='right')
        print('')

    plt.xlabel("Data set size", **axis_font)
    plt.ylabel("Bandwidth (GBs/s)", **axis_font)
    plt.title("Bench_mem - " + str(os.path.basename(_fileLog_mem)), **title_font)

    plt.xscale('log')
    ax.legend(title="Size of stride (MB)", loc=0,
              ncol=2, borderaxespad=0.)
    plt.gcf().set_facecolor(contour_color)

    ax = fig.add_subplot(1, 1, 1)
    for spine in ax.spines:
        ax.spines[spine].set_color('#C6C9CA')

    plt.grid(True)

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
    global _is_cache

    parser = argparse.ArgumentParser(
        description='Generate a graph based on a log file gathered with YAMB profiling tool.\n')

    parser.add_argument('-d', '--data', dest='path_log', nargs='?', type=str, default="not_set", required=True,
                        help='log file where data are stored')

    parser.add_argument("--cache", type=str2bool, nargs='?', const=True, default=False, help="Activate nice mode.")
    # read and set parameters
    args = parser.parse_args()

    # -- LOG --
    _fileLog_mem = args.path_log
    if not os.path.exists(_fileLog_mem):
        logging.error(" Log file does not exist = " + _fileLog_mem)
        sys.exit(-1)
    logging.info(" Log file = " + _fileLog_mem)

    _is_cache = args.cache
    if _is_cache:
        logging.info(" Display cache = " + str(_is_cache))


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


def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


if __name__ == "__main__":
    main()
