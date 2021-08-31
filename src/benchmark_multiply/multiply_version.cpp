//
// Created by Jean Pourroy on 07/09/2017.
//

#include <cstdlib>
#include "multiply_version.h"
#include "iostream"
#include "iomanip"

#include "omp.h"

using namespace std;

//TODO check init with real double
void init_mat(double *a, double *b, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i, j;
    for (i = 0; i < MATRIX_LINES; i++) {
        for (j = 0; j < MATRIX_COLUMNS; j++) {
            A(i, j) = rand() % 100;
        }
    }

    for (i = 0; i < MATRIX_COLUMNS; i++) {
        for (j = 0; j < MATRIX_LINES; j++) {
            B(i, j) = rand() % 100;
        }
    }
}


void print_matrix(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i, j;

    cout << " -- MATRIX A -- \n";
    for (i = 0; i < MATRIX_LINES; i++) {
        for (j = 0; j < MATRIX_COLUMNS; j++)
            cout << setw(4) << A(i, j) << " ";
        cout << endl;
    }
    cout << endl;

    cout << " -- MATRIX B -- \n";
    for (i = 0; i < MATRIX_COLUMNS; i++) {
        for (j = 0; j < MATRIX_LINES; j++)
            cout << setw(4) << B(i, j) << " ";
        cout << endl;
    }
    cout << endl;

    cout << " -- MATRIX C -- \n";
    for (i = 0; i < MATRIX_LINES; i++) {
        for (j = 0; j < MATRIX_LINES; j++)
            cout << setw(4) << C(i, j) << " ";
        cout << endl;
    }
}


long sum_res(double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i;
    float res = 0;
    for (i = 0; i < MATRIX_LINES * MATRIX_LINES; i++) {
        res += c[i];
    }
    return res;
}


void count_omp_threads() {
    int nb_threads = 0;
    #pragma omp parallel reduction (+:nb_threads)
    {
        nb_threads = 1;
    }
    cout << "(threads=" << nb_threads << ")\n";
}


//   __      ________ _____   _____ _____ ____  _   _            __
//   \ \    / /  ____|  __ \ / ____|_   _/ __ \| \ | |          /_ |
//    \ \  / /| |__  | |__) | (___   | || |  | |  \| |  ______   | |
//     \ \/ / |  __| |  _  / \___ \  | || |  | | . ` | |______|  | |
//      \  /  | |____| | \ \ ____) |_| || |__| | |\  |           | |
//       \/   |______|_|  \_\_____/|_____\____/|_| \_|           |_|
//
//
void mult_simple(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i, j, k;
    for (i = 0; i < MATRIX_LINES; ++i) {
        for (j = 0; j < MATRIX_LINES; ++j) {
            for (k = 0; k < MATRIX_COLUMNS; ++k) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
}


//   __      ________ _____   _____ _____ ____  _   _            ___
//   \ \    / /  ____|  __ \ / ____|_   _/ __ \| \ | |          |__ \
//    \ \  / /| |__  | |__) | (___   | || |  | |  \| |  ______     ) |
//     \ \/ / |  __| |  _  / \___ \  | || |  | | . ` | |______|   / /
//      \  /  | |____| | \ \ ____) |_| || |__| | |\  |           / /_
//       \/   |______|_|  \_\_____/|_____\____/|_| \_|          |____|
//
//
void mult_KIJ(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i, j, k;
    for (k = 0; k < MATRIX_COLUMNS; ++k)
        for (i = 0; i < MATRIX_LINES; ++i) {
            for (j = 0; j < MATRIX_LINES; ++j) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
}


//   __      ________ _____   _____ _____ ____  _   _            ____
//   \ \    / /  ____|  __ \ / ____|_   _/ __ \| \ | |          |___ \
//    \ \  / /| |__  | |__) | (___   | || |  | |  \| |  ______    __) |
//     \ \/ / |  __| |  _  / \___ \  | || |  | | . ` | |______|  |__ <
//      \  /  | |____| | \ \ ____) |_| || |__| | |\  |           ___) |
//       \/   |______|_|  \_\_____/|_____\____/|_| \_|          |____/
//
//
void mult_block(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS, int BLOCK_SIZE) {
    for (int ii = 0; ii < MATRIX_LINES; ii += BLOCK_SIZE)
        for (int jj = 0; jj < MATRIX_LINES; jj += BLOCK_SIZE)
            for (int kk = 0; kk < MATRIX_LINES; kk += BLOCK_SIZE)
                for (int i = ii; i < ii + BLOCK_SIZE; ++i)
                    for (int j = jj; j < jj + BLOCK_SIZE; ++j)
                        for (int k = kk; k < kk + BLOCK_SIZE; ++k)
                            C(i, j) += A(i, k) * B(k, j);


}


//   __      ________ _____   _____ _____ ____  _   _            _  _
//   \ \    / /  ____|  __ \ / ____|_   _/ __ \| \ | |          | || |
//    \ \  / /| |__  | |__) | (___   | || |  | |  \| |  ______  | || |_
//     \ \/ / |  __| |  _  / \___ \  | || |  | | . ` | |______| |__   _|
//      \  /  | |____| | \ \ ____) |_| || |__| | |\  |             | |
//       \/   |______|_|  \_\_____/|_____\____/|_| \_|             |_|
//
//
void mult_simple_omp(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i, j, k;
    count_omp_threads();

    #pragma omp parallel for schedule(static, 50)  collapse(2) private(i, j, k) shared(a, b, c)
    {
        for (i = 0; i < MATRIX_LINES; ++i)
            for (j = 0; j < MATRIX_LINES; ++j) {
                double dot = 0;
                for (k = 0; k < MATRIX_COLUMNS; ++k)
                    dot += A(i, k) * B(k, j);
                C(i, j) = dot;
            }
    }
}

//   __      ________ _____   _____ _____ ____  _   _            _____
//   \ \    / /  ____|  __ \ / ____|_   _/ __ \| \ | |          | ____|
//    \ \  / /| |__  | |__) | (___   | || |  | |  \| |  ______  | |__
//     \ \/ / |  __| |  _  / \___ \  | || |  | | . ` | |______| |___ \
//      \  /  | |____| | \ \ ____) |_| || |__| | |\  |           ___) |
//       \/   |______|_|  \_\_____/|_____\____/|_| \_|          |____/
//
//
void mult_simple_omp_gpu(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {

    #pragma omp target teams distribute parallel for collapse(2) map(to: a[0:MATRIX_LINES*MATRIX_COLUMNS], b[0:MATRIX_LINES*MATRIX_COLUMNS]) map(tofrom: c[0:MATRIX_LINES*MATRIX_LINES]) shared(a, b, c) schedule(auto)
    {
        for (int i = 0; i < MATRIX_LINES; ++i)
            for (int j = 0; j < MATRIX_LINES; ++j) {
                double dot = 0;
                for (int k = 0; k < MATRIX_COLUMNS; ++k)
                    dot += A(i, k) * B(k, j);
                C(i, j) = dot;
            }
    }
}

//   __      ________ _____   _____ _____ ____  _   _              __
//   \ \    / /  ____|  __ \ / ____|_   _/ __ \| \ | |            / /
//    \ \  / /| |__  | |__) | (___   | || |  | |  \| |  ______   / /_
//     \ \/ / |  __| |  _  / \___ \  | || |  | | . ` | |______| | '_ \
//      \  /  | |____| | \ \ ____) |_| || |__| | |\  |          | (_) |
//       \/   |______|_|  \_\_____/|_____\____/|_| \_|           \___/
//
//
void mult_block_omp_orig(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS, int BLOCK_SIZE) {
    int i, j, k, jj, kk;
    double tmp;
    int size = MATRIX_LINES;
    int block_size = BLOCK_SIZE; //40 is best on AMD EPYC 7542 32-Core Processor
    count_omp_threads();
    #pragma omp parallel shared(a, b, c, size) private(i, j, k, jj, kk, tmp)
    {
        #pragma omp for schedule (static) collapse(2)
        for (jj = 0; jj < size; jj += block_size) {
            for (kk = 0; kk < size; kk += block_size) {
                for (i = 0; i < size; i++) {
                    for (j = jj; j < ((jj + block_size) > size ? size : (jj + block_size)); j++) {
                        tmp = 0;
                        for (k = kk; k < ((kk + block_size) > size ? size : (kk + block_size)); k++) {
                            tmp += A(i, k) * B(k, j);
                        }
                        C(i, j) += tmp;
                    }
                }
            }
        }
    }
}


void mult_block_omp(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS, int BLOCK_SIZE) {
    count_omp_threads();
    #pragma omp parallel for collapse (2) schedule (static, 1) shared(a, b, c, MATRIX_LINES, MATRIX_COLUMNS, BLOCK_SIZE)
    {
        for (int ii = 0; ii < MATRIX_LINES; ii += BLOCK_SIZE) {
            for (int jj = 0; jj < MATRIX_LINES; jj += BLOCK_SIZE) {
                for (int kk = 0; kk < MATRIX_LINES; kk += BLOCK_SIZE) {
                    for (int i = ii; i < ((ii + BLOCK_SIZE) > MATRIX_LINES ? MATRIX_LINES : (ii + BLOCK_SIZE)); ++i) {
                        for (int j = jj; j < ((jj + BLOCK_SIZE) > MATRIX_LINES ? MATRIX_LINES : (jj + BLOCK_SIZE)); ++j) {
                            for (int k = kk; k < ((kk + BLOCK_SIZE) > MATRIX_LINES ? MATRIX_LINES : (kk + BLOCK_SIZE)); ++k) {
                                C(i, j) += A(i, k) * B(k, j);
                            }
                        }
                    }
                }
            }
        }
    }
}



//-------------------------------------------------
//-------------------------------------------------
//-------------------------------------------------
//TODO: task works bur are serial ...
//from https://github.com/OpenMP/Examples/blob/v4.5.0/sources/Example_task_dep.5.c
void mult_simple_omp_tasks(double *a, double *b, double *c, int no, int nono) {
    int i, j, k, ii, jj, kk;
    count_omp_threads();

    // Task need 2D array
    int BLOCK_SIZE = 100;
    #define MATRIX_COLUMNS 2000
    #define MATRIX_LINES 2000
    auto AA = new double[MATRIX_LINES][MATRIX_COLUMNS];
    auto BB = new double[MATRIX_LINES][MATRIX_COLUMNS];
    auto CC = new double[MATRIX_LINES][MATRIX_COLUMNS];

    for (i = 0; i < MATRIX_LINES; i++) {
        for (j = 0; j < MATRIX_COLUMNS; j++) {
            AA[i][j] = A(i, j);
            BB[i][j] = B(i, j);
            CC[i][j] = 0;
        }
    }

    for (i = 0; i < MATRIX_LINES; i += BLOCK_SIZE) {
        for (j = 0; j < MATRIX_LINES; j += BLOCK_SIZE) {
            for (k = 0; k < MATRIX_LINES; k += BLOCK_SIZE) {
                // Note 1: i, j, k, A, B, C are firstprivate by default
                // Note 2: A, B and C are just pointers
                #pragma omp task private(ii, jj, kk)depend ( in: AA[i:BLOCK_SIZE][k:BLOCK_SIZE], BB[k:BLOCK_SIZE][j:BLOCK_SIZE] ) depend ( inout: CC[i:BLOCK_SIZE][j:BLOCK_SIZE] )
                for (ii = i; ii < i + BLOCK_SIZE; ii++)
                    for (jj = j; jj < j + BLOCK_SIZE; jj++)
                        for (kk = k; kk < k + BLOCK_SIZE; kk++)
                            CC[ii][jj] = CC[ii][jj] + AA[ii][kk] * BB[kk][jj];
            }
        }
    }

    //copy result back
    for (i = 0; i < MATRIX_LINES; i++) {
        for (j = 0; j < MATRIX_COLUMNS; j++) {
            C(i, j) = CC[i][j];
        }
    }
    #undef MATRIX_COLUMNS
    #undef MATRIX_LINES
}

