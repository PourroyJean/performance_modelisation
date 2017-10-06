

float check_frequency() {
    if (!is_check_freq) {
        return 1;
    }

    cout << "--------------------  CHECK FREQUENCY  ------------------------" << endl;

    int i;
    double timeStart, timeEnd, time_total, freq;
    unsigned long long int cycleInStart, cycleInEnd;

    unsigned int freq2;
    timeStart = mygettime();
    cycleInStart = rdtsc();
    usleep(10000);
    cycleInEnd = rdtsc();
    timeEnd = mygettime();
    unsigned long long int time = (cycleInEnd - cycleInStart);
    freq = time / (1000000000 * (timeEnd - timeStart));
    cout << "+ Base      frequency is " << freq << "Ghz" << endl;

    cycleInStart = rdtsc();
    for (i = 0; i < 20; i++) {
        __asm__ ("aloop: "
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       sub    $0x1,%%eax;"
                "       jnz    aloop" : : "a" (40000000UL)
        );
    }
    cycleInEnd = rdtsc();
    time = (cycleInEnd - cycleInStart);
    float ipc = (double) 20 * (double) 40000000UL / (double) time;
    float real_freq = freq * ipc;
    cout << "+ Current   frequency is " << real_freq << "Ghz" << endl;
    float Base_vs_Current_freq;
    if (freq - real_freq > 0.05) {
        cout << "+ /!\\ The frequence seems to be capped: x" << (1-ipc) <<  endl;
    } else if (real_freq - freq > 0.05) {
        cout << "+ /!\\ Turbo seems to be ON: x" << ipc << endl;
    } else{
        cout << "+ OK: the core is running at his frequency based value" << endl;
        Base_vs_Current_freq = 1;
    }



    if (Base_vs_Current_freq < 0.98){
//        cout << "+      FREQ is mutliplied by " << Base_vs_Current_freq << endl;
//        cout << "+      IPC  is mutliplied by " << (2 - Base_vs_Current_freq)<< endl;
//        freq = freq * Base_vs_Current_freq;
//        IPC  = IPC  * (2 - Base_vs_Current_freq);

    }

    else if (Base_vs_Current_freq > 1.02){
//        cout << "+      FREQ is mutliplied by " << Base_vs_Current_freq << endl;
//        cout << "+      IPC  is mutliplied by " << Base_vs_Current_freq << endl;
//        freq = freq * Base_vs_Current_freq;
//        IPC  = IPC  * (2 - Base_vs_Current_freq);


    }


    return  ipc;
}