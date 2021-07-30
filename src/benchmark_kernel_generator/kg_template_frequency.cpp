

float check_frequency() {
    if (!is_check_freq) {
        return 1;
    }

    cout << "--------------------  CHECK FREQUENCY  ------------------------" << endl;

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
    cout << "+ Base      frequency is " << freq_Base << "Ghz" << endl;


    //********************
    //** REAL FREQUENCY  *
    //********************
    cycleInStart = rdtsc();
    for (i = 0; i < N_LOOP; i++) {
        __asm__ ("aloop%=: "
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloop%=" : : "a" (40000000UL)
        );
    }
    cycleInEnd = rdtsc();
    cycleSpent = (cycleInEnd - cycleInStart);

    unsigned long long int NbInstruction = (double) N_LOOP * (double) 40000000UL;
    float ipc = NbInstruction / (double) cycleSpent; //Should be equal to 1: 1 inst. per cycle
    float freq_Real = freq_Base * ipc;
    cout << "+ Current   frequency is " << freq_Real << "Ghz" << endl;



    //***************************************************
    //** OUTPUT: warning if the frequency is different  *
    //***************************************************
    if (freq_Base - freq_Real > 0.05) {
        cout << "+ /!\\ The frequency seems to be capped: -" << (1-ipc)*100 <<'%' <<  endl;
    } else if (freq_Real - freq_Base > 0.05) {
        cout << "+ /!\\ Turbo seems to be ON: +" << ipc  <<'%'  << " (be carful with the following values)"<< endl;
    } else{
        cout << "+ OK: the core is running at his frequency based value" << endl;
    }
    cout << endl << flush;



    return  ipc;
}