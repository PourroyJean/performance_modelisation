
float check_frequency(bench_result* result) {

    if (!is_check_freq) {
        return 1;
    }

    int i;
    double timeStart, timeEnd, time_total, freq_Base;
    unsigned long long int cycleInStart, cycleInEnd;
    int N_LOOP = 5;

    //********************
    //** BASE FREQUENCY  *
    //********************
    timeStart = mygettime();
    cycleInStart = rdtsc();
    usleep(10000);
    cycleInEnd = rdtsc();
    timeEnd = mygettime();
    unsigned long long int cycleSpent = (cycleInEnd - cycleInStart);

    freq_Base = cycleSpent / (1000000000 * (timeEnd - timeStart));

    //********************
    //** REAL FREQUENCY  *
    //********************
    cycleInStart = rdtsc();
    for (i = 0; i < N_LOOP; i++) {
        __asm__ ("aloop%=: "
                "       sub    $0x1,%%eax\n"
                "       sub    $0x1,%%eax\n"
                "       sub    $0x1,%%eax\n"
                "       sub    $0x1,%%eax\n"
                "       sub    $0x1,%%eax\n"
                "       sub    $0x1,%%eax\n"
                "       sub    $0x1,%%eax\n"
                "       sub    $0x1,%%eax\n"
                "       jnz    aloop%=" : : "a" (40000000UL)
        );
    }

    cycleInEnd = rdtsc();
    cycleSpent = (cycleInEnd - cycleInStart);

    unsigned long long int NbInstruction = (double) N_LOOP * (double) 40000000UL;
    float ipc = NbInstruction / (double) cycleSpent; //Should be equal to 1: 1 inst. per cycle
    float freq_Real = freq_Base * ipc;

    result->base_frequency = freq_Base;
    result->real_frequency = freq_Real;

    return ipc;

}