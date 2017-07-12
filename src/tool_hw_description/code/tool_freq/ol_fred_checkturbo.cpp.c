//
// Created by Jean Pourroy on 27/02/2017.
//

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

void print_res(const char *test, double ipc) {
    printf("%s\t%d\t%4.2f\t\t\t%u", test, tool_freq_parameters->P_BIND, ipc, (unsigned int) ((double) freq * ipc));
    if (ipc > 1.0001)
        printf("\t\t(Turbo ON)");
    else
        printf("\t\t(Turbo OFF)");

    printf("\n");
}
