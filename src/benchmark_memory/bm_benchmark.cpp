//
// Created by Jean Pourroy on 13/04/2018.
//

#include "bm_benchmark.h"



BM_DATA_TYPE sum_read_unroll1(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
//    printf ("\n");
//    printf ("_unroll_1_ Mat            %p \n", mat);
//    printf ("_unroll_1_ Mat[0]         %f \n", mat[0]);
//    cout << "\n COUCOU unroll 1 - mat : \n" << flush;

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
