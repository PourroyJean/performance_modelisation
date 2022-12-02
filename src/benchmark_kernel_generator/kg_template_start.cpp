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
#include <sys/time.h>
#include <string>
#include <vector>
#include "omp.h"

using namespace std;

struct bench_result {
    double base_frequency;
    double real_frequency;
    double bench_frequency;
    double time_total;
    uint64_t instructions;
    uint64_t cycles;
    double ipc;
    double flop_cycle_sp;
    double flop_cycle_dp;
    double flops_sp;
    double flops_dp;
};

void bench(bench_result* result);
float check_frequency(bench_result* result);

uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t) hi << 32 | lo;
}

double mygettime() {
    struct timeval tp;
    struct timezone tzp;
    int i;
    i = gettimeofday(&tp, &tzp);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1.e-6);
}

int main(int argc, char **argv) {

    cout.precision(3);

    bench_result global_res {
        .base_frequency = 0,
        .real_frequency = 0,
        .bench_frequency = 0,
        .time_total = 0,
        .instructions = 0,
        .cycles = 0,
        .ipc = 0,
        .flop_cycle_sp = 0,
        .flop_cycle_dp = 0,
        .flops_sp = 0,
        .flops_dp = 0,
    };

    if (argc >= 2) {

        int cpu_bind = stoi(argv[1]);
        cpu_set_t cpus;
        CPU_ZERO(&cpus);
        CPU_SET(cpu_bind, &cpus);
        sched_setaffinity(0, sizeof(cpu_set_t), &cpus);

        cout << "+ Bound to CPU " << cpu_bind << endl;

        bench(&global_res);

    } else {

        int num_threads = omp_get_max_threads();
        cout << "+ Number of OpenMP threads: " << num_threads << endl;

        #pragma omp parallel for
        for (int i = 0; i < num_threads; i++) {

            // We get the CPU affinity of the current thread.
            cpu_set_t cpus;
            CPU_ZERO(&cpus);
            sched_getaffinity(0, sizeof(cpu_set_t), &cpus);

            #pragma omp critical
            {
                int cpu_count = 0;
                for (int j = 0; j < CPU_SETSIZE; j++) {
                    if (CPU_ISSET(j, &cpus)) {
                        cout << "+ OpenMP thread " << i << " bound to CPU " << j << endl;
                        cpu_count++;
                    }
                }
                if (cpu_count > 1) {
                    cout << "+ /!\\ The OpenMP thread is running on more than one CPU." << endl;
                    cout << "       If you don't want to run it using OpenMP, give the core to bind" << endl;
                    cout << "       to as parameter. You can also use OMP_PLACES and OMP_PROC_BIND." << endl;
                }
            }

            // Initialized by bench function.
            bench_result res;
            bench(&res);

            #pragma omp critical
            {
                global_res.base_frequency += res.base_frequency;
                global_res.real_frequency += res.real_frequency;
                global_res.bench_frequency += res.bench_frequency;
                global_res.time_total += res.time_total;
                global_res.instructions += res.instructions;
                global_res.cycles += res.cycles;
                global_res.ipc += res.ipc;
                global_res.flop_cycle_sp += res.flop_cycle_sp;
                global_res.flop_cycle_dp += res.flop_cycle_dp;
                global_res.flops_sp += res.flops_sp;
                global_res.flops_dp += res.flops_dp;
            }

        }

        global_res.base_frequency /= (double) num_threads;
        global_res.real_frequency /= (double) num_threads;
        global_res.bench_frequency /= (double) num_threads;
        global_res.ipc /= (double) num_threads;
        global_res.flop_cycle_sp /= (double) num_threads;
        global_res.flop_cycle_dp /= (double) num_threads;
        global_res.flops_sp /= (double) num_threads;
        global_res.flops_dp /= (double) num_threads;

    }

    cout << endl;
    cout << "-------------------  FREQUENCES SUMMARY ------------------------" << endl;
    cout << "+ Base frequency is " << global_res.base_frequency << "Ghz" << endl;
    cout << "+ Real frequency is " << global_res.real_frequency << "Ghz" << endl;
    cout << "+ Bench frequency is " << global_res.bench_frequency << "Ghz" << endl;

    if (global_res.base_frequency - global_res.real_frequency > 0.05) {
        cout << "+ /!\\ The frequency seems to be capped: -" << (1 - global_res.ipc) * 100 << '%' << endl;
    } else if (global_res.real_frequency - global_res.base_frequency > 0.05) {
        cout << "+ /!\\ Turbo seems to be ON: +" << global_res.ipc << '%' << " (be carful with the following values)" << endl;
    } else{
        cout << "+ OK: the core is running at his frequency based value" << endl;
    }

    cout << endl;

    cout << "------------------  INSTRUCTIONS SUMMARY -----------------------" << endl;
    double inst_second = global_res.ipc * global_res.bench_frequency;
    cout << setw(16) << "NB INSTRUCTIONS"         << setw(10) << "Time"                << setw(20) << "Giga_inst/sec" << setw(10) << "IPC"          << endl;
    cout << setw(16) << global_res.instructions   << setw(10) << global_res.time_total << setw(20) << inst_second     << setw(10) << global_res.ipc << endl;
    cout << endl;

    cout << "----------------------  FLOP SUMMARY  --------------------------" << endl;
    cout << setw(10) << "PRECISION"  << setw(15) << "FLOP/cycle"              << setw(20) << "FLOP/second"       << endl;
    cout << setw(10) << "Single"     << setw(15) << global_res.flop_cycle_sp  << setw(20) << global_res.flops_sp << endl;
    cout << setw(10) << "Double"     << setw(15) << global_res.flop_cycle_dp  << setw(20) << global_res.flops_dp << endl;

    return 0;

}

void bench(bench_result* result) {

    uint64_t cycleInStart, cycleInEnd;
    uint64_t cycle_total = 0, instructions_total = 0, instructions_executed = 0, instructions_executed_total = 0, loop_nb_instruction = 0, nb_total_loop_iteration = 0;
    double IPC/*, inst_second = 0.0*/;
    int i;
    double timeStart, timeEnd, time_total = 0;

    vector<pair<int, double>> pairVec;

    for (i = 0; i < NB_lOOP; i++) {
        timeStart = mygettime();
        cycleInStart = rdtsc();