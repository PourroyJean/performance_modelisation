//
// Created by Jean Pourroy on 07/09/2017.
//

#include <cstring>
#include <cstdlib>
#include "multiply_version.h"


void init_mat(double *a, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i, j;
    for (i = 0; i < MATRIX_LINES; i++) {
        for (j = 0; j < MATRIX_COLUMNS; j++) {
            A(i, j) = rand() % 100;
        }
    }
}

int sum_res(double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i;
    int res = 0;
    for (i = 0; i < MATRIX_LINES; i++) {
        res += C(i, 0);
    }
    return res;
}

void mult_simple(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {

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

void mult_KIJ(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {
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


void mult_simple_omp(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS) {
    int i, j, k;

    for (i = 0; i < MATRIX_LINES; ++i) {
        for (j = 0; j < MATRIX_LINES; ++j) {
            double my_ij = 0;
            #pragma omp parallel private(my_ij)
            {
                #pragma omp for
                for (k = 0; k < MATRIX_COLUMNS; ++k) {
                    my_ij += A(i, k) * B(k, j);
                }
                #pragma omp atomic
                C(i, j) += my_ij;

                my_ij = 0;

            }
        }
    }
    return;

}

