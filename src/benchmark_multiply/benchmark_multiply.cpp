#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <getopt.h>
#include <iomanip>
#include "multiply_version.h"

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

extern std::stringstream black_hole;

//Default values (can be changed with command line parameters)
int BENCH_VERSION = 1;
int MATRIX_LINES = 100;
int MATRIX_COLUMNS = 100;
int BLOCK_SIZE = 10;

//    #define OMP_TARGET_GPU //TODO remove this

void print_usage(int argc, char **argv);

void parse_arguments(int argc, char **argv);

int main(int argc, char *argv[]) {

    //    DEBUG << "HEY ";
    parse_arguments(argc, argv);

    //MeMory allocation
    //TODO
    //int * af = (int*)aligned_alloc((2*1024*1024), sizeof(int)*4);
    double *a = (double *) malloc(sizeof(double) * MATRIX_LINES * MATRIX_COLUMNS);
    double *b = (double *) malloc(sizeof(double) * MATRIX_LINES * MATRIX_COLUMNS);
    double *c = (double *) malloc(sizeof(double) * MATRIX_LINES * MATRIX_LINES);

    //Initilisation of the matrix: A and B random, C with 0
    init_mat(a, b, MATRIX_LINES, MATRIX_COLUMNS);
    memset(c, 0, sizeof(c[0]) * MATRIX_LINES * MATRIX_LINES);

    switch (BENCH_VERSION) {
        case 1: {
            cout << "BENCH_VERSION IJK     ...\n";
            mult_simple(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
            cout << "SUM = " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS) << endl;
            break;
        }
        case 2: {
            cout << "BENCH_VERSION KIJ     ...\n";
            mult_KIJ(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
            cout << "SUM = " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS) << endl;
            break;
        }
        case 3: {
            cout << "BENCH_VERSION BLOCKING... (block=" << BLOCK_SIZE << ")\n";
            mult_block(a, b, c, MATRIX_LINES, MATRIX_COLUMNS, BLOCK_SIZE);
            cout << "SUM = " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS) << endl;
            break;
        }
        case 4: {
            #if defined(BM_OMP)
            cout << "BENCH_VERSION OMP    ... ";
            mult_simple_omp(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
            cout << "SUM = "  << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS) << endl;
            #endif
            break;
        }
        case 5: {
            #if defined(BM_OMP) && defined(BM_OMP_TARGET_GPU)
            cout << "BENCH_VERSION OMP_GPU...\n";
            mult_simple_omp_gpu(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
            cout << "SUM = " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS) << endl;
            #else
            cout << "Error: check the following compilation flags: BM_OMP, BM_OMP_TARGET_GPU" << endl;
            return -1;
            #endif
            break;
        }
        case 6: {
            #if defined(BM_OMP)
            cout << "BENCH_VERSION OMP_CPU_BLOCK  ... ";
            mult_block_omp(a, b, c, MATRIX_LINES, MATRIX_COLUMNS, BLOCK_SIZE);
            cout << "SUM = "  << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS) << endl;
            #endif
            break;
        }
        default: {
            cout << "Please enter a correct BENCH_VERSION with -V option\n";
            break;
        }
    }

    free(a);
    free(b);
    free(c);

    print_matrix(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);

    return 0;
}


void print_usage(int argc, char **argv) {
    cout << "Usage: " << argv[0] << " <option>" << endl;
    cout << "   -V <version>" << endl <<
         "       1 : SIMPLE" << endl <<
         "       2 : SIMPLE + KIJ" << endl <<
         "       3 : BLOCKING" << endl <<
         "       4 : OPENMP CPU" << endl <<
         "       5 : OPENMP GPU" << endl <<
         "       6 : OPENMP CPU + BLOCKING" << endl <<
         "       10 : All versions" << endl <<
         "   -L <lines>    number of lines" << endl <<
         "   -C <columns>  number of columns" << endl <<
         "   -B <block>    number of block only for -V 3";
    cout << endl;
}


void parse_arguments(int argc, char **argv) {
    if (argc <= 1) {
        print_usage(argc, argv);
        exit(EXIT_FAILURE);
    }

    const char *const short_opts = "V:L:C:B:h";
    const option long_opts[] = {
            {"version", required_argument, nullptr, 'V'},
            {"lines",   required_argument, nullptr, 'L'},
            {"columns", required_argument, nullptr, 'C'},
            {"block",   optional_argument, nullptr, 'B'},

            {"help",    no_argument,       nullptr, 'h'},
            {nullptr, 0,                   nullptr, 0}
    };

    int option;
    string tmp_str;
    while ((option = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
        int ioptarg;
        switch (option) {
            case 'L':
                ioptarg = atoi(optarg);

                if (ioptarg >= 0 && ioptarg < 999999999) {
                    MATRIX_LINES = ioptarg;
                } else {
                    printf("/!\\ WRONG LINES OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'C':
                ioptarg = atoi(optarg);

                if (ioptarg > 0 && ioptarg < 999999999) {
                    MATRIX_COLUMNS = ioptarg;
                } else {
                    printf("/!\\ WRONG COLUMNS OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'B':
                ioptarg = atoi(optarg);

                if (ioptarg > 0 && ioptarg < 999999999) {
                    BLOCK_SIZE = ioptarg;
                } else {
                    printf("/!\\ WRONG BLOCK SIZE OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'V':
                ioptarg = atoi(optarg);

                if (ioptarg >= 1 && ioptarg <= 6) {
                    BENCH_VERSION = ioptarg;
                } else {
                    printf("/!\\ WRONG VERSION OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;


            case 'h':
                print_usage(argc, argv);
                exit(EXIT_SUCCESS);
                break;
            default:
                print_usage(argc, argv);
                exit(EXIT_FAILURE);
        }
    }

}

