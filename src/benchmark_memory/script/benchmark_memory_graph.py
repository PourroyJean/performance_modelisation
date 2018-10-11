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

from colour import Color #https://github.com/vaab/colour used to generate the color gradient

from math import log, sqrt, pow, fabs

# --------------------------------------------------------------------------------------------
# ------------------------------------ GLOBAL VARIABLE INIT ----------------------------------
# --------------------------------------------------------------------------------------------

_fileLog_mem = ""  # Argument parsing
_screen = True # is screen re
_is_cache = False  # Argument
_is_annotate = False  # Argument

logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)

# source https://en.wikichip.org/wiki/intel/xeon_gold/6148#Cache
_cache_size = [32000, 1000000, 28000000]
_cache_label = ["L1", "L2", "L3"]
_cache_hauteur = [50, 70, 70]  # hauteur d'affichage du label en GB/s

_stride_array = []
annot = []
_ax = []
_sc_l = []
_fig = []


# --------------------------------------------------------------------------------------------
# ------------------------------------ MAIN --------------------------------------------------
# --------------------------------------------------------------------------------------------


def main():
    global annot
    global _ax
    global sc
    global _sc_l
    global _fig
    global names
    global _is_annotate
    global _is_cache
    global _stride_array
    is_screen_available()


    # -- PARSING COMMAND LINE --
    parse_parameter()

    # -- OPEN LOG FILE --

    # we check if the last line is full (could not be if the program was stopped)
    file_txt = open(_fileLog_mem, 'rt').readlines()
    len_first = len(file_txt[1])
    len_last = len(file_txt[-1])

    log_file_array = 1
    if (len_first > len_last):
        log_file_array = np.loadtxt(file_txt[:-1], delimiter=',', dtype='float', comments='#')
    else:
        log_file_array = np.loadtxt(file_txt, delimiter=',', dtype='float', comments='#')

    # -- PARSING LOG FILE --


    # 1st step: recover the stride (the first line) and delete it from the array
    _stride_array = log_file_array[0].astype(int)[1:]
    nb_of_stride = len(_stride_array)

    print(" --- Stride (" + str(nb_of_stride) + "): " +  str(_stride_array[:3]) + ((" ... " + str(_stride_array[-3:])) if nb_of_stride > 3 else ''))
    log_file_array = np.delete(log_file_array, 0, axis=0)
    # 2nd step: recover the data set size --> the first column
    x_value_dataset_size = log_file_array[0:log_file_array.shape[0], 0].astype(int)
    print(" --- Data set in bit (" + str(len(x_value_dataset_size)) + ")" + str(x_value_dataset_size[:3]) + ((" ... " + str(x_value_dataset_size[-3:] ) if len(x_value_dataset_size) > 3 else '')))

    ## -- PLOT THE RESULTS --
    _fig, _ax = plt.subplots()  # create figure and axes

    title_font = {'fontname': 'Arial', 'size': '20', 'color': 'black', 'weight': 'bold',
                  'verticalalignment': 'bottom'}
    axis_font = {'fontname': 'Arial', 'size': '14', 'weight': 'bold'}
    contour_color = 'white'

    # Color gradient to generate a different color for each stride
    gradient = list(Color("blue").range_to(Color("red"), len(_stride_array)))
    COLOR = [ C.hex_l for C in gradient]


    # for each stride we draw a curve
    for i in range(0, nb_of_stride):
        y_value = log_file_array[:, i + 1]  # get the column for the current stride
        y_value[y_value == 0] = np.nan  # replace 0 by nan to not plot not existing values
        y_value[y_value > 10000] = np.nan  # replace 0 by nan to not plot not existing values
        # if (i % 2 == 0):
        _ax.plot(x_value_dataset_size, y_value, label=str(_stride_array[i]), linestyle='-', linewidth=1,
                 color=COLOR[i])
        # _ax.plot(x_value_dataset_size, y_value, label=str(str(_stride_array[i]) + "bw"), linewidth=1, linestyle='-')

        # If the annotation option is requested
        # - We draw a point with the scatter function to be able to generate a 'hover' event
        # - We generate a box which will be used when the cursor is on the point
        if _is_annotate:
            _sc_l.extend([_ax.scatter(x_value_dataset_size, y_value, color=COLOR[i])])
            # annot = _ax.annotate("", xy=(0, 0), xytext=(20, 20), textcoords="offset points",
            #                      bbox=dict(boxstyle="round", fc="w"),
            #                      arrowprops=dict(arrowstyle="->"))
            # annot.set_visible(False)


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
    _ax.legend(title="Size of stride (byte)", loc=0,
               ncol=2, borderaxespad=0.)
    plt.gcf().set_facecolor(contour_color)

    # x_v = [10, 100, 1000, 10000, 100000, 1000000, 100000000, 10000000000,100000000000]
    # x_l = ['10byte', '100byte', '1Kb', '10Kb', '100Kb', '1Mb', '10Mb', '10000000000', '1Gb', '10Gb']
    # plt.xticks(x_v,x_l)


    _ax = _fig.add_subplot(1, 1, 1)
    for spine in _ax.spines:
        _ax.spines[spine].set_color('#C6C9CA')

    plt.grid(True)

    # If annotate option was set: connect the event to its handler
    if _is_annotate:
        _fig.canvas.mpl_connect("motion_notify_event", hover)

    # No negatives values
    plt.ylim(ymin=0)

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
    global _is_annotate

    parser = argparse.ArgumentParser(
        description='Generate a graph based on a log file gathered with YAMB profiling tool.\n')

    parser.add_argument('-d', '--data', dest='path_log', nargs='?', type=str, default="not_set", required=True,
                        help='log file where data are stored')

    parser.add_argument("--cache", type=str2bool, nargs='?', const=True, default=False, help="Draw the 3 cache levels")
    parser.add_argument("--annotate", type=str2bool, nargs='?', const=True, default=False,
                        help="Annotate the plot with the name of each lines")

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

    _is_annotate = args.annotate
    if _is_annotate:
        logging.info(" Annotate the graph = " + str(_is_annotate))


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

# https://stackoverflow.com/questions/7908636/possible-to-make-labels-appear-when-hovering-over-a-point-in-matplotlib
def update_annot(ind, sc, stride):
    global names
    global _sc_l
    pos = sc.get_offsets()[ind["ind"][0]]
    annot.xy = pos
    annot.set_text(stride)
    annot.get_bbox_patch().set_alpha(0.4)

def hover(event):
    global _fig
    global names
    global _stride_array
    global _ax
    # vis = annot.get_visible()
    if event.inaxes == _ax:
        for i in range(0, len(_sc_l)):
            sc = _sc_l[i]
            cont, ind = sc.contains(event)
            if cont:
                print(" Stride = " + str(_stride_array[i]))
                # TODO remettre
                # update_annot(ind, sc, str(_stride_array[i]))
                # annot.set_visible(True)
                # _fig.canvas.draw_idle()
            # else:
            #     if vis:
            #         annot.set_visible(False)
            #         _fig.canvas.draw_idle()




if __name__ == "__main__":
    main()
