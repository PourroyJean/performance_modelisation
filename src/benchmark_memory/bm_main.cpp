#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>

#include <sys/time.h>
#include <misc.h>
#include "bm_misc.h"
#include "misc.h"
#include "bm_parameters.h"
#include "bm_benchmark.h"

using namespace std;

void init_mat(bm_parameters *p);

int work(bm_parameters *p);

double work_part2(bm_parameters *p, uint64_t max_index, int step);

double get_micros();

double total_loops = 0.01; //TODO metre a 0

BM_DATA_TYPE *mat;  //THE matrix :)


int main(int argc, const char *argv[]) {
    double tot, band;
    double start_time, end_time = 0.0;


    //Parse argument, initiliaze the argument structure and print the configuration
    cout << "\n ------------- CONFIGURATION --------------\n";
    bm_parameters *my_parameters = new bm_parameters();
    my_parameters->init_arguments(argc, argv);
    cout << "\n-- The following configuration will be used for the benchmark:\n";
    my_parameters->print_configuration();

    //Matrice initialisation
    init_mat(my_parameters);


    cout << "main " << mat << " val " << mat[0] << endl;
    cout << "\n ---------------- BENCHMARK ----------------\n";

    start_time = get_micros();
    work(my_parameters);
    end_time = get_micros();


    cout << "\n ---------------- RESULTS ----------------\n";
    tot = (end_time - start_time) * 1000.0 / total_loops;
    band = my_parameters->m_CACHE_LINE / tot;

    printf("%20s    %-10s \n", "Name", my_parameters->m_prefix.c_str());
    printf("%20s    %-10f \n", "Total micros", end_time - start_time);
    printf("%20s    %-10f \n", "Total Loops", total_loops);
    printf("%20s    %-10f ns/loop\n", "Performance", tot);
    printf("%20s    %-10f GB/s\n", "Bandwidth", band);


//    printf("_ %s  total micros = %1.0f total loops = %1.0f perfo = %1.2f ns/loop band= %1.3lf GB/s\n",
//           my_parameters->m_prefix.c_str(), end_time - start_time, total_loops, tot, band);
    return (0);
}

int work(bm_parameters *p) {
    //Some init
    double log_max_index, time1, time2, ns_per_op, num_ops, best_measure, worst_measure, sum_measures;
    uint64_t max_index;
    int step, measure;


    //Print header
    printf("_ %s  maxKmem ", p->m_prefix.c_str());
    for (step = p->m_MIN_STRIDE; step <= p->m_MAX_STRIDE; step *= 2) {
        if (p->m_DISP == DISP_MODE::AVERAGE) printf(" AV=%4d  ", step * 8);
        if (p->m_DISP == DISP_MODE::BEST) printf(" BE=%4d  ", step * 8);
        if (p->m_DISP == DISP_MODE::TWO) printf(" BE=%4d AV=%4d  ", step * 8, step * 8);
        if (p->m_DISP == DISP_MODE::ALL) printf(" BE=%4d WO=%4d AV=%4d  ", step * 8, step * 8, step * 8);
    }


    printf("\n");

    //Work
    for (log_max_index = p->m_MIN_LOG10; log_max_index < p->m_MAX_LOG10 + .0000001; log_max_index += p->m_STEP_LOG10) {
        max_index = (THEINT) (double) (exp(log_max_index * LOG10) + 0.5);
        THEINT istride = max_index * sizeof(BM_DATA_TYPE) / 1024;
//        if(max_index > MAT_SIZE)break; //TODO MAT_SIZE a ete modifie avant pour avoir le nb elment
        if (max_index > p->m_MAT_NB_ELEM)break;
        printf("_ %s %8lld ", p->m_prefix.c_str(), istride);


        for (step = p->m_MIN_STRIDE; step <= p->m_MAX_STRIDE; step *= 2) {
            double gb;
            best_measure = BIG_VAL;
            worst_measure = 0;
            sum_measures = 0.0;
            for (measure = 0; measure < p->m_MAX_MEASURES; measure++) {
                double t;

//                printf ("_work_ p->m_MIN_LOG10   %f \n", p->m_MIN_LOG10);
//                printf ("_work_ p->m_MAX_LOG10   %f \n", p->m_MAX_LOG10);
//                printf ("_work_ p->m_STEP_LOG10  %f \n", p->m_STEP_LOG10);
//                printf ("_work_ max_index        %d \n", max_index);
//                printf ("_work_ step             %d \n", step);
//                printf ("_work_ p->MIN_STRIDE    %d \n", p->m_MIN_STRIDE);
//                exit (-1);


                int repeat;
                THEINT ops_per_scan = max_index / step;
                if (ops_per_scan < MIN_OPS_PER_SCAN) {
                    num_ops = BIG_VAL;
                } else {
                    repeat = p->m_MAX_OPS / ops_per_scan;
                    if (repeat == 0)repeat = 1;

                    // --- BENCHMARK MEASURE ---
                    time1 = get_micros();
                    num_ops = p->m_BENCHMARK(p, max_index, step, repeat, ops_per_scan);
                    time2 = get_micros();
                }


                t = (time2 - time1) * 1000.0;
                if (num_ops != 0)
                    total_loops += num_ops;
                if (t < best_measure)
                    best_measure = t;
                if (t > worst_measure)
                    worst_measure = t;
                sum_measures += t;
            }

            ns_per_op = best_measure / num_ops;
            gb = p->m_CACHE_LINE;
            gb /= ns_per_op;
            if (p->m_DISP == DISP_UNIT::GB)ns_per_op = gb;
            if (p->m_DISP == DISP_UNIT::CY)ns_per_op *= p->m_GHZ;
            if ((p->m_DISP == DISP_MODE::BEST || p->m_DISP == DISP_MODE::ALL || p->m_DISP == DISP_MODE::TWO)) {
                printf(" %7.3f", ns_per_op);
            }

            ns_per_op = worst_measure / num_ops;
            gb = p->m_CACHE_LINE;
            gb /= ns_per_op;
            if (p->m_DISP == DISP_UNIT::GB)ns_per_op = gb;
            if (p->m_DISP == DISP_UNIT::CY)ns_per_op *= p->m_GHZ;
            if (p->m_DISP == DISP_MODE::ALL) {
                printf(" %7.3f", ns_per_op);
            }

            ns_per_op = sum_measures / num_ops / p->m_MAX_MEASURES;

            gb = p->m_CACHE_LINE;
            gb /= ns_per_op;
            if (p->m_unit == DISP_UNIT::GB) {
                ns_per_op = gb;
            }
            if (p->m_unit == DISP_UNIT::CY) {
                ns_per_op *= p->m_GHZ;
            }

//            cout << "\n JEAN : " << ns_per_op << " " << gb << " real : " ;

            if (p->m_DISP == DISP_MODE::ALL || p->m_DISP == DISP_MODE::TWO || p->m_DISP == DISP_MODE::AVERAGE) {
                printf(" %7.3f  ", ns_per_op);
            }
        }

        printf("\n");
        fflush(stdout);
    }
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//   we use the content of the data to count the number of memory operations
//   then for read count 1 per operation
//   then for write count 2 per operation    read+write
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void init_mat(bm_parameters *p) {
    int lp;
    char tmpc[1024];
    unsigned int m2;
    uint64_t si = p->m_MAT_SIZE;
    BM_DATA_TYPE *mat_for_free;


//    set_affinity(MEM_AFF); //TODO

    sprintf(tmpc, "_____  %lld \n", si);
    DEBUG << tmpc;
    mat_for_free = (BM_DATA_TYPE *) malloc(si);
    assert(mat_for_free != NULL);

//    p->m_MAT_SIZE /= (sizeof(double));
    mat = mat_for_free;
    mat += (p->m_MAT_OFFSET / sizeof(double));
    if (p->m_VERBOSE != 0)
        sprintf(tmpc, "__ mat address is %p %lld\n", mat_for_free, si);
    DEBUG << tmpc;

    /* store 1 to count the read accesses */
    /* store 2 to count the read + write accesses */
    BM_DATA_TYPE init = (p->m_type == BENCH_TYPE::READ) ? 1.0 : 2.0;
    for (lp = 0; lp < p->m_MAT_NB_ELEM; lp++) {
        mat[lp] = init;
    }

    mat[0] = 5.0;
    cout << "MAT add is : " << mat << " first val is " << *mat << endl;
    cout << "init " << *mat << endl;

    // need to set CPU affinity after having writen in memory
//    set_affinity(CPU_AFF); //TODO
}

double get_micros() {
    static struct timezone tz;
    static struct timeval tv;
    gettimeofday(&tv, &tz);
    return ((tv.tv_sec * 1000000.0) + tv.tv_usec);
}

