#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <iomanip>
#include <utility>
#include <fstream>

/*
 *   unsigned long long int CUMUL_CYCLE = 0;
        double CUMUL_TIME = 0;

        __asm__ ("myBench: ");
        cycleInStart = rdtsc();
        timeStart = mygettime();

        __asm__(
                "vmulpd %xmm0, %xmm1, %xmm2; "
                "vmulpd %xmm0, %xmm1, %xmm3; "
                "vmulpd %xmm0, %xmm1, %xmm4; "
                "vmulpd %xmm0, %xmm1, %xmm5; "
                "vmulpd %xmm0, %xmm1, %xmm6; "
                "vmulpd %xmm0, %xmm1, %xmm7; "
                "vmulpd %xmm0, %xmm1, %xmm8; "
                "vmulpd %xmm0, %xmm1, %xmm9; "
                "vmulpd %xmm0, %xmm1, %xmm10; "
                "vmulpd %xmm0, %xmm1, %xmm11; ");

        cycleInEnd = rdtsc();
        timeEnd = mygettime();

        CUMUL_CYCLE += cycleInEnd - cycleInStart;
        CUMUL_TIME  += timeEnd - timeStart;
        __asm__(
                "  sub    $0x1, %%eax;"
                "  jnz    myBench" : : "a" (200000));//--------------
        pairArr[i] = make_pair(CUMUL_CYCLE, CUMUL_TIME);
 */


uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t) hi << 32 | lo;
}

#include <sys/time.h>
#include <string>

using namespace std;

double mygettime() {
    struct timeval tp;
    struct timezone tzp;
    int i;
    i = gettimeofday(&tp, &tzp);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1.e-6);
}

int main(int argc, char **argv) {
    unsigned long long int nbCycleIn;
    unsigned long long int cycleInStart, cycleInEnd;
    double timeStart, timeEnd, timeSpent;
    int i;
    double ipc;
    std::pair<int, double> *pairArr = new pair<int, double>[1000000];

    for (i = 0; i < NB_lOOP; i++) {
        timeStart = mygettime();
        cycleInStart = rdtsc();
