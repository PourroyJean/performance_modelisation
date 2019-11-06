//
// Created by Jean Pourroy on 13/04/2018.
//

#ifndef PERFORMANCE_MODELISATION_DML_BENCHMARK_H
#define PERFORMANCE_MODELISATION_DML_BENCHMARK_H

#include "dml_misc.h"
#include "dml_parameters.h"


static double towrite = 03.0592;


DML_DATA_TYPE sum_read_unroll1  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_read_unroll2  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_read_unroll4  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_read_unroll8  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

DML_DATA_TYPE sum_readspe_unroll2  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_readspe_unroll4  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_readspe_unroll8  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_readspe_unroll16  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_readspe_unroll32  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_readspe_unroll64  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

DML_DATA_TYPE sum_readind_unroll2  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_readind_unroll4  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_readind_unroll8  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

DML_DATA_TYPE sum_write_unroll1  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_write_unroll2  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_write_unroll4  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_write_unroll8  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

DML_DATA_TYPE sum_writespe_unroll2  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_writespe_unroll4  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_writespe_unroll8  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);

DML_DATA_TYPE sum_writeind_unroll2  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_writeind_unroll4  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
DML_DATA_TYPE sum_writeind_unroll8  (Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);



#endif //PERFORMANCE_MODELISATION_DML_BENCHMARK_H
