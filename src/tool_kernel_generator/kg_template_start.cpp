#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <iomanip>
#include <utility>
#include <fstream>

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
    unsigned long long int cycle_total, instructions_total, instructions_executed, instructions_executed_total = 0;
    double IPC = 0.0;
    int i;
    double timeStart, timeEnd, timeSpent;
    std::pair<int, double> *pairArr = new pair<int, double>[1000000];

    for (i = 0; i < NB_lOOP; i++) {
        timeStart = mygettime();
        cycleInStart = rdtsc();
