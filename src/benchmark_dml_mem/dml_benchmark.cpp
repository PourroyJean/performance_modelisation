//
// Created by Jean Pourroy on 13/04/2018.
//

#include "dml_benchmark.h"


DML_DATA_TYPE sum_read_unroll1(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
//    printf ("\n");
//    printf ("_unroll_1_ Mat            %p \n", mat);
//    printf ("_unroll_1_ Mat[0]         %f \n", mat[0]);
//    cout << "\n COUCOU unroll 1 - mat : \n" << flush;

    THEINT steps;
    DML_DATA_TYPE sum = 0;
    int rep;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            p1 += step;
        }
    }
    return (sum);
}


DML_DATA_TYPE sum_read_unroll2(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    int rep;
    DML_DATA_TYPE sum = 0;
    ops_per_scan /= 2;
//    printf ("\n_sum_read_unroll2_ step           %d \n", step);
//    printf ("_sum_read_unroll2_ xstep          %d \n", xstep);
//    printf ("_sum_read_unroll2_ ops_per_scan   %d \n", ops_per_scan);
//    printf ("_sum_read_unroll2_ repeat         %d \n", repeat);

    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
//        cout << rep << " (" <<  mat[0] << ") " <<  flush;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            p1 += xstep;
            sum += *p2;
            p2 += xstep;
        }
    }
//    exit (-1);

    return (sum);
}


DML_DATA_TYPE sum_read_unroll4(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    DML_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 4;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
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

//
DML_DATA_TYPE sum_read_unroll8(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    DML_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 8;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
        DML_DATA_TYPE *p5 = p4 + step;
        DML_DATA_TYPE *p6 = p5 + step;
        DML_DATA_TYPE *p7 = p6 + step;
        DML_DATA_TYPE *p8 = p7 + step;
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
//
//DML_DATA_TYPE sum_read_unroll8(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
//    THEINT steps, xstep = 8 * 8;
//    DML_DATA_TYPE sum = 0;
//    int rep;
//    ops_per_scan /= 8;
//    for (rep = 0; rep < repeat; rep++) {
//        DML_DATA_TYPE *p1 = mat;
//        DML_DATA_TYPE *p2 = p1 + 8;
//        DML_DATA_TYPE *p3 = p2 + 8;
//        DML_DATA_TYPE *p4 = p3 + 8;
//        DML_DATA_TYPE *p5 = p4 + 8;
//        DML_DATA_TYPE *p6 = p5 + 8;
//        DML_DATA_TYPE *p7 = p6 + 8;
//        DML_DATA_TYPE *p8 = p7 + 8;
//        for (steps = 0; steps < ops_per_scan; steps++) {
//            sum += *p1;
//            p1 += xstep;
//            sum += *p2;
//            p2 += xstep;
//            sum += *p3;
//            p3 += xstep;
//            sum += *p4;
//            p4 += xstep;
//            sum += *p5;
//            p5 += xstep;
//            sum += *p6;
//            p6 += xstep;
//            sum += *p7;
//            p7 += xstep;
//            sum += *p8;
//            p8 += xstep;
//        }
//    }
//    return (sum);
//}

// ------------ SPE

DML_DATA_TYPE sum_readspe_unroll2(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    DML_DATA_TYPE sum1 = 0, sum2 = 0;
    int rep;
    ops_per_scan /= 2;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
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

DML_DATA_TYPE sum_readspe_unroll4(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    int rep;
    ops_per_scan /= 4;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
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

DML_DATA_TYPE sum_readspe_unroll8(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0;
    int rep;
    ops_per_scan /= 8;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
        DML_DATA_TYPE *p5 = p4 + step;
        DML_DATA_TYPE *p6 = p5 + step;
        DML_DATA_TYPE *p7 = p6 + step;
        DML_DATA_TYPE *p8 = p7 + step;
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

DML_DATA_TYPE sum_readspe_unroll16(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 16;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0, sum9 = 0, sum10 = 0, sum11 = 0, sum12 = 0, sum13 = 0, sum14 = 0, sum15 = 0, sum16 = 0;
    int rep;
    ops_per_scan /= 16;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
        DML_DATA_TYPE *p5 = p4 + step;
        DML_DATA_TYPE *p6 = p5 + step;
        DML_DATA_TYPE *p7 = p6 + step;
        DML_DATA_TYPE *p8 = p7 + step;
        DML_DATA_TYPE *p9 = p8 + step;
        DML_DATA_TYPE *p10 = p9 + step;
        DML_DATA_TYPE *p11 = p10 + step;
        DML_DATA_TYPE *p12 = p11 + step;
        DML_DATA_TYPE *p13 = p12 + step;
        DML_DATA_TYPE *p14 = p13 + step;
        DML_DATA_TYPE *p15 = p14 + step;
        DML_DATA_TYPE *p16 = p15 + step;

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

            sum9 += *p9;
            p9 += xstep;
            sum10 += *p10;
            p10 += xstep;
            sum11 += *p11;
            p11 += xstep;
            sum12 += *p12;
            p12 += xstep;
            sum13 += *p13;
            p13 += xstep;
            sum14 += *p14;
            p14 += xstep;
            sum15 += *p15;
            p15 += xstep;
            sum16 += *p16;
            p16 += xstep;

        }
    }
    return (sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7 + sum8 + sum9 + sum10 + sum11 + sum12 + sum13 + sum14 +
            sum15 + sum16);
}


DML_DATA_TYPE sum_readspe_unroll32(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 32;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0, sum9 = 0, sum10 = 0, sum11 = 0, sum12 = 0, sum13 = 0, sum14 = 0, sum15 = 0, sum16 = 0, sum17 = 0, sum18 = 0, sum19 = 0, sum20 = 0, sum21 = 0, sum22 = 0, sum23 = 0, sum24 = 0, sum25 = 0, sum26 = 0, sum27 = 0, sum28 = 0, sum29 = 0, sum30 = 0, sum31 = 0, sum32 = 0;
    int rep;
    ops_per_scan /= 32;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
        DML_DATA_TYPE *p5 = p4 + step;
        DML_DATA_TYPE *p6 = p5 + step;
        DML_DATA_TYPE *p7 = p6 + step;
        DML_DATA_TYPE *p8 = p7 + step;
        DML_DATA_TYPE *p9 = p8 + step;
        DML_DATA_TYPE *p10 = p9 + step;
        DML_DATA_TYPE *p11 = p10 + step;
        DML_DATA_TYPE *p12 = p11 + step;
        DML_DATA_TYPE *p13 = p12 + step;
        DML_DATA_TYPE *p14 = p13 + step;
        DML_DATA_TYPE *p15 = p14 + step;
        DML_DATA_TYPE *p16 = p15 + step;
        DML_DATA_TYPE *p17 = p16 + step;
        DML_DATA_TYPE *p18 = p17 + step;
        DML_DATA_TYPE *p19 = p18 + step;
        DML_DATA_TYPE *p20 = p19 + step;
        DML_DATA_TYPE *p21 = p20 + step;
        DML_DATA_TYPE *p22 = p21 + step;
        DML_DATA_TYPE *p23 = p22 + step;
        DML_DATA_TYPE *p24 = p23 + step;
        DML_DATA_TYPE *p25 = p24 + step;
        DML_DATA_TYPE *p26 = p25 + step;
        DML_DATA_TYPE *p27 = p26 + step;
        DML_DATA_TYPE *p28 = p27 + step;
        DML_DATA_TYPE *p29 = p28 + step;
        DML_DATA_TYPE *p30 = p29 + step;
        DML_DATA_TYPE *p31 = p30 + step;
        DML_DATA_TYPE *p32 = p31 + step;

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
            sum9 += *p9;
            p9 += xstep;
            sum10 += *p10;
            p10 += xstep;
            sum11 += *p11;
            p11 += xstep;
            sum12 += *p12;
            p12 += xstep;
            sum13 += *p13;
            p13 += xstep;
            sum14 += *p14;
            p14 += xstep;
            sum15 += *p15;
            p15 += xstep;
            sum16 += *p16;
            p16 += xstep;
            sum17 += *p17;
            p17 += xstep;
            sum18 += *p18;
            p18 += xstep;
            sum19 += *p19;
            p19 += xstep;
            sum20 += *p20;
            p20 += xstep;
            sum21 += *p21;
            p21 += xstep;
            sum22 += *p22;
            p22 += xstep;
            sum23 += *p23;
            p23 += xstep;
            sum24 += *p24;
            p24 += xstep;
            sum25 += *p25;
            p25 += xstep;
            sum26 += *p26;
            p26 += xstep;
            sum27 += *p27;
            p27 += xstep;
            sum28 += *p28;
            p28 += xstep;
            sum29 += *p29;
            p29 += xstep;
            sum30 += *p30;
            p30 += xstep;
            sum31 += *p31;
            p31 += xstep;
            sum32 += *p32;
            p32 += xstep;


        }
    }
    return (sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7 + sum8 + sum9 + sum10 + sum11 + sum12 + sum13 + sum14 + sum15 + sum16 + sum17 + sum18 + sum19 + sum20 + sum21 + sum22 + sum23 + sum24 + sum25 + sum26 + sum27 + sum28 + sum29 + sum30 + sum31 + sum32);
}



DML_DATA_TYPE sum_readspe_unroll64(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 64;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0, sum9 = 0, sum10 = 0, sum11 = 0, sum12 = 0, sum13 = 0, sum14 = 0, sum15 = 0, sum16 = 0, sum17 = 0, sum18 = 0, sum19 = 0, sum20 = 0, sum21 = 0, sum22 = 0, sum23 = 0, sum24 = 0, sum25 = 0, sum26 = 0, sum27 = 0, sum28 = 0, sum29 = 0, sum30 = 0, sum31 = 0, sum32 = 0, sum33 = 0, sum34 = 0, sum35 = 0, sum36 = 0, sum37 = 0, sum38 = 0, sum39 = 0, sum40 = 0, sum41 = 0, sum42 = 0, sum43 = 0, sum44 = 0, sum45 = 0, sum46 = 0, sum47 = 0, sum48 = 0, sum49 = 0, sum50 = 0, sum51 = 0, sum52 = 0, sum53 = 0, sum54 = 0, sum55 = 0, sum56 = 0, sum57 = 0, sum58 = 0, sum59 = 0, sum60 = 0, sum61 = 0, sum62 = 0, sum63 = 0, sum64 = 0;
    int rep;
    ops_per_scan /= 64;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
        DML_DATA_TYPE *p5 = p4 + step;
        DML_DATA_TYPE *p6 = p5 + step;
        DML_DATA_TYPE *p7 = p6 + step;
        DML_DATA_TYPE *p8 = p7 + step;
        DML_DATA_TYPE *p9 = p8 + step;
        DML_DATA_TYPE *p10 = p9 + step;
        DML_DATA_TYPE *p11 = p10 + step;
        DML_DATA_TYPE *p12 = p11 + step;
        DML_DATA_TYPE *p13 = p12 + step;
        DML_DATA_TYPE *p14 = p13 + step;
        DML_DATA_TYPE *p15 = p14 + step;
        DML_DATA_TYPE *p16 = p15 + step;
        DML_DATA_TYPE *p17 = p16 + step;
        DML_DATA_TYPE *p18 = p17 + step;
        DML_DATA_TYPE *p19 = p18 + step;
        DML_DATA_TYPE *p20 = p19 + step;
        DML_DATA_TYPE *p21 = p20 + step;
        DML_DATA_TYPE *p22 = p21 + step;
        DML_DATA_TYPE *p23 = p22 + step;
        DML_DATA_TYPE *p24 = p23 + step;
        DML_DATA_TYPE *p25 = p24 + step;
        DML_DATA_TYPE *p26 = p25 + step;
        DML_DATA_TYPE *p27 = p26 + step;
        DML_DATA_TYPE *p28 = p27 + step;
        DML_DATA_TYPE *p29 = p28 + step;
        DML_DATA_TYPE *p30 = p29 + step;
        DML_DATA_TYPE *p31 = p30 + step;
        DML_DATA_TYPE *p32 = p31 + step;
        DML_DATA_TYPE *p33 = p32 + step;
        DML_DATA_TYPE *p34 = p33 + step;
        DML_DATA_TYPE *p35 = p34 + step;
        DML_DATA_TYPE *p36 = p35 + step;
        DML_DATA_TYPE *p37 = p36 + step;
        DML_DATA_TYPE *p38 = p37 + step;
        DML_DATA_TYPE *p39 = p38 + step;
        DML_DATA_TYPE *p40 = p39 + step;
        DML_DATA_TYPE *p41 = p40 + step;
        DML_DATA_TYPE *p42 = p41 + step;
        DML_DATA_TYPE *p43 = p42 + step;
        DML_DATA_TYPE *p44 = p43 + step;
        DML_DATA_TYPE *p45 = p44 + step;
        DML_DATA_TYPE *p46 = p45 + step;
        DML_DATA_TYPE *p47 = p46 + step;
        DML_DATA_TYPE *p48 = p47 + step;
        DML_DATA_TYPE *p49 = p48 + step;
        DML_DATA_TYPE *p50 = p49 + step;
        DML_DATA_TYPE *p51 = p50 + step;
        DML_DATA_TYPE *p52 = p51 + step;
        DML_DATA_TYPE *p53 = p52 + step;
        DML_DATA_TYPE *p54 = p53 + step;
        DML_DATA_TYPE *p55 = p54 + step;
        DML_DATA_TYPE *p56 = p55 + step;
        DML_DATA_TYPE *p57 = p56 + step;
        DML_DATA_TYPE *p58 = p57 + step;
        DML_DATA_TYPE *p59 = p58 + step;
        DML_DATA_TYPE *p60 = p59 + step;
        DML_DATA_TYPE *p61 = p60 + step;
        DML_DATA_TYPE *p62 = p61 + step;
        DML_DATA_TYPE *p63 = p62 + step;
        DML_DATA_TYPE *p64 = p63 + step;

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
            sum9 += *p9;
            p9 += xstep;
            sum10 += *p10;
            p10 += xstep;
            sum11 += *p11;
            p11 += xstep;
            sum12 += *p12;
            p12 += xstep;
            sum13 += *p13;
            p13 += xstep;
            sum14 += *p14;
            p14 += xstep;
            sum15 += *p15;
            p15 += xstep;
            sum16 += *p16;
            p16 += xstep;
            sum17 += *p17;
            p17 += xstep;
            sum18 += *p18;
            p18 += xstep;
            sum19 += *p19;
            p19 += xstep;
            sum20 += *p20;
            p20 += xstep;
            sum21 += *p21;
            p21 += xstep;
            sum22 += *p22;
            p22 += xstep;
            sum23 += *p23;
            p23 += xstep;
            sum24 += *p24;
            p24 += xstep;
            sum25 += *p25;
            p25 += xstep;
            sum26 += *p26;
            p26 += xstep;
            sum27 += *p27;
            p27 += xstep;
            sum28 += *p28;
            p28 += xstep;
            sum29 += *p29;
            p29 += xstep;
            sum30 += *p30;
            p30 += xstep;
            sum31 += *p31;
            p31 += xstep;
            sum32 += *p32;
            p32 += xstep;
            sum33 += *p33;
            p33 += xstep;
            sum34 += *p34;
            p34 += xstep;
            sum35 += *p35;
            p35 += xstep;
            sum36 += *p36;
            p36 += xstep;
            sum37 += *p37;
            p37 += xstep;
            sum38 += *p38;
            p38 += xstep;
            sum39 += *p39;
            p39 += xstep;
            sum40 += *p40;
            p40 += xstep;
            sum41 += *p41;
            p41 += xstep;
            sum42 += *p42;
            p42 += xstep;
            sum43 += *p43;
            p43 += xstep;
            sum44 += *p44;
            p44 += xstep;
            sum45 += *p45;
            p45 += xstep;
            sum46 += *p46;
            p46 += xstep;
            sum47 += *p47;
            p47 += xstep;
            sum48 += *p48;
            p48 += xstep;
            sum49 += *p49;
            p49 += xstep;
            sum50 += *p50;
            p50 += xstep;
            sum51 += *p51;
            p51 += xstep;
            sum52 += *p52;
            p52 += xstep;
            sum53 += *p53;
            p53 += xstep;
            sum54 += *p54;
            p54 += xstep;
            sum55 += *p55;
            p55 += xstep;
            sum56 += *p56;
            p56 += xstep;
            sum57 += *p57;
            p57 += xstep;
            sum58 += *p58;
            p58 += xstep;
            sum59 += *p59;
            p59 += xstep;
            sum60 += *p60;
            p60 += xstep;
            sum61 += *p61;
            p61 += xstep;
            sum62 += *p62;
            p62 += xstep;
            sum63 += *p63;
            p63 += xstep;
            sum64 += *p64;
            p64 += xstep;

        }
    }
    return (sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7 + sum8 + sum9 + sum10 + sum11 + sum12 + sum13 + sum14 + sum15 + sum16 + sum17 + sum18 + sum19 + sum20 + sum21 + sum22 + sum23 + sum24 + sum25 + sum26 + sum27 + sum28 + sum29 + sum30 + sum31 + sum32 + sum33 + sum34 + sum35 + sum36 + sum37 + sum38 + sum39 + sum40 + sum41 + sum42 + sum43 + sum44 + sum45 + sum46 + sum47 + sum48 + sum49 + sum50 + sum51 + sum52 + sum53 + sum54 + sum55 + sum56 + sum57 + sum58 + sum59 + sum60 + sum61 + sum62 + sum63 + sum64);
}

//-------------------- INDEXED


DML_DATA_TYPE sum_readind_unroll2(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    DML_DATA_TYPE sum1 = 0, sum2 = 0;
    int rep;
    ops_per_scan /= 2;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum1 += p1[0];
            sum2 += p1[step];
            p1 += xstep;
        }
    }
    sum1 += sum2;
    return (sum1);
}

DML_DATA_TYPE sum_readind_unroll4(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    int rep;
    int step2 = step + step;
    int step3 = step2 + step;
    int step4 = step3 + step;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    ops_per_scan /= 4;
    for (rep = 0; rep < repeat; rep++) {
        if (p->m_NUM_INDEX == 1) {
            DML_DATA_TYPE *p1 = mat;
            for (steps = 0; steps < ops_per_scan; steps++) {
                sum1 += p1[0];
                sum2 += p1[step];
                sum3 += p1[step2];
                sum4 += p1[step3];
                p1 += xstep;
            }
        }
        if (p->m_NUM_INDEX == 2) {
            DML_DATA_TYPE *p1 = mat;
            DML_DATA_TYPE *p2 = mat + step2;
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

DML_DATA_TYPE sum_readind_unroll8(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0;
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
            DML_DATA_TYPE *p1 = mat;
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
            DML_DATA_TYPE *p1 = mat;
            DML_DATA_TYPE *p2 = mat + step4;
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
            DML_DATA_TYPE *p1 = mat;
            DML_DATA_TYPE *p2 = p1 + step * 2;
            DML_DATA_TYPE *p3 = p2 + step * 2;
            DML_DATA_TYPE *p4 = p3 + step * 2;
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


DML_DATA_TYPE sum_write_unroll1(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps;
    DML_DATA_TYPE sum = 0;
    DML_DATA_TYPE wr = towrite;
    int rep;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        for (steps = 0; steps < ops_per_scan; steps++) {
            sum += *p1;
            *p1 += wr;
            p1 += step;
        }
    }
    return (sum);
}


DML_DATA_TYPE sum_write_unroll2(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    DML_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 2;
    DML_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
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


DML_DATA_TYPE sum_write_unroll4(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    DML_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 4;
    DML_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
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


DML_DATA_TYPE sum_write_unroll8(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    DML_DATA_TYPE sum = 0;
    int rep;
    ops_per_scan /= 8;
    DML_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
        DML_DATA_TYPE *p5 = p4 + step;
        DML_DATA_TYPE *p6 = p5 + step;
        DML_DATA_TYPE *p7 = p6 + step;
        DML_DATA_TYPE *p8 = p7 + step;
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

DML_DATA_TYPE sum_writespe_unroll2(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    DML_DATA_TYPE sum1 = 0, sum2 = 0;
    int rep;
    ops_per_scan /= 2;
    DML_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
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

DML_DATA_TYPE sum_writespe_unroll4(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    int rep;
    ops_per_scan /= 4;
    DML_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
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

DML_DATA_TYPE sum_writespe_unroll8(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0;
    int rep;
    ops_per_scan /= 8;
    DML_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
        DML_DATA_TYPE *p2 = p1 + step;
        DML_DATA_TYPE *p3 = p2 + step;
        DML_DATA_TYPE *p4 = p3 + step;
        DML_DATA_TYPE *p5 = p4 + step;
        DML_DATA_TYPE *p6 = p5 + step;
        DML_DATA_TYPE *p7 = p6 + step;
        DML_DATA_TYPE *p8 = p7 + step;
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


DML_DATA_TYPE sum_writeind_unroll2(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 2;
    DML_DATA_TYPE sum1 = 0, sum2 = 0;
    int rep;
    ops_per_scan /= 2;
    DML_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        DML_DATA_TYPE *p1 = mat;
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

DML_DATA_TYPE sum_writeind_unroll4(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 4;
    int rep;
    int step2 = step + step;
    int step3 = step2 + step;
    int step4 = step3 + step;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    ops_per_scan /= 4;
    DML_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        if (p->m_NUM_INDEX == 1) {
            DML_DATA_TYPE *p1 = mat;
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
            DML_DATA_TYPE *p1 = mat;
            DML_DATA_TYPE *p2 = mat + step2;
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

DML_DATA_TYPE sum_writeind_unroll8(Dml_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan) {
    THEINT steps, xstep = step * 8;
    DML_DATA_TYPE sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0;
    int rep;
    int step2 = step + step;
    int step3 = step2 + step;
    int step4 = step3 + step;
    int step5 = step4 + step;
    int step6 = step5 + step;
    int step7 = step6 + step;
    ops_per_scan /= 8;
    DML_DATA_TYPE wr = towrite;
    for (rep = 0; rep < repeat; rep++) {
        if (p->m_NUM_INDEX == 1) {
            DML_DATA_TYPE *p1 = mat;
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
            DML_DATA_TYPE *p1 = mat;
            DML_DATA_TYPE *p2 = mat + step4;
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
            DML_DATA_TYPE *p1 = mat;
            DML_DATA_TYPE *p2 = p1 + step * 2;
            DML_DATA_TYPE *p3 = p2 + step * 2;
            DML_DATA_TYPE *p4 = p3 + step * 2;
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
