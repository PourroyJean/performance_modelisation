//
// Created by Jean Pourroy on 13/04/2018.
//

#ifndef PERFORMANCE_MODELISATION_BM_BENCHMARK_H
#define PERFORMANCE_MODELISATION_BM_BENCHMARK_H

#include "bm_misc.h"
#include "bm_parameters.h"


static double towrite = 0.0;


BM_DATA_TYPE sum_read_unroll1  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_read_unroll2  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_read_unroll4  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_read_unroll8  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

BM_DATA_TYPE sum_readspe_unroll2  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_readspe_unroll4  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_readspe_unroll8  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_readspe_unroll16  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_readspe_unroll32  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_readspe_unroll64  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

BM_DATA_TYPE sum_readind_unroll2  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_readind_unroll4  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_readind_unroll8  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

BM_DATA_TYPE sum_write_unroll1  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_write_unroll2  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_write_unroll4  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_write_unroll8  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

BM_DATA_TYPE sum_writespe_unroll2  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_writespe_unroll4  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_writespe_unroll8  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

BM_DATA_TYPE sum_writeind_unroll2  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_writeind_unroll4  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
BM_DATA_TYPE sum_writeind_unroll8  (bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);



#endif //PERFORMANCE_MODELISATION_BM_BENCHMARK_H
