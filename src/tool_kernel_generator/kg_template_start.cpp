#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <iomanip>
#include <utility>
#include <fstream>
#include <inttypes.h>
#include <sched.h>

float check_frequency ();

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
    uint64_t cycle_total, instructions_total, instructions_executed, instructions_executed_total, loop_nb_instruction, nb_total_loop_iteration = 0;
    double IPC, inst_second = 0.0;
    int i;
    double timeStart, timeEnd, time_total;
    std::pair<int, double> *pairArr = new pair<int, double>[NB_lOOP];
    float Base_vs_Current_freq = 1;
    double flop_cycle_sp ,flop_cycle_dp ,flops_sp ,flops_dp;

    std::cout.precision(3);



    //    Let's bind the process to a particular core
    cpu_set_t mycpumask;
    CPU_ZERO(&mycpumask);
    CPU_SET(CPU_BIND, &mycpumask);
    sched_setaffinity(0, sizeof(cpu_set_t), &mycpumask);
    /* double-check */
    sched_getaffinity(0, sizeof(cpu_set_t), &mycpumask);
    for (i = 0; i < sysconf(_SC_NPROCESSORS_CONF); i++) {
        if (CPU_ISSET(i, &mycpumask))
            printf("+ Running on CPU #%d\n", i);
    };

    for (i = 0; i < NB_lOOP; i++) {
        timeStart = mygettime();
        cycleInStart = rdtsc();
