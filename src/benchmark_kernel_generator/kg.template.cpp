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
#include <chrono>
#include <thread>

#ifdef _OPENMP
#include "omp.h"
#endif

using namespace std;

struct bench_result {

    uint64_t base_frequency;
    uint64_t bench_frequency;
    double duration;
    uint64_t instructions;
    uint64_t cycles;
    double ipc;
    uint64_t flop_cycle_sp;
    uint64_t flop_cycle_dp;
    uint64_t flops_sp;
    uint64_t flops_dp;

    bench_result(): 
        base_frequency(0),
        bench_frequency(0),
        duration(0),
        instructions(0),
        cycles(0),
        ipc(0),
        flop_cycle_sp(0),
        flop_cycle_dp(0),
        flops_sp(0),
        flops_dp(0) {}

};

void bench(bench_result* result);

inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (uint64_t) hi << 32 | lo;
}

inline chrono::steady_clock::time_point instant() {
    return chrono::steady_clock::now();
}

int main(int argc, char **argv) {

    cout.precision(3);

    bench_result global_res;

#ifndef _OPENMP

    int cpu_bind = 0;
    if (argc >= 2) {
        cpu_bind = stoi(argv[1]);
    }

    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(cpu_bind, &cpus);
    sched_setaffinity(0, sizeof(cpu_set_t), &cpus);
    cout << "+ Bound to CPU " << cpu_bind << endl;

    bench(&global_res);

#else

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
            global_res.bench_frequency += res.bench_frequency;
            global_res.duration += res.duration;
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
    global_res.bench_frequency /= (double) num_threads;
    global_res.duration /= (double) num_threads;
    global_res.ipc /= (double) num_threads;
    global_res.flop_cycle_sp /= (double) num_threads;
    global_res.flop_cycle_dp /= (double) num_threads;
    global_res.flops_sp /= (double) num_threads;
    global_res.flops_dp /= (double) num_threads;

#endif

    cout << endl;
    cout << "-------------------  FREQUENCES SUMMARY ------------------------" << endl;
    cout << "+ Base frequency is " << (double) global_res.base_frequency / 1e9 << "GHz" << endl;
    cout << "+ Bench frequency is " << (double) global_res.bench_frequency / 1e9 << "GHz" << endl;

    // if (global_res.base_frequency - global_res.real_frequency > 0.05) {
    //     cout << "+ /!\\ The frequency seems to be capped: -" << (1 - global_res.ipc) * 100 << '%' << endl;
    // } else if (global_res.real_frequency - global_res.base_frequency > 0.05) {
    //     cout << "+ /!\\ Turbo seems to be ON: +" << global_res.ipc << '%' << " (be carful with the following values)" << endl;
    // } else{
    //     cout << "+ OK: the core is running at his frequency based value" << endl;
    // }

    cout << endl;

    cout << "------------------  INSTRUCTIONS SUMMARY -----------------------" << endl;
    double inst_second = global_res.ipc * global_res.bench_frequency;
    cout << setw(16) << "NB INSTRUCTIONS"         << setw(10) << "Duration"          << setw(20) << "Giga_inst/sec" << setw(10) << "IPC"          << endl;
    cout << setw(16) << global_res.instructions   << setw(10) << global_res.duration << setw(20) << inst_second     << setw(10) << global_res.ipc << endl;
    cout << endl;

    cout << "----------------------  FLOP SUMMARY  --------------------------" << endl;
    cout << setw(10) << "PRECISION"  << setw(15) << "FLOP/cycle"              << setw(20) << "FLOP/second"       << endl;
    cout << setw(10) << "Single"     << setw(15) << global_res.flop_cycle_sp  << setw(20) << (double) global_res.flops_sp << endl;
    cout << setw(10) << "Double"     << setw(15) << global_res.flop_cycle_dp  << setw(20) << (double) global_res.flops_dp << endl;

    return 0;

}

void bench(bench_result* result) {

    // Here we measure the invariant base frequency
    auto start_time = instant();
    uint64_t start_tsc = rdtsc();
    this_thread::sleep_for(chrono::milliseconds(100));
    uint64_t stop_tsc = rdtsc();
    auto stop_time = instant();
    uint64_t base_freq = (double) (stop_tsc - start_tsc) / chrono::duration_cast<chrono::duration<double>>(stop_time - start_time).count();

    chrono::steady_clock::duration total_bench_duration = chrono::steady_clock::duration::zero();
    uint64_t total_bench_freq = 0;
    uint64_t total_bench_cycles = 0;
    double total_bench_ipc = 0;

    for (int i = 0; i < NB_lOOP; i++) {

        // Actual benchmark
        auto bench_start_time = instant();
        uint64_t bench_start_tsc = rdtsc();

        #pragma kg asm

        uint64_t bench_stop_tsc = rdtsc();
        auto bench_stop_time = instant();
        
        // Frequency adjustment
        uint64_t freq_sub_count = 10000;
        uint64_t freq_start_tsc = rdtsc();

        __asm__ __volatile__ (
            ".loop: "
                "sub $0x1, %%eax\n"
                "sub $0x1, %%eax\n"
                "sub $0x1, %%eax\n"
                "sub $0x1, %%eax\n"
                "sub $0x1, %%eax\n"
                "sub $0x1, %%eax\n"
                "sub $0x1, %%eax\n"
                "sub $0x1, %%eax\n"
                "jnz .loop" 
                : 
                : "a" (freq_sub_count));

        uint64_t freq_stop_tsc = rdtsc();

        // The measure of elapsed TSC is not affected by frequency
        // changes, therefore because we expect this to be approx
        // "freq_sub_count" cycles, we can adjust the number of cycles
        // measured on the actual benchmark.
        double freq_adjustment = (double) freq_sub_count / (freq_stop_tsc - freq_start_tsc);

        uint64_t bench_freq = base_freq * freq_adjustment;
        uint64_t bench_cycles = (bench_stop_tsc - bench_start_tsc) / freq_adjustment;
        double bench_ipc = (double) (NB_INST * NB_lOOP_IN * P_UNROLLING) / bench_cycles;
        double bench_duration = chrono::duration_cast<chrono::duration<double>>(bench_stop_time - bench_start_time).count();

        result->bench_frequency += bench_freq;
        result->duration += bench_duration;
        result->cycles += bench_cycles;
        result->ipc += bench_ipc;

        result->flop_cycle_sp += (uint64_t) ((double) (NB_lOOP_IN * FLOP_SP_PER_LOOP) / bench_cycles);
        result->flop_cycle_dp += (uint64_t) ((double) (NB_lOOP_IN * FLOP_DP_PER_LOOP) / bench_cycles);
        result->flops_sp += (uint64_t) ((double) (NB_lOOP_IN * FLOP_SP_PER_LOOP) / bench_duration);
        result->flops_dp += (uint64_t) ((double) (NB_lOOP_IN * FLOP_DP_PER_LOOP) / bench_duration);
        
    }

    result->base_frequency = base_freq;
    result->bench_frequency /= NB_lOOP;
    result->duration /= NB_lOOP;
    result->cycles /= NB_lOOP;
    result->ipc /= NB_lOOP;
    result->flop_cycle_sp /= NB_lOOP;
    result->flop_cycle_dp /= NB_lOOP;
    result->flops_sp /= NB_lOOP;
    result->flops_dp /= NB_lOOP;

    result->instructions = (uint64_t) NB_INST * NB_lOOP_IN * P_UNROLLING * NB_lOOP;

}
