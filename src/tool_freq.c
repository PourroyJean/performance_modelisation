#

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#

#define NITE 100
//#define NLOOPS 40000000UL
#define NLOOPS 400UL
#

//Select which mode you want to test OK | KO
//#define FREQ_TURBO_ON           true
//#define FREQ_TURBO_OFF          true
////AVX
//#define FREQ_TURBO_AVX_64       true
//#define FREQ_TURBO_AVX_128      true
//#define FREQ_TURBO_AVX_256      true
////AVX2
//#define FREQ_TURBO_AVX2_64      true
//#define FREQ_TURBO_AVX2_128     true
//#define FREQ_TURBO_AVX2_256     true


//Select which mode you want to test OK | KO
#define FREQ_TURBO_ON           false
#define FREQ_TURBO_OFF          false
//AVX
#define FREQ_TURBO_AVX_64       false
#define FREQ_TURBO_AVX_128      false
#define FREQ_TURBO_AVX_256      true
//AVX2
#define FREQ_TURBO_AVX2_64      false
#define FREQ_TURBO_AVX2_128     false
#define FREQ_TURBO_AVX2_256     false


void aloop(unsigned int n);

uint64_t rdtsc();

unsigned int freq;
int mycpu;

double mygettime();

void print_res(char *test, double ipc) {
    printf("%s\t%d\t%4.2f\t\t\t%u", test, mycpu, ipc, (unsigned int) ((double) freq * ipc));
    if (ipc > 1.0001)
        printf("\t\t(Turbo ON)");
    else
        printf("\t\t(Turbo OFF)");

    printf("\n");
}


void cpu_binding() {
    int i = 0;
    cpu_set_t mycpumask;

    CPU_ZERO(&mycpumask); //Clears set, so that it contains no CPUs.
    if (mycpu >= 0) {
        CPU_SET(mycpu, &mycpumask); //Add CPU cpu to set
        sched_setaffinity(0, sizeof(cpu_set_t), &mycpumask);
    };
    /* double-check */
    sched_getaffinity(0, sizeof(cpu_set_t), &mycpumask);
    for (i = 0; i < sysconf(_SC_NPROCESSORS_CONF); i++) {
        if (CPU_ISSET(i, &mycpumask)) printf("+ Running on CPU #%d\n", i);
    };
    return;
}

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
    printf("+ Estimating 'native' frequency to    %u MHz\n", freq);

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


int main(int argc, char **argv) {
    int i = 0;
    mycpu = 0;
    cpu_set_t mycpumask;
    unsigned int time;
    unsigned int res;
    uint64_t rtcstart, rtcend;
    double tstart, tend;
    float ipc;


    printf("TEST:\n");
    printf("FREQ_TURBO_ON           %s \n", (FREQ_TURBO_ON ? "TRUE" : "FALSE"));
    printf("FREQ_TURBO_OFF          %s \n", (FREQ_TURBO_OFF ? "TRUE" : "FALSE"));
    printf("FREQ_TURBO_AVX_64       %s \n", (FREQ_TURBO_AVX_64 ? "TRUE" : "FALSE"));
    printf("FREQ_TURBO_AVX_128      %s \n", (FREQ_TURBO_AVX_128 ? "TRUE" : "FALSE"));
    printf("FREQ_TURBO_AVX_256      %s \n", (FREQ_TURBO_AVX_256 ? "TRUE" : "FALSE"));
    printf("FREQ_TURBO_AVX2_64      %s \n", (FREQ_TURBO_AVX2_64 ? "TRUE" : "FALSE"));
    printf("FREQ_TURBO_AVX2_128     %s \n", (FREQ_TURBO_AVX2_128 ? "TRUE" : "FALSE"));
    printf("FREQ_TURBO_AVX2_256     %s \n", (FREQ_TURBO_AVX2_256 ? "TRUE" : "FALSE"));



    /* Retrieve which CPU to bind to */
    if (argv[1]) mycpu = atoi(argv[1]);
    cpu_binding();


    /* Compute native frequency */
    native_frequency();


    /* Go through each test selected*/
    printf("OPERARTION\tCORE_ID\tIPC (Expect 1.0) \tFreq (Mhz) \tTurbo\n");
//    if (FREQ_TURBO_ON) { freq_turbo_on(); }
//    if (FREQ_TURBO_OFF) { freq_turbo_off(); }
    if (FREQ_TURBO_AVX_64) { freq_turbo_avx_64(); }
    if (FREQ_TURBO_AVX_128) { freq_turbo_avx_128(); }
    if (FREQ_TURBO_AVX_256) { freq_turbo_avx_256(); }
    if (FREQ_TURBO_AVX2_64) { freq_turbo_avx2_64(); }
    if (FREQ_TURBO_AVX2_128) { freq_turbo_avx2_128(); }
    if (FREQ_TURBO_AVX2_256) { freq_turbo_avx2_256(); }

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