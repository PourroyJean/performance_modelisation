//
// Created by pourroy on 7/26/18.

#include "dml_misc.h"
#include <cstddef> // For NULL/nullptr
#include <sstream> // Needed for std::stringstream

// Definition for the main data matrix pointer.
DML_DATA_TYPE *mat = nullptr;

// Definition for the MPI rank.
int mpi_rank = 0;

// Definition for the total number of MPI processes.
int mpi_size = 1;

// Definition for the logging flag.
bool is_I_LOG = false;

// Definition for the overall loop counter (used in main for final aggregation)
double overall_total_loops_global = 0.0;
