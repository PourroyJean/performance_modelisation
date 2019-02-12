//
// Created by Jean Pourroy on 12/02/2019.
//

#include <stdio.h>
#include <math.h>

typedef unsigned long long ui64;
#include <iostream>
#include <sys/time.h>
#include "code_annotation.h"


static ui64 dml_cycles() {
    ui64 eax, edx;
    __asm__ volatile ("rdtsc":"=a" (eax), "=d" (edx));
    return ((edx << 32) | eax);
}

static double dml_micros() {
    static struct timezone tz;
    static struct timeval tv;
    gettimeofday(&tv, &tz);
    return ((tv.tv_sec * 1000000.0) + tv.tv_usec);
}

double f1(double x) {
    double sum = 0.0;
    sum += (.43724046807752044936
            + x * (.13237657548034352332
                   - x * (.01360502604767418865
                          - x * (.01356762197010358089
                                 - x * (.00162372532314446528
                                        + x * (.00029705353733379691
                                               + x * (.00007943300879521470
                                                      + x * (.00000046556124614505
                                                             + x * (.00000143272516309551
                                                                    - x * (.00000010354847112313
                                                                           + x * (.00000001235792708386
                                                                                  + x * (.00000000178810838580
                                                                                         - x * (.00000000003391414390
    )))))))))))));
    return (sum);
}

double range_f1(double x, double step, int count) {
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += (.43724046807752044936
                + x * (.13237657548034352332
                       - x * (.01360502604767418865
                              - x * (.01356762197010358089
                                     - x * (.00162372532314446528
                                            + x * (.00029705353733379691
                                                   + x * (.00007943300879521470
                                                          + x * (.00000046556124614505
                                                                 + x * (.00000143272516309551
                                                                        - x * (.00000010354847112313
                                                                               + x * (.00000001235792708386
                                                                                      + x * (.00000000178810838580
                                                                                             -
                                                                                             x * (.00000000003391414390
                                                                                             )))))))))))));
        x += step;
    }
    return (sum);
}

int main() {
    const double pi2 = 3.1415926535897932385 * 2.0;
    double step = 1.0 / 1024.0 / 1024.0 / 1024.0 / 1024.0 / 1024.0;
    int block = 1024;
    int nblock = 1000000;
    double d1 = 0.1;
    double sum1 = 0.0;
    double sum2 = 0.0;
    ui64 ui1, ui2, ui3;
    yamb_annotate_set_event("f1", "blue");
    for (int i = 0; i < 10; ++ i) {
        d1 = 0.1;
        sum1 = 0.0;
        ui1 = dml_cycles();
        for (int i2 = 0; i2 < block; i2++)
            for (int i1 = 0; i1 < nblock; i1++) {
                sum1 += f1(d1);
                d1 += step;
            }
        ui2 = dml_cycles();
    }
    std::cout << "GO ? ";
//    char go;
//    std::cin >> go;
    yamb_annotate_set_event("range_f1", "red");
    for (int i = 0; i < 10; ++ i) {
        d1 = 0.1;
        sum2 = 0.0;
        for (int i1 = 0; i1 < nblock; i1++) {
            sum2 += range_f1(d1, step, block);
            d1 += step * block;
        }
        ui64 ui3 = dml_cycles();
    }
    yamb_annotate_set_event("end", "green");
    double cy1 = (double) (ui2 - ui1) / block / nblock;
    double cy2 = (double) (ui3 - ui2) / block / nblock;
    printf("_ %10.3lf %10.3lf %20.15lf %20.15lf %20.15lf %20.15lf\n", cy1, cy2, d1, sum1, sum2, sum2 - sum1);
}