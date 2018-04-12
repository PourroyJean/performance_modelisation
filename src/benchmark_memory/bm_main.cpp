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

using namespace std;

void init_mat(bm_parameters *p);

int work(bm_parameters *p);

double work_part2(bm_parameters *p, uint64_t max_index, int step);

double get_micros();

double total_loops = 0.01; //TODO metre a 0

BM_DATA_TYPE *mat;

double towrite = 0.0;

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


    cout << "main " << mat << endl;
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

                time1 = get_micros();
                num_ops = work_part2(p, max_index, step);
                time2 = get_micros();

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


BM_DATA_TYPE sum_read_unroll1(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps;
    BM_DATA_TYPE sum = 0;
    int rep;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            p1 += step;
        }
    }
    return (sum);
}


BM_DATA_TYPE sum_read_unroll2(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    int rep;
    BM_DATA_TYPE sum = 0;
    ops_per_scan /= 2;
//    printf ("\n_sum_read_unroll2_ step           %d \n", step);
//    printf ("_sum_read_unroll2_ xstep          %d \n", xstep);
//    printf ("_sum_read_unroll2_ ops_per_scan   %d \n", ops_per_scan);
//    printf ("_sum_read_unroll2_ repeat         %d \n", repeat);

    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
//        cout << rep << " (" <<  mat[0] << ") " <<  flush;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            p1 += xstep;
            sum += *p2;
            p2 += xstep;
//            sum += 1; //TODO remove
        }
    }
//    exit (-1);

    return (sum);
}


BM_DATA_TYPE sum_read_unroll4(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    BM_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 4;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        BM_DATA_TYPE *p3 = p2 + step;
        BM_DATA_TYPE *p4 = p3 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            p1 += xstep;
            sum += *p2;
            p2 += xstep;
            sum += *p3;
            p3 += xstep;
            sum += *p4;
            p4 += xstep;
        }
    }
    return (sum);
}


BM_DATA_TYPE sum_read_unroll8(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    BM_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 8;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        BM_DATA_TYPE *p3 = p2 + step;
        BM_DATA_TYPE *p4 = p3 + step;
        BM_DATA_TYPE *p5 = p4 + step;
        BM_DATA_TYPE *p6 = p5 + step;
        BM_DATA_TYPE *p7 = p6 + step;
        BM_DATA_TYPE *p8 = p7 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            p1 += xstep;
            sum += *p2;
            p2 += xstep;
            sum += *p3;
            p3 += xstep;
            sum += *p4;
            p4 += xstep;
            sum += *p5;
            p5 += xstep;
            sum += *p6;
            p6 += xstep;
            sum += *p7;
            p7 += xstep;
            sum += *p8;
            p8 += xstep;
        }
    }
    return (sum);
}

// ------------ SPE

BM_DATA_TYPE sum_readspe_unroll2(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    BM_DATA_TYPE sum1 = 0, sum2 = 0;
    int rep;
    ops_per_scan /= 2;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum1 += *p1;
            p1 += xstep;
            sum2 += *p2;
            p2 += xstep;
        }
    }
    sum1 += sum2;
    return (sum1);
}

BM_DATA_TYPE sum_readspe_unroll4(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    BM_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    int rep;
    ops_per_scan /= 4;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        BM_DATA_TYPE *p3 = p2 + step;
        BM_DATA_TYPE *p4 = p3 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum1 += *p1;
            p1 += xstep;
            sum2 += *p2;
            p2 += xstep;
            sum3 += *p3;
            p3 += xstep;
            sum4 += *p4;
            p4 += xstep;
        }
    }
    sum1 += sum2;
    sum3 += sum4;
    sum1 += sum3;
    return (sum1);
}

BM_DATA_TYPE sum_readspe_unroll8(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    BM_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0;
    int rep;
    ops_per_scan /= 8;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        BM_DATA_TYPE *p3 = p2 + step;
        BM_DATA_TYPE *p4 = p3 + step;
        BM_DATA_TYPE *p5 = p4 + step;
        BM_DATA_TYPE *p6 = p5 + step;
        BM_DATA_TYPE *p7 = p6 + step;
        BM_DATA_TYPE *p8 = p7 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum1 += *p1;
            p1 += xstep;
            sum2 += *p2;
            p2 += xstep;
            sum3 += *p3;
            p3 += xstep;
            sum4 += *p4;
            p4 += xstep;
            sum5 += *p5;
            p5 += xstep;
            sum6 += *p6;
            p6 += xstep;
            sum7 += *p7;
            p7 += xstep;
            sum8 += *p8;
            p8 += xstep;
        }
    }
    sum1 += sum2;
    sum3 += sum4;
    sum5 += sum6;
    sum7 += sum8;
    sum1 += sum3;
    sum5 += sum7;
    sum1 += sum5;
    return (sum1);
}

//-------------------- INDEXED


BM_DATA_TYPE sum_readind_unroll2(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    BM_DATA_TYPE sum1 = 0, sum2 = 0;
    int rep;
    ops_per_scan /= 2;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum1 += p1[0];
            sum2 += p1[step];
            p1 += xstep;
        }
    }
    sum1 += sum2;
    return (sum1);
}

BM_DATA_TYPE sum_readind_unroll4(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    int rep;
    int step2 = step + step;
    int step3 = step2 + step;
    int step4 = step3 + step;
    BM_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    ops_per_scan /= 4;
    for (rep = 0; rep < repeat; rep++) {
        if (p->m_NUM_INDEX == 1) {
            BM_DATA_TYPE *p1 = mat;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                sum2 += p1[step];
                sum3 += p1[step2];
                sum4 += p1[step3];
                p1 += xstep;
            }
        }
        if (p->m_NUM_INDEX == 2) {
            BM_DATA_TYPE *p1 = mat;
            BM_DATA_TYPE *p2 = mat + step2;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                sum2 += p1[step];
                p2 += xstep;
                sum3 += p2[0];
                sum4 += p2[step];
                p2 += xstep;
            }
        }
    }
    sum1 += sum2;
    sum3 += sum4;
    sum1 += sum3;
    return (sum1);
}

BM_DATA_TYPE sum_readind_unroll8(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    BM_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0;
    int rep;
    int step2 = step + step;
    int step3 = step2 + step;
    int step4 = step3 + step;
    int step5 = step4 + step;
    int step6 = step5 + step;
    int step7 = step6 + step;
    ops_per_scan /= 8;
    for (rep = 0; rep < repeat; rep++) {
        if (p->m_NUM_INDEX == 1) {
            BM_DATA_TYPE *p1 = mat;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                sum2 += p1[step];
                sum3 += p1[step2];
                sum4 += p1[step3];
                sum5 += p1[step4];
                sum6 += p1[step5];
                sum7 += p1[step6];
                sum8 += p1[step7];
                p1 += xstep;
            }
        }
        if (p->m_NUM_INDEX == 2) {
            BM_DATA_TYPE *p1 = mat;
            BM_DATA_TYPE *p2 = mat + step4;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                sum2 += p1[step];
                sum3 += p1[step2];
                sum4 += p1[step3];
                p1 += xstep;
                sum5 += p2[0];
                sum6 += p2[step];
                sum7 += p2[step2];
                sum8 += p2[step3];
                p2 += xstep;
            }
        }
        if (p->m_NUM_INDEX == 4) {
            BM_DATA_TYPE *p1 = mat;
            BM_DATA_TYPE *p2 = p1 + step * 2;
            BM_DATA_TYPE *p3 = p2 + step * 2;
            BM_DATA_TYPE *p4 = p3 + step * 2;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                sum2 += p1[step];
                p1 += xstep;
                sum3 += p2[0];
                sum4 += p2[step];
                p2 += xstep;
                sum5 += p3[0];
                sum6 += p3[step];
                p3 += xstep;
                sum7 += p4[0];
                sum8 += p4[step];
                p4 += xstep;
            }
        }
    }
    sum1 += sum2;
    sum3 += sum4;
    sum5 += sum6;
    sum7 += sum8;
    sum1 += sum3;
    sum5 += sum7;
    sum1 += sum5;
    return (sum1);
}





//
//                                   WRITE
//                                   WRITE
//                                   WRITE
//                                   WRITE
//


BM_DATA_TYPE sum_write_unroll1(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps;
    BM_DATA_TYPE sum = 0;
    BM_DATA_TYPE wr = towrite;
    int rep;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            *p1 += wr;
            p1 += step;
        }
    }
    return (sum);
}


BM_DATA_TYPE sum_write_unroll2(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    BM_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 2;
    BM_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            *p1 += wr;
            p1 += xstep;
            sum += *p2;
            *p2 += wr;
            p2 += xstep;
        }
    }
    return (sum);
}


BM_DATA_TYPE sum_write_unroll4(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    BM_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 4;
    BM_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        BM_DATA_TYPE *p3 = p2 + step;
        BM_DATA_TYPE *p4 = p3 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            *p1 += wr;
            p1 += xstep;
            sum += *p2;
            *p2 += wr;
            p2 += xstep;
            sum += *p3;
            *p3 += wr;
            p3 += xstep;
            sum += *p4;
            *p4 += wr;
            p4 += xstep;
        }
    }
    return (sum);
}


BM_DATA_TYPE sum_write_unroll8(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    BM_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 8;
    BM_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        BM_DATA_TYPE *p3 = p2 + step;
        BM_DATA_TYPE *p4 = p3 + step;
        BM_DATA_TYPE *p5 = p4 + step;
        BM_DATA_TYPE *p6 = p5 + step;
        BM_DATA_TYPE *p7 = p6 + step;
        BM_DATA_TYPE *p8 = p7 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            *p1 += wr;
            p1 += xstep;
            sum += *p2;
            *p2 += wr;
            p2 += xstep;
            sum += *p3;
            *p3 += wr;
            p3 += xstep;
            sum += *p4;
            *p4 += wr;
            p4 += xstep;
            sum += *p5;
            *p5 += wr;
            p5 += xstep;
            sum += *p6;
            *p6 += wr;
            p6 += xstep;
            sum += *p7;
            *p7 += wr;
            p7 += xstep;
            sum += *p8;
            *p8 += wr;
            p8 += xstep;
        }
    }
    return (sum);
}

// ------------ SPE

BM_DATA_TYPE sum_writespe_unroll2(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    BM_DATA_TYPE sum1 = 0, sum2 = 0;
    int rep;
    ops_per_scan /= 2;
    BM_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum1 += *p1;
            *p1 += wr;
            p1 += xstep;
            sum2 += *p2;
            *p2 += wr;
            p2 += xstep;
        }
    }
    sum1 += sum2;
    return (sum1);
}

BM_DATA_TYPE sum_writespe_unroll4(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    BM_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    int rep;
    ops_per_scan /= 4;
    BM_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        BM_DATA_TYPE *p3 = p2 + step;
        BM_DATA_TYPE *p4 = p3 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum1 += *p1;
            *p1 += wr;
            p1 += xstep;
            sum2 += *p2;
            *p2 += wr;
            p2 += xstep;
            sum3 += *p3;
            *p3 += wr;
            p3 += xstep;
            sum4 += *p4;
            *p4 += wr;
            p4 += xstep;
        }
    }
    sum1 += sum2;
    sum3 += sum4;
    sum1 += sum3;
    return (sum1);
}

BM_DATA_TYPE sum_writespe_unroll8(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    BM_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0;
    int rep;
    ops_per_scan /= 8;
    BM_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        BM_DATA_TYPE *p2 = p1 + step;
        BM_DATA_TYPE *p3 = p2 + step;
        BM_DATA_TYPE *p4 = p3 + step;
        BM_DATA_TYPE *p5 = p4 + step;
        BM_DATA_TYPE *p6 = p5 + step;
        BM_DATA_TYPE *p7 = p6 + step;
        BM_DATA_TYPE *p8 = p7 + step;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum1 += *p1;
            *p1 += wr;
            p1 += xstep;
            sum2 += *p2;
            *p2 += wr;
            p2 += xstep;
            sum3 += *p3;
            *p3 += wr;
            p3 += xstep;
            sum4 += *p4;
            *p4 += wr;
            p4 += xstep;
            sum5 += *p5;
            *p5 += wr;
            p5 += xstep;
            sum6 += *p6;
            *p6 += wr;
            p6 += xstep;
            sum7 += *p7;
            *p7 += wr;
            p7 += xstep;
            sum8 += *p8;
            *p8 += wr;
            p8 += xstep;
        }
    }
    sum1 += sum2;
    sum3 += sum4;
    sum5 += sum6;
    sum7 += sum8;
    sum1 += sum3;
    sum5 += sum7;
    sum1 += sum5;
    return (sum1);
}

//-------------------- INDEXED


BM_DATA_TYPE sum_writeind_unroll2(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    BM_DATA_TYPE sum1 = 0, sum2 = 0;
    int rep;
    ops_per_scan /= 2;
    BM_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        BM_DATA_TYPE *p1 = mat;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum1 += p1[0];
            p1[0] += wr;
            sum2 += p1[step];
            p1[step] += wr;
            p1 += xstep;
        }
    }
    sum1 += sum2;
    return (sum1);
}

BM_DATA_TYPE sum_writeind_unroll4(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    int rep;
    int step2 = step + step;
    int step3 = step2 + step;
    int step4 = step3 + step;
    BM_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    ops_per_scan /= 4;
    BM_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        if (p->m_NUM_INDEX == 1) {
            BM_DATA_TYPE *p1 = mat;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                p1[0] += wr;
                sum2 += p1[step];
                p1[step] += wr;
                sum3 += p1[step2];
                p1[step2] += wr;
                sum4 += p1[step3];
                p1[step3] += wr;
                p1 += xstep;
            }
        }
        if (p->m_NUM_INDEX == 2) {
            BM_DATA_TYPE *p1 = mat;
            BM_DATA_TYPE *p2 = mat + step2;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                p1[0] += wr;
                sum2 += p1[step];
                p1[step] += wr;
                p2 += xstep;
                sum3 += p2[0];
                p2[0] += wr;
                sum4 += p2[step];
                p2[step] += wr;
                p2 += xstep;
            }
        }
    }
    sum1 += sum2;
    sum3 += sum4;
    sum1 += sum3;
    return (sum1);
}

BM_DATA_TYPE sum_writeind_unroll8(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    BM_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0;
    int rep;
    int step2 = step + step;
    int step3 = step2 + step;
    int step4 = step3 + step;
    int step5 = step4 + step;
    int step6 = step5 + step;
    int step7 = step6 + step;
    ops_per_scan /= 8;
    BM_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        if (p->m_NUM_INDEX == 1) {
            BM_DATA_TYPE *p1 = mat;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                p1[0] += wr;
                sum2 += p1[step];
                p1[step] += wr;
                sum3 += p1[step2];
                p1[step2] += wr;
                sum4 += p1[step3];
                p1[step3] += wr;
                sum5 += p1[step4];
                p1[step4] += wr;
                sum6 += p1[step5];
                p1[step5] += wr;
                sum7 += p1[step6];
                p1[step6] += wr;
                sum8 += p1[step7];
                p1[step7] += wr;
                p1 += xstep;
            }
        }
        if (p->m_NUM_INDEX == 2) {
            BM_DATA_TYPE *p1 = mat;
            BM_DATA_TYPE *p2 = mat + step4;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                p1[0] += wr;
                sum2 += p1[step];
                p1[step] += wr;
                sum3 += p1[step2];
                p1[step2] += wr;
                sum4 += p1[step3];
                p1[step3] += wr;
                p1 += xstep;
                sum5 += p2[0];
                p2[0] += wr;
                sum6 += p2[step];
                p2[step] += wr;
                sum7 += p2[step2];
                p2[step2] += wr;
                sum8 += p2[step3];
                p2[step3] += wr;
                p2 += xstep;
            }
        }
        if (p->m_NUM_INDEX == 4) {
            BM_DATA_TYPE *p1 = mat;
            BM_DATA_TYPE *p2 = p1 + step * 2;
            BM_DATA_TYPE *p3 = p2 + step * 2;
            BM_DATA_TYPE *p4 = p3 + step * 2;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                p1[0] += wr;
                sum2 += p1[step];
                p1[step] += wr;
                p1 += xstep;
                sum3 += p2[0];
                p2[0] += wr;
                sum4 += p2[step];
                p2[step] += wr;
                p2 += xstep;
                sum5 += p3[0];
                p3[0] += wr;
                sum6 += p3[step];
                p3[step] += wr;
                p3 += xstep;
                sum7 += p4[0];
                p4[0] += wr;
                sum8 += p4[step];
                p4[step] += wr;
                p4 += xstep;
            }
        }
    }
    sum1 += sum2;
    sum3 += sum4;
    sum5 += sum6;
    sum7 += sum8;
    sum1 += sum3;
    sum5 += sum7;
    sum1 += sum5;
    return (sum1);
}


double work_part2(bm_parameters *p, uint64_t max_index, int step) {
    int repeat;
    THEINT ops_per_scan = max_index / step;
    if (ops_per_scan < MIN_OPS_PER_SCAN)return (BIG_VAL);
    repeat = p->m_MAX_OPS / ops_per_scan;
    if (repeat == 0)repeat = 1;
    //towrite = 0.0;

//    printf ("\n_work_part2_ maxindex     %d \n", max_index);
//    printf ("_work_part2_ step           %d \n", step);
//    printf ("_work_part2_ ops_per_scan   %d \n", ops_per_scan);
//    printf ("_work_part2_ Repeat         %d \n", ops_per_scan);
//    exit (-1);

    //todo: if normal bench then
    if (p->m_type == BENCH_TYPE::READ) {
        if (p->m_mode == BENCH_MODE::NORMAL) {
            if (p->m_UNROLL == UNROLL1) {
                return (sum_read_unroll1(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL2) {
                return (sum_read_unroll2(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL4) {
                return (sum_read_unroll4(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL8) {
                return (sum_read_unroll8(p, max_index, step, repeat, ops_per_scan));
            }
        }
        if (p->m_mode == BENCH_MODE::SPECIAL) {
            if (p->m_UNROLL == UNROLL1) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
//                return (sum_readspe_unroll1(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL2) {
                return (sum_readspe_unroll2(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL4) {
                return (sum_readspe_unroll4(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL8) {
                return (sum_readspe_unroll8(p, max_index, step, repeat, ops_per_scan));
            }
        }

        if (p->m_mode == BENCH_MODE::INDEXED) {
            if (p->m_UNROLL == UNROLL1) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
//                return (sum_readind_unroll1(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL2) {
                return (sum_readind_unroll2(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL4) {
                return (sum_readind_unroll4(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL8) {
                return (sum_readind_unroll8(p, max_index, step, repeat, ops_per_scan));
            }
        }
    } else if (p->m_type == BENCH_TYPE::WRITE) {
        if (p->m_mode == BENCH_MODE::NORMAL) {
            if (p->m_UNROLL == UNROLL1) {
                return (sum_write_unroll1(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL2) {
                return (sum_write_unroll2(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL4) {
                return (sum_write_unroll4(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL8) {
                return (sum_write_unroll8(p, max_index, step, repeat, ops_per_scan));
            }
        }
        if (p->m_mode == BENCH_MODE::SPECIAL) {
            if (p->m_UNROLL == UNROLL1) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
//                return (sum_writespe_unroll1(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL2) {
                return (sum_writespe_unroll2(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL4) {
                return (sum_writespe_unroll4(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL8) {
                return (sum_writespe_unroll8(p, max_index, step, repeat, ops_per_scan));
            }
        }

        if (p->m_mode == BENCH_MODE::INDEXED) {
            if (p->m_UNROLL == UNROLL1) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
//                return (sum_writeind_unroll1(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL2) {
                return (sum_writeind_unroll2(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL4) {
                return (sum_writeind_unroll4(p, max_index, step, repeat, ops_per_scan));
            } else if (p->m_UNROLL == UNROLL8) {
                return (sum_writeind_unroll8(p, max_index, step, repeat, ops_per_scan));
            }
        }
    }
    return (0.0);
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

