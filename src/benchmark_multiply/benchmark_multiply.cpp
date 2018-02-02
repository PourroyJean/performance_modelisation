#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <iomanip>
#include "multiply_version.h"


#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace std;

int BENCH_VERSION = 1;
int MATRIX_LINES = 100;
int MATRIX_COLUMNS = 100;


void print_usage(int argc, char **argv) {
  cout << "Usage: " << argv[0] << " <option>" << endl;
  cout << "   -V <version>" << endl <<
    "       1 : standard multiplication (IJK)" << endl <<
    "       2 : rotated  multiplication (KIJ)" << endl <<
    "       3 : standard multiplication with OpenMP" << endl <<
    "       4 : All versions" << endl <<
    "   -L <lines>    number of lines" << endl <<
    "   -C <columns>  number of columns";
  cout << endl;
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

      if (ioptarg >= 0 && ioptarg < 9999999999999) {
	MATRIX_LINES = ioptarg;
      } else {
	printf("/!\\ WRONG LINES OPTION: %s\n", optarg);
	exit(EXIT_FAILURE);
      }
      break;

    case 'C':
      ioptarg = atoi(optarg);

      if (ioptarg > 0 && ioptarg < 9999999999999) {
	MATRIX_COLUMNS = ioptarg;
      } else {
	printf("/!\\ WRONG COLUMNS OPTION: %s\n", optarg);
	exit(EXIT_FAILURE);
      }
      break;

    case 'V':
      ioptarg = atoi(optarg);

      if (ioptarg >= 1 || ioptarg <= 4) {
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
    mult_simple(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
    cout << "BENCH_VERSION " << BENCH_VERSION << " - " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS)
	 << endl;
    break;

  case 2:
    mult_KIJ(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
    cout << "BENCH_VERSION " << BENCH_VERSION << " - " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS)
	 << endl;
    break;
    /*
  case 3:
        int nb_threads = 0;
#pragma omp  parallel default(shared)
    {
#pragma omp atomic
      nb_threads ++;
    }
#pragma omp barrier

cout << "There is " << nb_threads << " threads\n";

    mult_simple_omp(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
    cout << "BENCH_VERSION " << BENCH_VERSION << " - " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS)
    << endl;
    
  
    break;
    */
  case 4:
    mult_simple(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
    mult_KIJ(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
    mult_simple(a, b, c, MATRIX_LINES, MATRIX_COLUMNS);
    cout << "BENCH_VERSION " << BENCH_VERSION << " - " << setw(10) << sum_res(c, MATRIX_LINES, MATRIX_COLUMNS)
	 << endl;

    break;
    //    mult_var(a, b, c);
  }

}
