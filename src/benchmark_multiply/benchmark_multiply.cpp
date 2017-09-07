#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <iomanip>

using namespace std;

int BENCH_VERSION = 1;
int MATRIX_LINES = 100;
int MATRIX_COLUMNS = 100;


#define A(i, j) a[i*MATRIX_COLUMNS+j]
#define B(i, j) b[i*MATRIX_LINES+j]
#define C(i, j) c[i*MATRIX_LINES+j]


void print_usage(int argc, char **argv) {
    cout << "Usage: " << argv[0] << " <option>" << endl;
    cout << "   -V <version>" << endl <<
         "       1 : standard multiplication (IJK)" << endl <<
         "       2 : rotated  multiplication (KIJ)" << endl <<
         "   -L <lines>    number of lines" << endl <<
         "   -C <columns>  number of columns";
    cout << endl;
}

double sum_res(double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i;
    double res = 0;
    for (i = 0; i < MATRIX_LINES; i++) {
        res += C(i, 0);
    }
    return res;
}

void mult_simple(double *a, double *b, double *c) {

    int i, j, k;
    for (i = 0; i < MATRIX_LINES; ++i) {
        for (j = 0; j < MATRIX_LINES; ++j) {
            for (k = 0; k < MATRIX_COLUMNS; ++k) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
    return;
}

void mult_KIJ(double *a, double *b, double *c) {
    memset(c, 0, sizeof(c[0]) * MATRIX_LINES * MATRIX_LINES);
    int i, j, k;
    for (k = 0; k < MATRIX_COLUMNS; ++k)
        for (i = 0; i < MATRIX_LINES; ++i) {
            for (j = 0; j < MATRIX_LINES; ++j) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    return;
}


void mult_var(double *a, double *b, double *c) {
    memset(c, 0, sizeof(c[0]) * MATRIX_LINES * MATRIX_LINES);
    if (BENCH_VERSION == 1) {

        int i, j, k;
        for (k = 0; k < MATRIX_COLUMNS; ++k)
            for (i = 0; i < MATRIX_LINES; ++i)
                for (j = 0; j < MATRIX_LINES; ++j)
                    C(i, j) += A(i, k) * B(k, j);
    }


    if (BENCH_VERSION == 2) {
        int i, j, k;
        for (i = 0; i < MATRIX_LINES; ++i) {
            for (j = 0; j < MATRIX_LINES; ++j) {
                for (k = 0; k < MATRIX_COLUMNS; ++k) {
                    C(i, j) += A(i, k) * B(k, j);
                }
            }
        }
    }


    return;
}


void init_mat(double *a, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i, j;
    for (i = 0; i < MATRIX_LINES; i++) {
        for (j = 0; j < MATRIX_COLUMNS; j++) {
            A(i, j) = rand() % 100;
        }
    }
}


void parse_arguments(int argc, char **argv) {


    if (argc <= 1) {
        print_usage(argc, argv);
        exit(EXIT_FAILURE);
    }


    const char *const short_opts = "V:L:C:h";
    const option long_opts[] = {
            {"version", required_argument, nullptr, 'V'},
            {"lines",   required_argument, nullptr, 'L'},
            {"columns", required_argument, nullptr, 'C'},

            {"help",    no_argument,       nullptr, 'h'},
            {nullptr, 0,                   nullptr, 0}
    };

    char option;
    string tmp_str;
    while ((option = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
        int ioptarg;
        switch (option) {
            case 'L':
                ioptarg = atoi(optarg);

                if (ioptarg <= 0 || ioptarg > 9999999999999) {
                    MATRIX_LINES = ioptarg;
                } else {
                    printf("/!\\ WRONG LINES OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'C':
                ioptarg = atoi(optarg);

                if (ioptarg <= 0 || ioptarg > 9999999999999) {
                    MATRIX_COLUMNS = ioptarg;
                } else {
                    printf("/!\\ WRONG COLUMNS OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'V':
                ioptarg = atoi(optarg);

                if (ioptarg >= 1 || ioptarg <= 2) {
                    BENCH_VERSION = ioptarg;
                } else {
                    printf("/!\\ WRONG VERSION OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;


            case 'h':
                print_usage(argc, argv);
                break;
            default:
                print_usage(argc, argv);
                exit(EXIT_FAILURE);
        }
    }

}


int main(int argc, char *argv[]) {


    parse_arguments(argc, argv);

    //MeMory allocation
    double *a = (double *) malloc(sizeof(double) * MATRIX_LINES * MATRIX_COLUMNS);
    double *b = (double *) malloc(sizeof(double) * MATRIX_LINES * MATRIX_COLUMNS);
    double *c = (double *) malloc(sizeof(double) * MATRIX_LINES * MATRIX_LINES);


    //Initilisation of the matrix
    init_mat(a, MATRIX_LINES, MATRIX_COLUMNS);
    init_mat(b, MATRIX_LINES, MATRIX_COLUMNS);
    memset(c, 0, sizeof(c[0]) * MATRIX_LINES * MATRIX_LINES);


    switch (BENCH_VERSION) {
        case 1:
            mult_simple(a, b, c);
            cout << "BENCH_VERSION " << BENCH_VERSION << " - " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS) << endl;
            break;

        case 2:
            mult_KIJ(a, b, c);
            cout << "BENCH_VERSION " << BENCH_VERSION << " - " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS) << endl;
            break;
    }


    mult_var(a, b, c);


}
