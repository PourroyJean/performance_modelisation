//
// Created by Jean Pourroy on 07/09/2017.
//

#ifndef PERFORMANCE_MODELISATION_MULTIPLY_VERSION_H
#define PERFORMANCE_MODELISATION_MULTIPLY_VERSION_H


#define A(i, j) a[i*MATRIX_COLUMNS+j]
#define B(i, j) b[i*MATRIX_LINES+j]
#define C(i, j) c[i*MATRIX_LINES+j]

void init_mat(double *a, int MATRIX_LINES, int MATRIX_COLUMNS);

long sum_res(double *c, int MATRIX_LINES, int MATRIX_COLUMNS);

void mult_simple(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS);

void mult_KIJ(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS);

void mult_block(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS, int BLOCK_SIZE);

void mult_simple_omp(double *a, double *b, double *c, int MATRIX_LINES, int MATRIX_COLUMNS);


#endif //PERFORMANCE_MODELISATION_MULTIPLY_VERSION_H
