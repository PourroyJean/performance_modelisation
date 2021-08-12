//
// Created by pourroy on 7/26/18.
//
#include "dml_misc.h"
#include <unistd.h>
#include <string>

using namespace std;


extern std::stringstream black_hole;
#define COUT_MPI ((mpi_rank == 0) ? std::cout : black_hole)
