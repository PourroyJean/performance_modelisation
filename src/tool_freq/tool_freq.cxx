#define _GNU_SOURCE             /* See feature_test_macros(7) */

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <iostream>
#include <iomanip>
#include "tool_freq_parameters.h"
#include "tool_freq_generators.h"
#include "tool_freq_misc.h"
#

#define NITE 100
//#define NLOOPS 40000000UL
#define NLOOPS 4000UL
#




const  char * paramName [1];
Tool_freq_parameters * tool_freq_parameters;

void aloop(unsigned int n);

uint64_t rdtsc();

unsigned int freq;
int mycpu;

double mygettime();


void native_frequency() {
    unsigned int time;
    uint64_t rtcstart, rtcend;
    double tstart, tend;


    tstart = mygettime();
    rtcstart = rdtsc();
    usleep(1000000);
    rtcend = rdtsc();
    tend = mygettime();
    time = rtcend - rtcstart;
    freq = time / (1000000 * (tend - tstart));
    printf("NATIVE_FREQUENCY %u MHz\n", freq);

}

void print_res(const char *test, double ipc) {
    printf("%s\t%d\t%4.2f\t\t\t%u", test, tool_freq_parameters->P_BIND, ipc, (unsigned int) ((double) freq * ipc));
    if (ipc > 1.0001)
        printf("\t\t(Turbo ON)");
    else
        printf("\t\t(Turbo OFF)");

    printf("\n");
}


void freq_scalar_64_add() {
    unsigned int time;
    uint64_t rtcstart, rtcend;
    int i;
    double ipc;

    //On espere 10 cycles
    for (i = 0; i < NITE; i++) {
        rtcstart = rdtsc();
        __asm__ ("aloopqq164: "
                "       add    $0x1,%%ebx;"
                "       add    $0x1,%%ebx;"
                "       add    $0x1,%%ebx;"
                "       add    $0x1,%%ebx;"
                "       add    $0x1,%%ebx;"
                "       add    $0x1,%%ebx;"
                "       add    $0x1,%%ebx;"
                "       add    $0x1,%%ebx;"
                "       add    $0x1,%%ebx;"
                "       add    $0x1,%%ebx;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloopqq164" : : "a" (NLOOPS)
        );
        rtcend = rdtsc();
    }
    time = rtcend - rtcstart;
    ipc = (double) NLOOPS *10 / (double) time;
    print_res("1 ADD par CYCLE  ", ipc);

}

void freq_turbo_avx_64() {
    unsigned int time;
    uint64_t rtcstart, rtcend;
    int i;
    double ipc;

    /* Make the CPU execute a given number of basic instructions,
     expected to be executed in 8 cycles */
    for (i = 0; i < NITE; i++) {
        rtcstart = rdtsc();
        __asm__ ("aloop164: "
                "       vaddsd %%xmm0,%%xmm1,%%xmm2;"
                "       vmulsd %%xmm3,%%xmm4,%%xmm5;"
                "       sub    $0x1,%%eax;"
                "       vaddsd %%xmm0,%%xmm1,%%xmm2;"
                "       vmulsd %%xmm3,%%xmm4,%%xmm5;"
                "       sub    $0x1,%%eax;"
                "       vaddsd %%xmm0,%%xmm1,%%xmm2;"
                "       vmulsd %%xmm3,%%xmm4,%%xmm5;"
                "       sub    $0x1,%%eax;"
                "       vaddsd %%xmm0,%%xmm1,%%xmm2;"
                "       vmulsd %%xmm3,%%xmm4,%%xmm5;"
                "       sub    $0x1,%%eax;"
                "       vaddsd %%xmm0,%%xmm1,%%xmm2;"
                "       vmulsd %%xmm3,%%xmm4,%%xmm5;"
                "       sub    $0x1,%%eax;"
                "       vaddsd %%xmm0,%%xmm1,%%xmm2;"
                "       vmulsd %%xmm3,%%xmm4,%%xmm5;"
                "       sub    $0x1,%%eax;"
                "       vaddsd %%xmm0,%%xmm1,%%xmm2;"
                "       vmulsd %%xmm3,%%xmm4,%%xmm5;"
                "       sub    $0x1,%%eax;"
                "       vaddsd %%xmm0,%%xmm1,%%xmm2;"
                "       vmulsd %%xmm3,%%xmm4,%%xmm5;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloop164" : : "a" (NLOOPS)
        );
        rtcend = rdtsc();
    }
    time = rtcend - rtcstart;
    ipc = (double) NLOOPS / (double) time;
    print_res("AVX_64  ", ipc);
}

void freq_turbo_avx_128() {
    unsigned int time;
    uint64_t rtcstart, rtcend;
    int i;
    double ipc;

    /* Make the CPU execute a given number of basic instructions,
     expected to be executed in 5 cycles */
    for (i = 0; i < NITE; i++) {
        rtcstart = rdtsc();
        __asm__ ("aloop1128: "
                "       vaddpd %%xmm0,%%xmm1,%%xmm2;"
                "       vmulpd %%xmm0,%%xmm1,%%xmm3;"
                "       sub    $0x1,%%eax;"
                "       vaddpd %%xmm0,%%xmm1,%%xmm4;"
                "       vmulpd %%xmm0,%%xmm1,%%xmm5;"
                "       sub    $0x1,%%eax;"
                "       vaddpd %%xmm0,%%xmm1,%%xmm6;"
                "       vmulpd %%xmm0,%%xmm1,%%xmm7;"
                "       sub    $0x1,%%eax;"
                "       vaddpd %%xmm0,%%xmm1,%%xmm8;"
                "       vmulpd %%xmm0,%%xmm1,%%xmm9;"
                "       sub    $0x1,%%eax;"
                "       vaddpd %%xmm0,%%xmm1,%%xmm10;"
                "       vmulpd %%xmm0,%%xmm1,%%xmm11;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloop1128" : : "a" (NLOOPS)
        );
        rtcend = rdtsc();
    }
    time = rtcend - rtcstart;
    ipc = (double) NLOOPS / (double) time;
    print_res("AVX_128 ", ipc);
}

void freq_turbo_avx_256() {
    unsigned int time;
    uint64_t rtcstart, rtcend;
    int i;
    double ipc;

    /* Make the CPU execute a given number of basic instructions,
    expected to be executed in 8 cycles */
    for (i = 0; i < NITE; i++) {
        rtcstart = rdtsc();
        __asm__ ("aloop1256: "
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       vaddpd %%ymm0,%%ymm1,%%ymm2;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloop1256" : : "a" (NLOOPS)
        );
        rtcend = rdtsc();
    }
    time = rtcend - rtcstart;
    ipc = (double) NLOOPS / (double) time;
    print_res("AVX_256 ", ipc);
}

void freq_turbo_avx2_64() {

    unsigned int time;
    uint64_t rtcstart, rtcend;
    int i;
    double ipc;

    /* Make the CPU execute a given number of basic instructions,
expected to be executed in 8 cycles */
    for (i = 0; i < NITE; i++) {
        rtcstart = rdtsc();
        __asm__ ("aloop264: "
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       vfmadd213sd %%xmm0,%%xmm1,%%xmm2;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloop264" : : "a" (NLOOPS)
        );
        rtcend = rdtsc();
    }
    time = rtcend - rtcstart;
    ipc = (double) NLOOPS / (double) time;
    print_res("AVX2_64 ", ipc);

}


void freq_turbo_avx2_128() {
    unsigned int time;
    uint64_t rtcstart, rtcend;
    int i;
    double ipc;

    /* Make the CPU execute a given number of basic instructions,
    expected to be executed in 8 cycles */
    for (i = 0; i < NITE; i++) {
        rtcstart = rdtsc();
        __asm__ ("aloop2128: "
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       vfmadd213pd %%xmm0,%%xmm1,%%xmm2;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloop2128" : : "a" (NLOOPS)
        );
        rtcend = rdtsc();
    }
    time = rtcend - rtcstart;
    ipc = (double) NLOOPS / (double) time;
    print_res("AVX2_128", ipc);

}

void freq_turbo_avx2_256() {
    unsigned int time;
    uint64_t rtcstart, rtcend;
    int i;
    double ipc;

    /* Make the CPU execute a given number of basic instructions,
    expected to be executed in 8 cycles */
    for (i = 0; i < NITE; i++) {
        rtcstart = rdtsc();
        __asm__ ("aloop2256: "
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       vfmadd213pd %%ymm0,%%ymm1,%%ymm2;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloop2256" : : "a" (NLOOPS)
        );
        rtcend = rdtsc();
    }
    time = rtcend - rtcstart;
    ipc = (double) NLOOPS / (double) time;
    print_res("AVX2_256", ipc);

}










void cpu_binding() {
    int i = 0;
//    cpu_set_t mycpumask;
//
//    CPU_ZERO(&mycpumask); //Clears set, so that it contains no CPUs.
//    if (P_BIND >= 0) {
//        CPU_SET(P_BIND, &mycpumask); //Add CPU cpu to set
//        sched_setaffinity(0, sizeof(cpu_set_t), &mycpumask);
//    };
//    /* double-check */
//    sched_getaffinity(0, sizeof(cpu_set_t), &mycpumask);
//    for (i = 0; i < sysconf(_SC_NPROCESSORS_CONF); i++) {
//        if (CPU_ISSET(i, &mycpumask)) printf("+ Running on CPU #%d\n", i);
//    };
    return;
}


int main(int argc, char **argv) {
    int i = 0;
    mycpu = 0;
//    cpu_set_t mycpumask;
    unsigned int time;
    unsigned int res;
    uint64_t rtcstart, rtcend;
    double tstart, tend;
    float ipc;

    //----------- ARGUMENT PARSING --------------
    Tool_freq_parameters * tool_freq_parameters = new Tool_freq_parameters();
    tool_freq_parameters->parse_arguments(argc, argv);
    tool_freq_parameters->check_arguments();


    //------------ CODE GENERATION  -------------
    Tool_freq_generators * generator = new Tool_freq_generators (tool_freq_parameters);
    generator->Generate_code();



    //------------ ASSEMBLY COMPILATION ---------
    system("bash -c \"g++ -o " ASM_FILE_exe " " ASM_FILE_source  "\"");

    //----------- BINDING ----------------------
    //We let the kernel bind the process himself if no binding are set
    if (tool_freq_parameters->P_BIND == -1) cpu_binding();


    //----------- EXECUTING --------------------

    rtcstart = rdtsc();
    system("./" ASM_FILE_exe);
    rtcend = rdtsc();
    time = rtcend - rtcstart;
    long long int NbInstruction = tool_freq_parameters->P_OPERATIONS.size();

//    float IPC = (float)a / (float)b;
    float IPC = (float) (NbInstruction * BENCH_NB_ITERATION) / (float)time;

    cout << " Nb Instruction    " << NbInstruction * BENCH_NB_ITERATION << endl
         << " Nb Cycle          " << time << endl
         << " IPC               " ;
    cout<< fixed <<setprecision(10) <<  IPC << endl;



    return 0;


    native_frequency();


    return 0;
}

uint64_t rdtsc() {
    uint32_t lo, hi;
    /* We cannot use "=A", since this would use %rax on x86_64 */
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