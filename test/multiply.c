#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int N;
int M;


#define A(i, j) a[i*M+j]
#define B(i, j) b[i*N+j]
#define C(i, j) c[i*N+j]


double sum_res(double *c, int N, int M) {
  int i;
  double res = 0;
  for (i = 0; i < N; i++) {
    res += C(i, 0);
  }
  return res;
}

void mult_simple(double *a, double *b, double *c) {

  memset(c, 0, sizeof(c[0]) * N * N);
  int i, j, k;
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      for (k = 0; k < M; ++k) {
	C(i, j) += A(i, k) * B(k, j);
      }
    }
  }
  return;
}

void mult_KIJ(double *a, double *b, double *c) {
  memset(c, 0, sizeof(c[0]) * N * N);
  int i, j, k;
  for (k = 0; k < M; ++k)
    for (i = 0; i < N; ++i) {
      for (j = 0; j < N; ++j) {
        C(i, j) += A(i, k) * B(k, j);
      }
    }
  return;
}


int VERSION;

void mult_var(double *a, double *b, double *c) {
  memset(c, 0, sizeof(c[0]) * N * N);
  if (VERSION == 1){

    int i, j, k;
    for (k = 0; k < M; ++k)
      for (i = 0; i < N; ++i) 
	for (j = 0; j < N; ++j) 
	  C(i, j) += A(i, k) * B(k, j);
  }


  if(VERSION   == 2){
    int i, j, k;
    for (i = 0; i < N; ++i) {
      for (j = 0; j < N; ++j) {
	for (k = 0; k < M; ++k) {
	  C(i, j) += A(i, k) * B(k, j);
	}
      }
    }
  }


  return;
}




void init_mat (double * a, int N, int M){
  int i, j;
  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      A(i, j) = rand() % 100;
    }
  }
}


int main (int argc, char *argv[]){
  N = 1000;
  M = 700;

  double *a = (double *) malloc(sizeof(double) * N * M);
  double *b = (double *) malloc(sizeof(double) * N * M);
  double *c = (double *) malloc(sizeof(double) * N * N);

  

  init_mat(a, N, M);
  init_mat(b, N, M);



  //  mult_simple(a, b, c);
  //printf("simple:  %lf\n", sum_res(c, N, M));

  VERSION = atoi(argv[1]);
  mult_var(a, b, c);
  printf("VERSION %d:  %lf\n",VERSION, sum_res(c, N, M));


}
