#

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <stdbool.h>

#

#define NITE 100
#define NLOOPS 40000000UL
//#define NLOOPS 400UL
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
#define FREQ_TURBO_AVX2_256     true


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


void usage() {
    fprintf(stderr, "Usage: ./tool_freq [-ilwT] [file...]\n");
}

enum {
    TURBO_ON, TURBO_OFF,        //P_TURBO
    SCALAR, SSE, AVX,           //P_SIMD
    ADD, MUL, FMA               //P_OPRATION
};
const char *paramName[] = {
        "ON", "OFF",
        "SCALAR","SSE","AVX",
        "ADD","MUL","FMA"
};

int P_TURBO = TURBO_OFF;
int P_SIMD = SCALAR;
int P_OPERATION_TYPE = ADD;
int P_OPERATION_NB = 1;
int P_BIND = 0;
int P_WIDTH = 64;
bool P_VERBOSE = false;

void parse_arguments(int argc, char **argv) {
    char option;
    while ((option = getopt(argc, argv, "T:P:B:O:N:W:v")) != -1) {
        int ioptarg;
        switch (option) {
            case 'T':
                if (!strcmp(optarg, paramName[TURBO_ON])) {
                    P_TURBO = TURBO_ON;
                } else if (!strcmp(optarg, paramName[TURBO_OFF])) {
                    P_TURBO = TURBO_OFF;
                } else {
                    printf("/!\\ WRONG TURBO OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'P':
                if (!strcmp(optarg, paramName[SCALAR])) {
                    P_SIMD = SCALAR;
                } else if (!strcmp(optarg, paramName[SSE])) {
                    P_SIMD = SSE;
                } else if (!strcmp(optarg, paramName[AVX])) {
                    P_SIMD = AVX;
                } else {
                    printf("/!\\ WRONG PARALLELISM OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'B':
                P_BIND = atoi(optarg);
                break;
            case 'W':
                ioptarg = atoi(optarg);
                if (ioptarg == 64 || ioptarg == 128 || ioptarg == 256 || ioptarg == 512) {
                    P_WIDTH = ioptarg;
                }
                 else {
                    printf("/!\\ WRONG WIDTH OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'N':
                ioptarg = atoi(optarg);
                if (ioptarg == 1 || ioptarg == 2 || ioptarg == 3) {
                    P_OPERATION_NB = ioptarg;
                }
                 else {
                    printf("/!\\ WRONG NUMBER OF OPERATION OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                P_VERBOSE = true;
                break;
            case 'O':
                if (!strcmp(optarg, paramName[ADD])) {
                    P_OPERATION_TYPE = ADD;
                } else if (!strcmp(optarg, paramName[MUL])) {
                    P_OPERATION_TYPE = MUL;
                } else if (!strcmp(optarg, paramName[FMA])) {
                    P_OPERATION_TYPE = FMA;
                } else {
                    printf("/!\\ WRONG OPERATION OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }


}

void summary() {
    printf("The frequency tool was launched with:\n");
    printf("\t -T (turbo):          %s\n", paramName[P_TURBO]);
    printf("\t -P (parallelism)     %s\n", paramName[P_SIMD]);
    printf("\t -W (width)           %d\n", P_WIDTH);
    printf("\t -O (type operation)  %s\n", paramName[P_OPERATION_TYPE]);
    printf("\t -N (nb operation)    %d\n", P_OPERATION_NB);
    printf("\t -B (core binding)    %d\n", P_BIND);
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

    parse_arguments(argc, argv);
    if (P_VERBOSE) summary();

    return 0;


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