//
// Created by Jean Pourroy on 11/04/2018.
//

#ifndef __bm_common_H__
#define __bm_common_H__

#include <limits>
#include <sstream>      // std::stringstream


#define BM_DATA_TYPE double
#define LOG10 2.30258509299404568401
#define BIG_VAL std::numeric_limits<BM_DATA_TYPE>::max()
//#define BIG_VAL 999999999.0
#define MIN_OPS_PER_SCAN 128     	/* we do not measure if less MIN_OPS_PER SCAN operations per loop */


// --------- MPI MACRO ------------
#ifdef COMPILED_WITH_MPI
#define MPI_BARRIER MPI_Barrier(MPI_COMM_WORLD);
#else
#define MPI_BARRIER
#endif

#ifdef COMPILED_WITH_MPI
#define MPI_FINALIZE MPI_Finalize();
#else
#define MPI_FINALIZE
#endif
// --------------------------------


//TODO
extern BM_DATA_TYPE *mat;  //THE matrix :)

extern int mpi_rank;
extern int mpi_size;
extern std::stringstream black_hole;
#define COUT_MPI ((mpi_rank == 0) ? std::cout : black_hole)
#define COUT ((SCREEN_OUTPUT) ? std::cout : black_hole)
#define DEBUG_MPI ((mpi_rank == 0) ? DEBUG : black_hole)


extern bool WITH_MPI;

bool is_Launched_with_mpirun ();


#endif //__bm_common_H__
